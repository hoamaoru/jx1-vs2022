#include "StdAfx.h"
#include "MainWnd.h"
#include "resource.h"
#include "ZipWriter.h"

namespace
{
	const char* kClassName = "JXStartupMainWndClass";

	struct SERVICE_DEF
	{
		int id;
		const char* name;
		const char* exeFileName;
		ECaptureMode captureMode;
		const char* logIdent;	// KGLog szIdent (logs/<logIdent>/...), nullptr if not applicable
	};

	// Thu tu khoi dong mac dinh, dung theo yeu cau: PaySys -> Relay thanh toan ->
	// DB -> Bishop -> S3Relay -> GameServer.
	// Sword3PaySys/Goddess/Bishop/S3Relay tu goi SetConsoleScreenBufferSize/... tren
	// STD_OUTPUT_HANDLE luc khoi dong nen KHONG duoc redirect stdio (se lam server
	// thoat som) -> dung HiddenConsole, doc log qua file KGLog tu ghi (logIdent).
	// GameServer khong dung console API nen dung duoc Pipe (doc log truc tiep).
	// S3RelayServer la Windows subsystem, khong co console.
	const SERVICE_DEF kServiceDefs[SERVICE_COUNT] =
	{
		{ 0, "Sword3PaySys",   "Sword3PaySys.exe",   ECaptureMode::HiddenConsole, "paysys"  },
		{ 1, "S3RelayServer",  "S3RelayServer.exe",  ECaptureMode::Window,        nullptr   },
		{ 2, "Goddess",        "Goddess.exe",         ECaptureMode::HiddenConsole, "goddess" },
		{ 3, "Bishop",         "Bishop.exe",           ECaptureMode::HiddenConsole, "bishop"  },
		{ 4, "S3Relay",        "S3Relay.exe",          ECaptureMode::HiddenConsole, "relay"   },
		{ 5, "GameServer",     "GameServer.exe",       ECaptureMode::Pipe,          nullptr   },
	};

	std::string GetExeDirectory()
	{
		char path[MAX_PATH + 1] = {};
		::GetModuleFileNameA(nullptr, path, MAX_PATH);
		std::string s = path;
		size_t pos = s.find_last_of('\\');
		if (pos != std::string::npos)
			s.erase(pos + 1);
		return s;
	}

	// Dark, terminal-style log view palette.
	const COLORREF kLogBackground   = RGB(18, 18, 18);
	const COLORREF kLogDefaultText  = RGB(210, 210, 210);
	const COLORREF kLogErrorText    = RGB(255, 105, 97);
	const COLORREF kLogWarnText     = RGB(255, 196, 92);
	const COLORREF kLogMetaText     = RGB(120, 190, 255);

	// Best-effort severity detection from KGLog's "[ERROR]"/"[WARN ]" tags and
	// JXStartup's own [JXStartup]-prefixed status lines.
	COLORREF DetectLineColor(const char* text)
	{
		if (strstr(text, "[ERROR]") ||
			strstr(text, "thoat ngoai y muon") ||
			strstr(text, "Khong the") ||
			strstr(text, "Khong tim thay") ||
			strstr(text, "Khong mo duoc") ||
			strstr(text, "Force Stop") ||
			strstr(text, "force kill"))
			return kLogErrorText;

		if (strstr(text, "[WARN"))
			return kLogWarnText;

		if (strstr(text, "[JXStartup]"))
			return kLogMetaText;

		return kLogDefaultText;
	}

	void SetLogSelectionColor(HWND hEdit, COLORREF color)
	{
		CHARFORMAT2A cf;
		ZeroMemory(&cf, sizeof(cf));
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = color;
		::SendMessageA(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	}

	// ListView row status -> text color, used by NM_CUSTOMDRAW.
	COLORREF StatusColor(EServiceStatus status)
	{
		switch (status)
		{
		case EServiceStatus::Running:  return RGB(0, 140, 0);
		case EServiceStatus::Crashed:  return RGB(200, 0, 0);
		case EServiceStatus::Starting:
		case EServiceStatus::Stopping: return RGB(180, 120, 0);
		case EServiceStatus::Stopped:  return RGB(110, 110, 110);
		}
		return RGB(0, 0, 0);
	}
}

CMainWnd::CMainWnd()
{
}

CMainWnd::~CMainWnd()
{
	delete m_pServiceManager;
	if (m_hUiFont) ::DeleteObject(m_hUiFont);
	if (m_hLogFont) ::DeleteObject(m_hLogFont);
	if (m_hRichEditLib) ::FreeLibrary(m_hRichEditLib);
	if (m_hIconGreen) ::DestroyIcon(m_hIconGreen);
	if (m_hIconYellow) ::DestroyIcon(m_hIconYellow);
	if (m_hIconRed) ::DestroyIcon(m_hIconRed);
	if (m_hIconGray) ::DestroyIcon(m_hIconGray);
}

LRESULT CALLBACK CMainWnd::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CMainWnd* pThis = nullptr;

