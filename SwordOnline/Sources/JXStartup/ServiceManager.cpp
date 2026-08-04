#include "StdAfx.h"
#include "ServiceManager.h"
#include "resource.h"
#include <psapi.h>
#include <tlhelp32.h>

namespace
{
	void PostLogLine(HWND hWnd, int id, const std::string& line)
	{
		char* copy = _strdup(line.c_str());
		if (copy)
			::PostMessageA(hWnd, WM_APP_LOGLINE, (WPARAM)id, (LPARAM)copy);
	}

	void CloseServiceHandles(SERVICE_ENTRY& svc)
	{
		if (svc.procInfo.hThread) { ::CloseHandle(svc.procInfo.hThread); svc.procInfo.hThread = nullptr; }
		if (svc.procInfo.hProcess) { ::CloseHandle(svc.procInfo.hProcess); svc.procInfo.hProcess = nullptr; }
		if (svc.hLogThread) { ::CloseHandle(svc.hLogThread); svc.hLogThread = nullptr; }
		if (svc.hStopEvent)
		{
			// Wake the log-tail thread (it holds its own duplicate handle and closes that
			// one itself); our handle is independent and safe to close right after.
			::SetEvent(svc.hStopEvent);
			::CloseHandle(svc.hStopEvent);
			svc.hStopEvent = nullptr;
		}
		// hStdOutRead is owned and closed by the reader thread itself once the pipe breaks.
		svc.hStdOutRead = nullptr;
	}

	BOOL CALLBACK EnumCloseWindowProc(HWND hWnd, LPARAM lParam)
	{
		DWORD pid = 0;
		::GetWindowThreadProcessId(hWnd, &pid);
		if (pid == (DWORD)lParam && ::IsWindowVisible(hWnd) && ::GetWindow(hWnd, GW_OWNER) == nullptr)
			::PostMessageA(hWnd, WM_CLOSE, 0, 0);
		return TRUE;
	}

	void RequestCloseTopWindows(DWORD pid)
	{
		::EnumWindows(&EnumCloseWindowProc, (LPARAM)pid);
	}

	// True if a process with this exe name is already running anywhere on the
	// system (e.g. started by hand, or left over from a previous JXStartup
	// session that exited without stopping it). Prevents launching a duplicate
	// that would fight the original over the same ports.
	bool IsExeRunningExternally(const std::string& exeFileName)
	{
		HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap == INVALID_HANDLE_VALUE)
			return false;

		PROCESSENTRY32 pe;
		ZeroMemory(&pe, sizeof(pe));
		pe.dwSize = sizeof(pe);

		bool found = false;
		if (::Process32First(hSnap, &pe))
		{
			do
			{
				if (lstrcmpiA(pe.szExeFile, exeFileName.c_str()) == 0)
				{
					found = true;
					break;
				}
			} while (::Process32Next(hSnap, &pe));
		}
		::CloseHandle(hSnap);
		return found;
	}

	struct READER_THREAD_PARAM
	{
		int serviceId;
		HANDLE hRead;
		HWND hNotifyWnd;
	};

	struct TAIL_THREAD_PARAM
	{
		int serviceId;
		std::string workDir;
		std::string logIdent;
		HWND hNotifyWnd;
		HANDLE hStopEvent;	// private duplicate, owned and closed by the tail thread
		FILETIME sinceUtc;	// ignore any log file created before this (leftover from a previous run)
	};

	// KGLog (Engine/Src/KGLog.cpp) writes to <workDir>logs/<ident>/<yyyy_mm_dd>/<ident>_<timestamp>.log,
	// truncating/recreating a new uniquely-named file every run. Only today's folder is checked;
	// that's the only one a freshly started process can possibly write to.
	std::string BuildTodayLogDir(const std::string& workDir, const std::string& logIdent)
	{
		SYSTEMTIME st;
		::GetLocalTime(&st);
		char buf[64];
		wsprintfA(buf, "logs\\%s\\%d_%02d_%02d\\", logIdent.c_str(), st.wYear, st.wMonth, st.wDay);
		return workDir + buf;
	}

	bool FindNewestLogFile(const std::string& dir, const std::string& logIdent, const FILETIME& sinceUtc, std::string& outPath)
	{
		std::string pattern = dir + logIdent + "_*.log";

		WIN32_FIND_DATAA fd;
		HANDLE hFind = ::FindFirstFileA(pattern.c_str(), &fd);
		if (hFind == INVALID_HANDLE_VALUE)
			return false;

		FILETIME bestTime = { 0, 0 };
		std::string bestName;
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				::CompareFileTime(&fd.ftCreationTime, &bestTime) > 0)
			{
				bestTime = fd.ftCreationTime;
				bestName = fd.cFileName;
			}
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);

		if (bestName.empty() || ::CompareFileTime(&bestTime, &sinceUtc) < 0)
			return false;

		outPath = dir + bestName;
		return true;
	}
}

