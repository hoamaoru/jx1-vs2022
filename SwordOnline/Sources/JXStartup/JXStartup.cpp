// JXStartup.cpp : entry point cho ung dung quan ly khoi dong server JX
#include "StdAfx.h"
#include "MainWnd.h"

namespace
{
	// Must match kClassName in MainWnd.cpp.
	const char* kMainWndClassName = "JXStartupMainWndClass";
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HANDLE hSingleInstanceMutex = ::CreateMutexA(nullptr, TRUE, "JXStartup_SingleInstance_Mutex_8F2A1C");
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND hExisting = ::FindWindowA(kMainWndClassName, nullptr);
		if (hExisting)
		{
			::ShowWindow(hExisting, SW_RESTORE);
			::SetForegroundWindow(hExisting);
		}
		else
		{
			::MessageBoxA(nullptr, "JXStartup dang chay roi (co the dang thu nho o khay he thong).",
				"JXStartup", MB_OK | MB_ICONINFORMATION);
		}

		if (hSingleInstanceMutex)
			::CloseHandle(hSingleInstanceMutex);
		return 0;
	}

	CMainWnd mainWnd;
	if (!mainWnd.Create(hInstance, nCmdShow))
	{
		if (hSingleInstanceMutex)
			::CloseHandle(hSingleInstanceMutex);
		return 0;
	}

	MSG msg;
	while (::GetMessageA(&msg, nullptr, 0, 0) > 0)
	{
		::TranslateMessage(&msg);
		::DispatchMessageA(&msg);
	}

	if (hSingleInstanceMutex)
		::CloseHandle(hSingleInstanceMutex);

	return (int)msg.wParam;
}