	if (msg == WM_NCCREATE)
	{
		CREATESTRUCTA* cs = (CREATESTRUCTA*)lParam;
		pThis = (CMainWnd*)cs->lpCreateParams;
		::SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
		pThis->m_hWnd = hWnd;
	}
	else
	{
		pThis = (CMainWnd*)::GetWindowLongPtrA(hWnd, GWLP_USERDATA);
	}

	if (pThis)
		return pThis->HandleMessage(msg, wParam, lParam);

	return ::DefWindowProcA(hWnd, msg, wParam, lParam);
}

LRESULT CMainWnd::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		OnCreate();
		return 0;
	case WM_SIZE:
		OnSize(LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return 0;
	case WM_NOTIFY:
		return OnNotify(lParam);
	case WM_TIMER:
		if (wParam == ID_TIMER_TICK)
			OnTimer();
		return 0;
	case WM_APP_LOGLINE:
		OnLogLine((int)wParam, (char*)lParam);
		free((void*)lParam);
		return 0;
	case WM_APP_STATUSCHANGED:
		OnStatusChanged((int)wParam);
		return 0;
	case WM_CLOSE:
		OnClose();
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xFFF0) == SC_MINIMIZE)
		{
			::ShowWindow(m_hWnd, SW_HIDE);
			return 0;
		}
		break;
	case WM_APP_TRAYICON:
		if (lParam == WM_LBUTTONDBLCLK)
		{
			::ShowWindow(m_hWnd, SW_RESTORE);
			::SetForegroundWindow(m_hWnd);
		}
		else if (lParam == WM_RBUTTONUP || lParam == WM_CONTEXTMENU)
		{
			ShowTrayMenu();
		}
		return 0;
	case WM_DESTROY:
		::KillTimer(m_hWnd, ID_TIMER_TICK);
		::Shell_NotifyIconA(NIM_DELETE, &m_nid);
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProcA(m_hWnd, msg, wParam, lParam);
}