CServiceManager::CServiceManager(HWND hNotifyWnd)
	: m_hNotifyWnd(hNotifyWnd)
{
	m_services.reserve(SERVICE_COUNT);
}

CServiceManager::~CServiceManager()
{
	for (auto& svc : m_services)
	{
		if (svc.procInfo.hProcess)
			::TerminateProcess(svc.procInfo.hProcess, 1);
		CloseServiceHandles(svc);
	}
}

void CServiceManager::SetWorkingDirectory(const std::string& dir)
{
	m_workDir = dir;
	if (!m_workDir.empty() && m_workDir.back() != '\\')
		m_workDir += '\\';
}

void CServiceManager::AddService(int id, const char* name, const char* exeFileName, ECaptureMode captureMode, const char* logIdent)
{
	SERVICE_ENTRY svc;
	svc.id = id;
	svc.name = name;
	svc.exeFileName = exeFileName;
	svc.captureMode = captureMode;
	if (logIdent)
		svc.logIdent = logIdent;
	m_services.push_back(svc);
}

SERVICE_ENTRY* CServiceManager::FindById(int id)
{
	for (auto& svc : m_services)
	{
		if (svc.id == id)
			return &svc;
	}
	return nullptr;
}

DWORD WINAPI CServiceManager::LogReaderThreadProc(LPVOID param)
{
	READER_THREAD_PARAM* p = (READER_THREAD_PARAM*)param;
	char buffer[4096];
	std::string pending;
	DWORD bytesRead = 0;

	while (::ReadFile(p->hRead, buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead > 0)
	{
		pending.append(buffer, bytesRead);

		size_t pos;
		while ((pos = pending.find('\n')) != std::string::npos)
		{
			std::string line = pending.substr(0, pos);
			if (!line.empty() && line.back() == '\r')
				line.pop_back();
			PostLogLine(p->hNotifyWnd, p->serviceId, line);
			pending.erase(0, pos + 1);
		}
	}

	if (!pending.empty())
		PostLogLine(p->hNotifyWnd, p->serviceId, pending);

	::CloseHandle(p->hRead);
	delete p;
	return 0;
}

DWORD WINAPI CServiceManager::LogTailThreadProc(LPVOID param)
{
	TAIL_THREAD_PARAM* p = (TAIL_THREAD_PARAM*)param;

	std::string dir = BuildTodayLogDir(p->workDir, p->logIdent);
	std::string filePath;

	// The server needs a moment after CreateProcess to reach KGLogInit() and open its
	// log file; poll for it briefly instead of failing immediately.
	ULONGLONG deadline = ::GetTickCount64() + 8000;
	for (;;)
	{
		if (FindNewestLogFile(dir, p->logIdent, p->sinceUtc, filePath))
			break;

		if (::GetTickCount64() >= deadline)
		{
			PostLogLine(p->hNotifyWnd, p->serviceId, "[JXStartup] Khong tim thay file log (server co the chua ghi kip).");
			::CloseHandle(p->hStopEvent);
			delete p;
			return 0;
		}

		if (::WaitForSingleObject(p->hStopEvent, 300) != WAIT_TIMEOUT)
		{
			::CloseHandle(p->hStopEvent);
			delete p;
			return 0;
		}
	}

	HANDLE hFile = ::CreateFileA(filePath.c_str(), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		char msg[512];
		wsprintfA(msg, "[JXStartup] Khong mo duoc file log (loi %lu): %s", ::GetLastError(), filePath.c_str());
		PostLogLine(p->hNotifyWnd, p->serviceId, msg);
		::CloseHandle(p->hStopEvent);
		delete p;
		return 0;
	}

	PostLogLine(p->hNotifyWnd, p->serviceId, "[JXStartup] Dang theo doi file log: " + filePath);

	std::string pending;
	char buffer[4096];

	for (;;)
	{
		DWORD bytesRead = 0;
		if (::ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead > 0)
		{
			pending.append(buffer, bytesRead);

			size_t pos;
			while ((pos = pending.find('\n')) != std::string::npos)
			{
				std::string line = pending.substr(0, pos);
				if (!line.empty() && line.back() == '\r')
					line.pop_back();
				PostLogLine(p->hNotifyWnd, p->serviceId, line);
				pending.erase(0, pos + 1);
			}
			continue;
		}

		if (::WaitForSingleObject(p->hStopEvent, 500) != WAIT_TIMEOUT)
			break;
	}

	::CloseHandle(hFile);
	::CloseHandle(p->hStopEvent);
	delete p;
	return 0;
}

bool CServiceManager::StartService(int id)
{
	SERVICE_ENTRY* svc = FindById(id);
	if (!svc)
		return false;
	if (svc->status == EServiceStatus::Running || svc->status == EServiceStatus::Starting)
		return true;

	std::string exePath = m_workDir + svc->exeFileName;
	if (::GetFileAttributesA(exePath.c_str()) == INVALID_FILE_ATTRIBUTES)
	{
		PostLogLine(m_hNotifyWnd, id, "[JXStartup] Khong tim thay file: " + exePath);
		return false;
	}

	if (IsExeRunningExternally(svc->exeFileName))
	{
		PostLogLine(m_hNotifyWnd, id,
			"[JXStartup] Da co tien trinh " + svc->exeFileName +
			" dang chay ngoai JXStartup (vi du: chay tay, hoac con sot lai tu lan truoc). "
			"Hay dong no (Task Manager) roi thu lai, de tranh 2 tien trinh tranh chap cong.");
		return false;
	}

	SYSTEMTIME nowSt;
	FILETIME sinceUtc;
	::GetSystemTime(&nowSt);
	::SystemTimeToFileTime(&nowSt, &sinceUtc);

	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	HANDLE hReadPipe = nullptr;
	HANDLE hWritePipe = nullptr;

	bool usePipe = (svc->captureMode == ECaptureMode::Pipe);

	if (usePipe)
	{
		SECURITY_ATTRIBUTES sa;
		ZeroMemory(&sa, sizeof(sa));
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;

		if (!::CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
		{
			PostLogLine(m_hNotifyWnd, id, "[JXStartup] Khong tao duoc pipe doc log.");
			return false;
		}
		::SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

		si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		si.hStdOutput = hWritePipe;
		si.hStdError = hWritePipe;
		si.hStdInput = nullptr;
		si.wShowWindow = SW_HIDE;
	}
	else if (svc->captureMode == ECaptureMode::HiddenConsole)
	{
		// Must NOT redirect stdio here: this server calls SetConsoleScreenBufferSize /
		// SetConsoleWindowInfo / SetConsoleTextAttribute on STD_OUTPUT_HANDLE during its
		// own startup, which only works on a real console handle, not a pipe. Give it a
		// real (CREATE_NEW_CONSOLE) console of its own and just hide the window instead.
		si.dwFlags |= STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}
	else // ECaptureMode::Window
	{
		si.dwFlags |= STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWNORMAL;
	}

	std::string cmdLine = "\"" + exePath + "\"";
	std::vector<char> cmdLineBuf(cmdLine.begin(), cmdLine.end());
	cmdLineBuf.push_back('\0');

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	DWORD creationFlags = CREATE_NEW_PROCESS_GROUP;
	if (usePipe)
		creationFlags |= CREATE_NO_WINDOW;
	else if (svc->captureMode == ECaptureMode::HiddenConsole)
		creationFlags |= CREATE_NEW_CONSOLE;

	BOOL ok = ::CreateProcessA(
		exePath.c_str(),
		cmdLineBuf.data(),
		nullptr, nullptr,
		usePipe ? TRUE : FALSE,
		creationFlags,
		nullptr,
		m_workDir.c_str(),
		&si, &pi);

	if (usePipe)
		::CloseHandle(hWritePipe);

	if (!ok)
	{
		if (hReadPipe)
			::CloseHandle(hReadPipe);

		char msg[256];
		wsprintfA(msg, "[JXStartup] Khong the khoi dong (loi %lu).", ::GetLastError());
		PostLogLine(m_hNotifyWnd, id, msg);

		svc->status = EServiceStatus::Crashed;
		::PostMessageA(m_hNotifyWnd, WM_APP_STATUSCHANGED, (WPARAM)id, 0);
		return false;
	}

	svc->procInfo = pi;
	svc->startTickMs = ::GetTickCount64();
	svc->status = EServiceStatus::Running;
	svc->hStdOutRead = hReadPipe;
	svc->forceKillDeadlineMs = 0;

	if (hReadPipe)
	{
		READER_THREAD_PARAM* p = new READER_THREAD_PARAM{ id, hReadPipe, m_hNotifyWnd };
		svc->hLogThread = ::CreateThread(nullptr, 0, &CServiceManager::LogReaderThreadProc, p, 0, nullptr);
	}
	else if (svc->captureMode == ECaptureMode::HiddenConsole && !svc->logIdent.empty())
	{
		svc->hStopEvent = ::CreateEventA(nullptr, TRUE, FALSE, nullptr);

		HANDLE hStopEventDup = nullptr;
		::DuplicateHandle(::GetCurrentProcess(), svc->hStopEvent, ::GetCurrentProcess(), &hStopEventDup, 0, FALSE, DUPLICATE_SAME_ACCESS);

		TAIL_THREAD_PARAM* p = new TAIL_THREAD_PARAM{ id, m_workDir, svc->logIdent, m_hNotifyWnd, hStopEventDup, sinceUtc };
		svc->hLogThread = ::CreateThread(nullptr, 0, &CServiceManager::LogTailThreadProc, p, 0, nullptr);
	}
	else if (svc->captureMode == ECaptureMode::HiddenConsole)
	{
		PostLogLine(m_hNotifyWnd, id,
			"[JXStartup] Server nay tu quan ly console rieng nen khong doc duoc log truc tiep tai day.");
	}

	PostLogLine(m_hNotifyWnd, id, "[JXStartup] Da khoi dong, PID = " + std::to_string(pi.dwProcessId));
	::PostMessageA(m_hNotifyWnd, WM_APP_STATUSCHANGED, (WPARAM)id, 0);
	return true;
}

bool CServiceManager::StopService(int id, bool force)
{
	SERVICE_ENTRY* svc = FindById(id);
	if (!svc)
		return false;
	if (svc->status != EServiceStatus::Running && svc->status != EServiceStatus::Starting)
		return true;

	if (force)
	{
		PostLogLine(m_hNotifyWnd, id, "[JXStartup] Buoc dung (force kill)...");
		::TerminateProcess(svc->procInfo.hProcess, 1);
		svc->forceKillDeadlineMs = 0;
	}
	else
	{
		PostLogLine(m_hNotifyWnd, id, "[JXStartup] Dang yeu cau dung...");
		if (svc->captureMode == ECaptureMode::Window)
			RequestCloseTopWindows(svc->procInfo.dwProcessId);
		else
			::GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, svc->procInfo.dwProcessId);

		svc->forceKillDeadlineMs = ::GetTickCount64() + 5000;
	}

	svc->status = EServiceStatus::Stopping;
	::PostMessageA(m_hNotifyWnd, WM_APP_STATUSCHANGED, (WPARAM)id, 0);
	return true;
}

void CServiceManager::Tick()
{
	ULONGLONG now = ::GetTickCount64();

	for (auto& svc : m_services)
	{
		if (svc.status != EServiceStatus::Running &&
			svc.status != EServiceStatus::Starting &&
			svc.status != EServiceStatus::Stopping)
			continue;

		if (svc.procInfo.hProcess)
		{
			DWORD code = 0;
			if (::GetExitCodeProcess(svc.procInfo.hProcess, &code) && code != STILL_ACTIVE)
			{
				bool wasStopping = (svc.status == EServiceStatus::Stopping);
				svc.exitCode = code;
				CloseServiceHandles(svc);
				svc.status = wasStopping ? EServiceStatus::Stopped : EServiceStatus::Crashed;

				PostLogLine(m_hNotifyWnd, svc.id, wasStopping
					? "[JXStartup] Da dung."
					: "[JXStartup] Tien trinh da thoat ngoai y muon! Ma loi = " + std::to_string(code));
				::PostMessageA(m_hNotifyWnd, WM_APP_STATUSCHANGED, (WPARAM)svc.id, 0);
				continue;
			}
		}

		if (svc.status == EServiceStatus::Stopping &&
			svc.forceKillDeadlineMs != 0 &&
			now >= svc.forceKillDeadlineMs)
		{
			PostLogLine(m_hNotifyWnd, svc.id, "[JXStartup] Qua thoi gian cho, buoc dung (force kill).");
			::TerminateProcess(svc.procInfo.hProcess, 1);
			svc.forceKillDeadlineMs = 0;
		}
	}

	UpdateResourceUsage();
}

void CServiceManager::UpdateResourceUsage()
{
	ULONGLONG nowMs = ::GetTickCount64();

	for (auto& svc : m_services)
	{
		if (svc.status != EServiceStatus::Running || !svc.procInfo.hProcess)
		{
			svc.cpuPercent = 0.0;
			svc.ramBytes = 0;
			svc.lastSampleTickMs = 0;
			svc.cpuWarningActive = false;
			svc.ramWarningActive = false;
			continue;
		}

		FILETIME creationTime, exitTime, kernelTime, userTime;
		if (!::GetProcessTimes(svc.procInfo.hProcess, &creationTime, &exitTime, &kernelTime, &userTime))
			continue;

		PROCESS_MEMORY_COUNTERS pmc;
		ZeroMemory(&pmc, sizeof(pmc));
		pmc.cb = sizeof(pmc);
		if (::K32GetProcessMemoryInfo(svc.procInfo.hProcess, &pmc, sizeof(pmc)))
			svc.ramBytes = pmc.WorkingSetSize;

		if (svc.lastSampleTickMs != 0)
		{
			ULONGLONG prevKernel = (((ULONGLONG)svc.lastKernelTime.dwHighDateTime) << 32) | svc.lastKernelTime.dwLowDateTime;
			ULONGLONG prevUser   = (((ULONGLONG)svc.lastUserTime.dwHighDateTime) << 32) | svc.lastUserTime.dwLowDateTime;
			ULONGLONG curKernel  = (((ULONGLONG)kernelTime.dwHighDateTime) << 32) | kernelTime.dwLowDateTime;
			ULONGLONG curUser    = (((ULONGLONG)userTime.dwHighDateTime) << 32) | userTime.dwLowDateTime;

			ULONGLONG cpuDelta100ns = (curKernel - prevKernel) + (curUser - prevUser);
			ULONGLONG wallDeltaMs = nowMs - svc.lastSampleTickMs;

			if (wallDeltaMs > 0)
			{
				double cpuMs = (double)cpuDelta100ns / 10000.0;
				svc.cpuPercent = (cpuMs / (double)wallDeltaMs) * 100.0;
			}
		}

		svc.lastKernelTime = kernelTime;
		svc.lastUserTime = userTime;
		svc.lastSampleTickMs = nowMs;

		if (svc.cpuPercent >= kCpuWarnPercent)
		{
			if (!svc.cpuWarningActive)
			{
				svc.cpuWarningActive = true;
				char msg[128];
				sprintf_s(msg, sizeof(msg), "[JXStartup] Canh bao: CPU dang o muc %.0f%%.", svc.cpuPercent);
				PostLogLine(m_hNotifyWnd, svc.id, msg);
			}
		}
		else
		{
			svc.cpuWarningActive = false;
		}

		if (svc.ramBytes >= kRamWarnBytes)
		{
			if (!svc.ramWarningActive)
			{
				svc.ramWarningActive = true;
				char msg[160];
				sprintf_s(msg, sizeof(msg), "[JXStartup] Canh bao: RAM dang su dung %.0f MB.", svc.ramBytes / (1024.0 * 1024.0));
				PostLogLine(m_hNotifyWnd, svc.id, msg);
			}
		}
		else
		{
			svc.ramWarningActive = false;
		}
	}
}
