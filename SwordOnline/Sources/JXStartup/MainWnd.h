#pragma once

#include "StdAfx.h"
#include "ServiceManager.h"
#include "resource.h"

class CMainWnd
{
public:
	CMainWnd();
	~CMainWnd();

	bool Create(HINSTANCE hInstance, int nCmdShow);
	HWND GetHwnd() const { return m_hWnd; }

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

	void OnCreate();
	void OnSize(int width, int height);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnTimer();
	LRESULT OnNotify(LPARAM lParam);
	LRESULT HandleListCustomDraw(NMLVCUSTOMDRAW* cd);
	void SyncTabVisibility();
	void OnLogLine(int serviceId, char* text);
	void OnStatusChanged(int serviceId);
	void OnClose();

	void SetupServices();
	void RefreshRow(int serviceId);
	int GetSelectedServiceId() const;
	void AppendLog(int serviceId, const char* text);
	void StartAllSequenced();
	void StopAllReverse();
	void ProcessBatchStep();
	const char* StatusText(EServiceStatus status) const;

	void InitTrayIcon();
	void UpdateTrayIcon();
	void ShowTrayMenu();
	HICON CreateColorIcon(COLORREF color);
	void ExportLogs();

	HWND m_hWnd = nullptr;
	HWND m_hList = nullptr;
	HWND m_hTab = nullptr;
	HWND m_hStatusBar = nullptr;
	HWND m_hEditLog[SERVICE_COUNT] = {};
	HWND m_hBtnStart = nullptr;
	HWND m_hBtnStop = nullptr;
	HWND m_hBtnRestart = nullptr;
	HWND m_hBtnStartAll = nullptr;
	HWND m_hBtnStopAll = nullptr;
	HWND m_hBtnForceStop = nullptr;
	HWND m_hBtnExportLog = nullptr;

	CServiceManager* m_pServiceManager = nullptr;

	enum class EBatchAction { None, Start, Stop };
	EBatchAction m_batchAction = EBatchAction::None;
	std::vector<int> m_batchQueue;
	ULONGLONG m_nextBatchTick = 0;

	std::vector<int> m_pendingRestart;

	HFONT m_hUiFont = nullptr;
	HFONT m_hLogFont = nullptr;
	HMODULE m_hRichEditLib = nullptr;

	NOTIFYICONDATAA m_nid{};
	HICON m_hIconGreen = nullptr;
	HICON m_hIconYellow = nullptr;
	HICON m_hIconRed = nullptr;
	HICON m_hIconGray = nullptr;
	int m_currentTrayIconKind = -1;
};