bool CMainWnd::Create(HINSTANCE hInstance, int nCmdShow)
{
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES | ICC_BAR_CLASSES | ICC_STANDARD_CLASSES;
	::InitCommonControlsEx(&icc);

	WNDCLASSEXA wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = &CMainWnd::WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = ::LoadCursorA(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wc.lpszClassName = kClassName;
	::RegisterClassExA(&wc);

	HWND hWnd = ::CreateWindowExA(
		0, kClassName, "JXStartup - Quan ly Server",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 960, 660,
		nullptr, nullptr, hInstance, this);

	if (!hWnd)
		return false;

	::ShowWindow(hWnd, nCmdShow);
	::UpdateWindow(hWnd);
	return true;
}

void CMainWnd::OnCreate()
{
	HINSTANCE hInst = ::GetModuleHandleA(nullptr);

	m_hRichEditLib = ::LoadLibraryA("Msftedit.dll");

	HDC hdc = ::GetDC(m_hWnd);
	int uiFontHeight = -::MulDiv(9, ::GetDeviceCaps(hdc, LOGPIXELSY), 72);
	::ReleaseDC(m_hWnd, hdc);

	m_hUiFont = ::CreateFontA(uiFontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

	m_hList = ::CreateWindowExA(WS_EX_CLIENTEDGE, WC_LISTVIEWA, "",
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
		0, 0, 0, 0, m_hWnd, (HMENU)IDC_LISTVIEW_SERVICES, hInst, nullptr);
	ListView_SetExtendedListViewStyle(m_hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	::SendMessageA(m_hList, WM_SETFONT, (WPARAM)m_hUiFont, TRUE);

	LVCOLUMNA col;
	ZeroMemory(&col, sizeof(col));
	col.mask = LVCF_TEXT | LVCF_WIDTH;

	col.pszText = (LPSTR)"Dich vu";    col.cx = 150; ListView_InsertColumn(m_hList, 0, &col);
	col.pszText = (LPSTR)"Trang thai"; col.cx = 120; ListView_InsertColumn(m_hList, 1, &col);
	col.pszText = (LPSTR)"PID";        col.cx = 70;  ListView_InsertColumn(m_hList, 2, &col);
	col.pszText = (LPSTR)"Thoi gian chay"; col.cx = 100; ListView_InsertColumn(m_hList, 3, &col);
	col.pszText = (LPSTR)"CPU %";      col.cx = 70;  ListView_InsertColumn(m_hList, 4, &col);
	col.pszText = (LPSTR)"RAM (MB)";   col.cx = 90;  ListView_InsertColumn(m_hList, 5, &col);
	col.pszText = (LPSTR)"Ghi chu";    col.cx = 220; ListView_InsertColumn(m_hList, 6, &col);

	m_hBtnStart = ::CreateWindowExA(0, "BUTTON", "Start",
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_START, hInst, nullptr);
	m_hBtnStop = ::CreateWindowExA(0, "BUTTON", "Stop",
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_STOP, hInst, nullptr);
	m_hBtnRestart = ::CreateWindowExA(0, "BUTTON", "Restart",
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_RESTART, hInst, nullptr);
	m_hBtnForceStop = ::CreateWindowExA(0, "BUTTON", "Force Stop",
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_FORCESTOP, hInst, nullptr);
	m_hBtnStartAll = ::CreateWindowExA(0, "BUTTON", "Start All",
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_STARTALL, hInst, nullptr);
	m_hBtnStopAll = ::CreateWindowExA(0, "BUTTON", "Stop All",
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_STOPALL, hInst, nullptr);
	m_hBtnExportLog = ::CreateWindowExA(0, "BUTTON", "Export Log",
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_EXPORTLOG, hInst, nullptr);

	HWND hButtons[] = { m_hBtnStart, m_hBtnStop, m_hBtnRestart, m_hBtnForceStop, m_hBtnStartAll, m_hBtnStopAll, m_hBtnExportLog };
	for (HWND hBtn : hButtons)
		::SendMessageA(hBtn, WM_SETFONT, (WPARAM)m_hUiFont, TRUE);

	m_hTab = ::CreateWindowExA(0, WC_TABCONTROLA, "",
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, (HMENU)IDC_TAB_LOGS, hInst, nullptr);
	::SendMessageA(m_hTab, WM_SETFONT, (WPARAM)m_hUiFont, TRUE);

	CHARFORMAT2A logFormat;
	ZeroMemory(&logFormat, sizeof(logFormat));
	logFormat.cbSize = sizeof(logFormat);
	logFormat.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE;
	logFormat.crTextColor = kLogDefaultText;
	logFormat.yHeight = 190; // twips (~9.5pt), independent of screen DPI
	lstrcpyA(logFormat.szFaceName, "Consolas");

	for (int i = 0; i < SERVICE_COUNT; ++i)
	{
		TCITEMA tie;
		ZeroMemory(&tie, sizeof(tie));
		tie.mask = TCIF_TEXT;
		tie.pszText = (LPSTR)kServiceDefs[i].name;
		TabCtrl_InsertItem(m_hTab, i, &tie);

		m_hEditLog[i] = ::CreateWindowExA(WS_EX_CLIENTEDGE, "RICHEDIT50W", "",
			WS_CHILD | (i == 0 ? WS_VISIBLE : 0) | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
			0, 0, 0, 0, m_hTab, (HMENU)(INT_PTR)(IDC_EDIT_LOG_BASE + i), hInst, nullptr);

		::SendMessageA(m_hEditLog[i], EM_SETBKGNDCOLOR, 0, (LPARAM)kLogBackground);
		::SendMessageA(m_hEditLog[i], EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&logFormat);
		::SendMessageA(m_hEditLog[i], EM_EXLIMITTEXT, 0, 400000);
		::SendMessageA(m_hEditLog[i], EM_SETEVENTMASK, 0, 0);
	}

	// Don't rely on the tab control defaulting to item 0 after several inserts.
	TabCtrl_SetCurSel(m_hTab, 0);

	m_hStatusBar = ::CreateWindowExA(0, STATUSCLASSNAMEA, "San sang.",
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hWnd, (HMENU)IDC_STATUSBAR, hInst, nullptr);
	::SendMessageA(m_hStatusBar, WM_SETFONT, (WPARAM)m_hUiFont, TRUE);

	SetupServices();

	for (int i = 0; i < SERVICE_COUNT; ++i)
	{
		LVITEMA item;
		ZeroMemory(&item, sizeof(item));
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.pszText = (LPSTR)kServiceDefs[i].name;
		ListView_InsertItem(m_hList, &item);
		RefreshRow(i);
	}

	ListView_SetItemState(m_hList, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	::SetTimer(m_hWnd, ID_TIMER_TICK, TICK_INTERVAL_MS, nullptr);

	InitTrayIcon();
}

void CMainWnd::SetupServices()
{
	m_pServiceManager = new CServiceManager(m_hWnd);
	m_pServiceManager->SetWorkingDirectory(GetExeDirectory());

	for (int i = 0; i < SERVICE_COUNT; ++i)
	{
		const SERVICE_DEF& def = kServiceDefs[i];
		m_pServiceManager->AddService(def.id, def.name, def.exeFileName, def.captureMode, def.logIdent);
	}
}

void CMainWnd::OnSize(int width, int height)
{
	if (!m_hList)
		return;

	const int margin = 8;
	const int listHeight = 150;
	const int btnHeight = 26;
	const int btnWidth = 88;
	const int btnGap = 6;

	::SendMessageA(m_hStatusBar, WM_SIZE, 0, 0);

	RECT rcStatus;
	::GetWindowRect(m_hStatusBar, &rcStatus);
	int statusHeight = rcStatus.bottom - rcStatus.top;

	::MoveWindow(m_hList, margin, margin, width - 2 * margin, listHeight, TRUE);

	int btnY = margin + listHeight + margin;
	int x = margin;
	::MoveWindow(m_hBtnStart, x, btnY, btnWidth, btnHeight, TRUE); x += btnWidth + btnGap;
	::MoveWindow(m_hBtnStop, x, btnY, btnWidth, btnHeight, TRUE); x += btnWidth + btnGap;
	::MoveWindow(m_hBtnRestart, x, btnY, btnWidth, btnHeight, TRUE); x += btnWidth + btnGap;
	::MoveWindow(m_hBtnForceStop, x, btnY, btnWidth, btnHeight, TRUE); x += btnWidth + btnGap * 3;
	::MoveWindow(m_hBtnStartAll, x, btnY, btnWidth, btnHeight, TRUE); x += btnWidth + btnGap;
	::MoveWindow(m_hBtnStopAll, x, btnY, btnWidth, btnHeight, TRUE); x += btnWidth + btnGap * 3;
	::MoveWindow(m_hBtnExportLog, x, btnY, btnWidth + 20, btnHeight, TRUE);

	int tabY = btnY + btnHeight + margin;
	int tabHeight = height - tabY - statusHeight - margin;
	if (tabHeight < 50) tabHeight = 50;
	::MoveWindow(m_hTab, margin, tabY, width - 2 * margin, tabHeight, TRUE);

	RECT rcTab = { 0, 0, width - 2 * margin, tabHeight };
	TabCtrl_AdjustRect(m_hTab, FALSE, &rcTab);

	for (int i = 0; i < SERVICE_COUNT; ++i)
	{
		::MoveWindow(m_hEditLog[i], rcTab.left, rcTab.top,
			rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, TRUE);
	}
}

int CMainWnd::GetSelectedServiceId() const
{
	int index = ListView_GetNextItem(m_hList, -1, LVNI_SELECTED);
	return index; // row index == service id, see SetupServices/OnCreate
}

const char* CMainWnd::StatusText(EServiceStatus status) const
{
	switch (status)
	{
	case EServiceStatus::Stopped:  return "Da dung";
	case EServiceStatus::Starting: return "Dang khoi dong";
	case EServiceStatus::Running:  return "Dang chay";
	case EServiceStatus::Stopping: return "Dang dung";
	case EServiceStatus::Crashed:  return "Loi / Crash";
	}
	return "";
}

void CMainWnd::RefreshRow(int serviceId)
{
	SERVICE_ENTRY* svc = m_pServiceManager->FindById(serviceId);
	if (!svc)
		return;

	char buf[64];

	ListView_SetItemText(m_hList, serviceId, 1, (LPSTR)StatusText(svc->status));

	if (svc->procInfo.dwProcessId != 0 &&
		(svc->status == EServiceStatus::Running || svc->status == EServiceStatus::Starting || svc->status == EServiceStatus::Stopping))
		wsprintfA(buf, "%lu", svc->procInfo.dwProcessId);
	else
		lstrcpyA(buf, "--");
	ListView_SetItemText(m_hList, serviceId, 2, (LPSTR)buf);

	if (svc->startTickMs != 0 &&
		(svc->status == EServiceStatus::Running || svc->status == EServiceStatus::Stopping))
	{
		ULONGLONG elapsedSec = (::GetTickCount64() - svc->startTickMs) / 1000;
		DWORD hh = (DWORD)(elapsedSec / 3600);
		DWORD mm = (DWORD)((elapsedSec % 3600) / 60);
		DWORD ss = (DWORD)(elapsedSec % 60);
		wsprintfA(buf, "%02lu:%02lu:%02lu", hh, mm, ss);
	}
	else
	{
		lstrcpyA(buf, "--:--:--");
	}
	ListView_SetItemText(m_hList, serviceId, 3, (LPSTR)buf);

	if (svc->status == EServiceStatus::Running)
	{
		// wsprintfA doesn't support %f, use the CRT's sprintf_s for these.
		sprintf_s(buf, sizeof(buf), "%.0f", svc->cpuPercent);
		ListView_SetItemText(m_hList, serviceId, 4, (LPSTR)buf);

		sprintf_s(buf, sizeof(buf), "%.0f", svc->ramBytes / (1024.0 * 1024.0));
		ListView_SetItemText(m_hList, serviceId, 5, (LPSTR)buf);
	}
	else
	{
		ListView_SetItemText(m_hList, serviceId, 4, (LPSTR)"--");
		ListView_SetItemText(m_hList, serviceId, 5, (LPSTR)"--");
	}

	const char* note = "";
	if (svc->status == EServiceStatus::Crashed)
		note = "Kiem tra tab log de biet chi tiet";
	else if (svc->captureMode == ECaptureMode::Window)
		note = "Khong doc duoc log truc tiep (Windows subsystem)";
	else if (svc->captureMode == ECaptureMode::HiddenConsole && svc->logIdent.empty())
		note = "Console rieng (an) - xem file log trong thu muc logs/";
	ListView_SetItemText(m_hList, serviceId, 6, (LPSTR)note);
}

void CMainWnd::AppendLog(int serviceId, const char* text)
{
	if (serviceId < 0 || serviceId >= SERVICE_COUNT || !m_hEditLog[serviceId])
		return;

	HWND hEdit = m_hEditLog[serviceId];

	// Cap buffer size so a long-running service doesn't grow the edit control forever.
	int len = ::GetWindowTextLengthA(hEdit);
	if (len > 200000)
	{
		::SendMessageA(hEdit, EM_SETSEL, 0, len / 2);
		::SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)"");
		len = ::GetWindowTextLengthA(hEdit);
	}

	::SendMessageA(hEdit, EM_SETSEL, len, len);
	SetLogSelectionColor(hEdit, DetectLineColor(text));
	::SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)text);
	::SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
}

