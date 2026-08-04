#pragma once

#include "StdAfx.h"

enum class EServiceStatus
{
	Stopped,
	Starting,
	Running,
	Stopping,
	Crashed,
};

// How a service's console/stdio is handled at launch:
//  - Pipe: plain console app that never touches its own console handle (safe to
//    redirect stdout/stderr to a pipe and read it live).
//  - HiddenConsole: console app that calls SetConsoleScreenBufferSize/SetConsoleWindowInfo/
//    SetConsoleTextAttribute on STD_OUTPUT_HANDLE during startup (e.g. Sword3PaySys,
//    Goddess, Bishop, S3Relay all share this pattern). Those APIs fail on a pipe handle,
//    which aborts the server's own startup, so these must get a REAL console (just hidden),
//    not a redirected one. Live log content instead comes from tailing the KGLog file the
//    server writes to <workdir>logs/<logIdent>/ (see SERVICE_ENTRY::logIdent).
//  - Window: Windows-subsystem app with no console at all (e.g. S3RelayServer).
enum class ECaptureMode
{
	Pipe,
	HiddenConsole,
	Window,
};

struct SERVICE_ENTRY
{
	int id = -1;
	std::string name;
	std::string exeFileName;
	ECaptureMode captureMode = ECaptureMode::Pipe;
	std::string logIdent;	// KGLog szIdent, e.g. "paysys" -> logs/paysys/. Empty = no known log file to tail.

	EServiceStatus status = EServiceStatus::Stopped;
	PROCESS_INFORMATION procInfo{};
	HANDLE hStdOutRead = nullptr;
	HANDLE hLogThread = nullptr;
	HANDLE hStopEvent = nullptr;	// signalled (then closed) by CloseServiceHandles() to stop a log-tail thread
	ULONGLONG startTickMs = 0;
	ULONGLONG forceKillDeadlineMs = 0;
	DWORD exitCode = 0;

	// Resource usage, refreshed each Tick() while Running.
	double cpuPercent = 0.0;		// 100 == fully saturating one core
	SIZE_T ramBytes = 0;			// working set
	FILETIME lastKernelTime{};
	FILETIME lastUserTime{};
	ULONGLONG lastSampleTickMs = 0;
	bool cpuWarningActive = false;	// edge-triggered, avoids spamming the log every tick
	bool ramWarningActive = false;
};

// Resource-usage thresholds shared between CServiceManager (log warnings) and
// the UI (cell highlighting) so the two never drift out of sync.
constexpr double kCpuWarnPercent = 90.0;
constexpr SIZE_T kRamWarnBytes = 1024ull * 1024 * 1024; // 1 GB

// Manages the lifetime of the 6 server processes. All mutation of SERVICE_ENTRY
// happens on the UI thread (via StartService/StopService/Tick); the background
// log-reader threads only ever post messages to hNotifyWnd, they never touch
// m_services directly. Do not call AddService after Start/Stop has been used,
// since std::vector reallocation would invalidate pointers handed to threads.
class CServiceManager
{
public:
	explicit CServiceManager(HWND hNotifyWnd);
	~CServiceManager();

	void SetWorkingDirectory(const std::string& dir);
	const std::string& GetWorkingDirectory() const { return m_workDir; }

	void AddService(int id, const char* name, const char* exeFileName, ECaptureMode captureMode, const char* logIdent = nullptr);

	bool StartService(int id);
	bool StopService(int id, bool force);

	// Non-blocking periodic maintenance: detects process exit and enforces the
	// force-kill deadline set by a graceful StopService() call. Call every ~1s.
	void Tick();

	std::vector<SERVICE_ENTRY>& Services() { return m_services; }
	SERVICE_ENTRY* FindById(int id);

private:
	static DWORD WINAPI LogReaderThreadProc(LPVOID param);
	static DWORD WINAPI LogTailThreadProc(LPVOID param);
	void UpdateResourceUsage();

	std::vector<SERVICE_ENTRY> m_services;
	std::string m_workDir;
	HWND m_hNotifyWnd;
};