void CMainWnd::OnLogLine(int serviceId, char* text)
{
	AppendLog(serviceId, text);
}

void CMainWnd::OnStatusChanged(int serviceId)
{
	RefreshRow(serviceId);

	SERVICE_ENTRY* svc = m_pServiceManager->FindById(serviceId);
	if (svc && svc->status == EServiceStatus::Stopped)
	{
		auto it = std::find(m_pendingRestart.begin(), m_pendingRestart.end(), serviceId);
		if (it != m_pendingRestart.end())
		{
			m_pendingRestart.erase(it);
			m_pServiceManager->StartService(serviceId);
		}
	}
}

void CMainWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int ctrlId = LOWORD(wParam);
	int selId = GetSelectedServiceId();

	switch (ctrlId)
	{
	case IDC_BTN_START:
		if (selId >= 0)
			m_pServiceManager->StartService(selId);
		break;

	case IDC_BTN_STOP:
		if (selId >= 0)
			m_pServiceManager->StopService(selId, false);
		break;

	case IDC_BTN_FORCESTOP:
		if (selId >= 0)
			m_pServiceManager->StopService(selId, true);
		break;

	case IDC_BTN_RESTART:
		if (selId >= 0)
		{
			SERVICE_ENTRY* svc = m_pServiceManager->FindById(selId);
			if (svc && svc->status == EServiceStatus::Stopped)
			{
				m_pServiceManager->StartService(selId);
			}
			else if (svc)
			{
				if (std::find(m_pendingRestart.begin(), m_pendingRestart.end(), selId) == m_pendingRestart.end())
					m_pendingRestart.push_back(selId);
				m_pServiceManager->StopService(selId, false);
			}
		}
		break;

	case IDC_BTN_STARTALL:
		StartAllSequenced();
		break;

	case IDC_BTN_STOPALL:
		StopAllReverse();
		break;

	case IDC_BTN_EXPORTLOG:
		ExportLogs();
		break;

	case IDM_TRAY_OPEN:
		::ShowWindow(m_hWnd, SW_RESTORE);
		::SetForegroundWindow(m_hWnd);
		break;

	case IDM_TRAY_STARTALL:
		StartAllSequenced();
		break;

	case IDM_TRAY_STOPALL:
		StopAllReverse();
		break;

	case IDM_TRAY_EXIT:
		::PostMessageA(m_hWnd, WM_CLOSE, 0, 0);
		break;
	}
}

void CMainWnd::SyncTabVisibility()
{
	int sel = TabCtrl_GetCurSel(m_hTab);
	for (int i = 0; i < SERVICE_COUNT; ++i)
		::ShowWindow(m_hEditLog[i], i == sel ? SW_SHOW : SW_HIDE);
}

LRESULT CMainWnd::OnNotify(LPARAM lParam)
{
	NMHDR* hdr = (NMHDR*)lParam;

	if (hdr->hwndFrom == m_hList && hdr->code == NM_CUSTOMDRAW)
		return HandleListCustomDraw((NMLVCUSTOMDRAW*)lParam);

	if (hdr->hwndFrom == m_hTab && hdr->code == TCN_SELCHANGE)
		SyncTabVisibility();

	return 0;
}

LRESULT CMainWnd::HandleListCustomDraw(NMLVCUSTOMDRAW* cd)
{
	switch (cd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;

	case CDDS_ITEMPREPAINT:
		return CDRF_NOTIFYSUBITEMDRAW;

	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		if (m_pServiceManager)
		{
			int id = (int)cd->nmcd.dwItemSpec;
			SERVICE_ENTRY* svc = m_pServiceManager->FindById(id);
			if (svc)
			{
				if (cd->iSubItem == 1) // cot "Trang thai"
					cd->clrText = StatusColor(svc->status);
				else if (cd->iSubItem == 4 && svc->cpuPercent >= kCpuWarnPercent) // CPU %
					cd->clrText = RGB(200, 0, 0);
				else if (cd->iSubItem == 5 && svc->ramBytes >= kRamWarnBytes) // RAM
					cd->clrText = RGB(200, 0, 0);
			}
		}
		return CDRF_NEWFONT;
	}
	return CDRF_DODEFAULT;
}

void CMainWnd::StartAllSequenced()
{
	m_batchAction = EBatchAction::Start;
	m_batchQueue.clear();
	for (int i = 0; i < SERVICE_COUNT; ++i)
		m_batchQueue.push_back(i);
	m_nextBatchTick = ::GetTickCount64();
	ProcessBatchStep();
}

void CMainWnd::StopAllReverse()
{
	m_batchAction = EBatchAction::Stop;
	m_batchQueue.clear();
	for (int i = SERVICE_COUNT - 1; i >= 0; --i)
		m_batchQueue.push_back(i);
	m_nextBatchTick = ::GetTickCount64();
	ProcessBatchStep();
}

void CMainWnd::ProcessBatchStep()
{
	if (m_batchAction == EBatchAction::None || m_batchQueue.empty())
	{
		m_batchAction = EBatchAction::None;
		return;
	}

	if (::GetTickCount64() < m_nextBatchTick)
		return;

	int id = m_batchQueue.front();
	m_batchQueue.erase(m_batchQueue.begin());

	if (m_batchAction == EBatchAction::Start)
		m_pServiceManager->StartService(id);
	else
		m_pServiceManager->StopService(id, false);

	m_nextBatchTick = ::GetTickCount64() + 1500;

	if (m_batchQueue.empty())
		m_batchAction = EBatchAction::None;
}

void CMainWnd::OnTimer()
{
	m_pServiceManager->Tick();

	for (int i = 0; i < SERVICE_COUNT; ++i)
		RefreshRow(i);

	// Safety net: keeps the visible log tab in sync with the tab control's actual
	// selection even if a TCN_SELCHANGE notification is ever missed.
	SyncTabVisibility();

	UpdateTrayIcon();

	if (m_batchAction != EBatchAction::None)
		ProcessBatchStep();
}

void CMainWnd::OnClose()
{
	bool anyRunning = false;
	for (auto& svc : m_pServiceManager->Services())
	{
		if (svc.status == EServiceStatus::Running || svc.status == EServiceStatus::Starting)
		{
			anyRunning = true;
			break;
		}
	}

	if (anyRunning)
	{
		int r = ::MessageBoxA(m_hWnd,
			"Dang co dich vu chay. Ban co muon dung tat ca truoc khi thoat khong?\n"
			"Yes = Dung tat ca roi thoat | No = Thoat, giu server dang chay | Cancel = Khong thoat",
			"JXStartup", MB_YESNOCANCEL | MB_ICONQUESTION);

		if (r == IDCANCEL)
			return;
		if (r == IDYES)
			StopAllReverse();
	}

	::DestroyWindow(m_hWnd);
}

HICON CMainWnd::CreateColorIcon(COLORREF color)
{
	const int size = 16;
	HDC hdcScreen = ::GetDC(nullptr);
	HDC hdcColor = ::CreateCompatibleDC(hdcScreen);
	HDC hdcMask = ::CreateCompatibleDC(hdcScreen);

	HBITMAP hbmColor = ::CreateCompatibleBitmap(hdcScreen, size, size);
	HBITMAP hbmMask = ::CreateBitmap(size, size, 1, 1, nullptr);

	HBITMAP hbmOldColor = (HBITMAP)::SelectObject(hdcColor, hbmColor);
	HBITMAP hbmOldMask = (HBITMAP)::SelectObject(hdcMask, hbmMask);

	RECT rc = { 0, 0, size, size };

	// Mask: white = transparent everywhere, except a black filled circle
	// (opaque) where the colored dot goes.
	::FillRect(hdcMask, &rc, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	HBRUSH hOldMaskBrush = (HBRUSH)::SelectObject(hdcMask, ::GetStockObject(BLACK_BRUSH));
	HPEN hOldMaskPen = (HPEN)::SelectObject(hdcMask, ::GetStockObject(NULL_PEN));
	::Ellipse(hdcMask, 1, 1, size - 1, size - 1);
	::SelectObject(hdcMask, hOldMaskBrush);
	::SelectObject(hdcMask, hOldMaskPen);

	// Color bitmap: only the pixels inside the (opaque) mask circle matter.
	::FillRect(hdcColor, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	HBRUSH hDotBrush = ::CreateSolidBrush(color);
	HBRUSH hOldColorBrush = (HBRUSH)::SelectObject(hdcColor, hDotBrush);
	HPEN hOldColorPen = (HPEN)::SelectObject(hdcColor, ::GetStockObject(NULL_PEN));
	::Ellipse(hdcColor, 1, 1, size - 1, size - 1);
	::SelectObject(hdcColor, hOldColorBrush);
	::SelectObject(hdcColor, hOldColorPen);
	::DeleteObject(hDotBrush);

	::SelectObject(hdcColor, hbmOldColor);
	::SelectObject(hdcMask, hbmOldMask);
	::DeleteDC(hdcColor);
	::DeleteDC(hdcMask);
	::ReleaseDC(nullptr, hdcScreen);

	ICONINFO ii;
	ZeroMemory(&ii, sizeof(ii));
	ii.fIcon = TRUE;
	ii.hbmColor = hbmColor;
	ii.hbmMask = hbmMask;

	HICON hIcon = ::CreateIconIndirect(&ii);

	::DeleteObject(hbmColor);
	::DeleteObject(hbmMask);

	return hIcon;
}

void CMainWnd::InitTrayIcon()
{
	m_hIconGreen = CreateColorIcon(RGB(40, 180, 40));
	m_hIconYellow = CreateColorIcon(RGB(230, 180, 30));
	m_hIconRed = CreateColorIcon(RGB(200, 40, 40));
	m_hIconGray = CreateColorIcon(RGB(140, 140, 140));

	ZeroMemory(&m_nid, sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.hWnd = m_hWnd;
	m_nid.uID = 1;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = WM_APP_TRAYICON;
	m_nid.hIcon = m_hIconGray;
	lstrcpyA(m_nid.szTip, "JXStartup - Quan ly Server");

	::Shell_NotifyIconA(NIM_ADD, &m_nid);
	m_currentTrayIconKind = 3;
}

void CMainWnd::UpdateTrayIcon()
{
	if (!m_pServiceManager)
		return;

	bool anyCrashed = false, anyRunning = false, anyTransition = false;
	for (auto& svc : m_pServiceManager->Services())
	{
		if (svc.status == EServiceStatus::Crashed) anyCrashed = true;
		else if (svc.status == EServiceStatus::Running) anyRunning = true;
		else if (svc.status == EServiceStatus::Starting || svc.status == EServiceStatus::Stopping) anyTransition = true;
	}

	int kind;
	HICON hIcon;
	const char* tip;
	if (anyCrashed)          { kind = 0; hIcon = m_hIconRed;    tip = "JXStartup - Co dich vu bi loi!"; }
	else if (anyTransition)  { kind = 1; hIcon = m_hIconYellow; tip = "JXStartup - Dang xu ly..."; }
	else if (anyRunning)     { kind = 2; hIcon = m_hIconGreen;  tip = "JXStartup - Dang chay"; }
	else                     { kind = 3; hIcon = m_hIconGray;   tip = "JXStartup - Da dung"; }

	if (kind != m_currentTrayIconKind)
	{
		m_nid.hIcon = hIcon;
		lstrcpynA(m_nid.szTip, tip, sizeof(m_nid.szTip));
		::Shell_NotifyIconA(NIM_MODIFY, &m_nid);
		m_currentTrayIconKind = kind;
	}
}

void CMainWnd::ShowTrayMenu()
{
	HMENU hMenu = ::CreatePopupMenu();
	::AppendMenuA(hMenu, MF_STRING, IDM_TRAY_OPEN, "Mo JXStartup");
	::AppendMenuA(hMenu, MF_SEPARATOR, 0, nullptr);
	::AppendMenuA(hMenu, MF_STRING, IDM_TRAY_STARTALL, "Start All");
	::AppendMenuA(hMenu, MF_STRING, IDM_TRAY_STOPALL, "Stop All");
	::AppendMenuA(hMenu, MF_SEPARATOR, 0, nullptr);
	::AppendMenuA(hMenu, MF_STRING, IDM_TRAY_EXIT, "Thoat");

	POINT pt;
	::GetCursorPos(&pt);

	// Required so the popup menu dismisses correctly when the user clicks away.
	::SetForegroundWindow(m_hWnd);
	::TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hWnd, nullptr);
	::PostMessageA(m_hWnd, WM_NULL, 0, 0);

	::DestroyMenu(hMenu);
}

void CMainWnd::ExportLogs()
{
	SYSTEMTIME st;
	::GetLocalTime(&st);
	char stamp[32];
	wsprintfA(stamp, "%04d%02d%02d_%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	std::string zipPath = m_pServiceManager->GetWorkingDirectory() + "JXStartup_Logs_" + stamp + ".zip";

	CZipWriter zip;
	std::string summary = "JXStartup - Xuat log luc " + std::string(stamp) + "\r\n\r\n";

	for (int i = 0; i < SERVICE_COUNT; ++i)
	{
		SERVICE_ENTRY* svc = m_pServiceManager->FindById(i);
		if (!svc)
			continue;

		// wsprintfA does not support %f (or reliably support width modifiers on
		// mixed arg lists), so use the CRT's sprintf_s for this formatted line.
		char line[256];
		sprintf_s(line, sizeof(line), "%-16s trang thai=%-16s pid=%-8lu cpu=%.0f%%  ram=%.0fMB\r\n",
			svc->name.c_str(), StatusText(svc->status), svc->procInfo.dwProcessId,
			svc->cpuPercent, svc->ramBytes / (1024.0 * 1024.0));
		summary += line;

		HWND hEdit = m_hEditLog[i];
		int len = ::GetWindowTextLengthA(hEdit);
		std::string content;
		if (len > 0)
		{
			content.resize((size_t)len + 1);
			int actual = ::GetWindowTextA(hEdit, &content[0], len + 1);
			content.resize(actual > 0 ? (size_t)actual : 0);
		}
		zip.AddEntry(svc->name + ".log.txt", content);
	}

	zip.AddEntry("summary.txt", summary);

	if (zip.Save(zipPath))
	{
		std::string msg = "Da xuat log ra:\n" + zipPath;
		::MessageBoxA(m_hWnd, msg.c_str(), "Export Log", MB_OK | MB_ICONINFORMATION);

		std::string cmd = "/select,\"" + zipPath + "\"";
		::ShellExecuteA(m_hWnd, "open", "explorer.exe", cmd.c_str(), nullptr, SW_SHOWNORMAL);
	}
	else
	{
		::MessageBoxA(m_hWnd, "Khong the tao file zip.", "Export Log", MB_OK | MB_ICONERROR);
	}
}
