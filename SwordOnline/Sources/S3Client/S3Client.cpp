// S3Client.cpp : Defines the entry point for the application.
//

#include "KWin32.h"
#include "KCore.h"
#include "S3Client.h"
// Removed header to avoid dependency on missing file: #include "S3ClientDLL.h"
#include "KWin32Wnd.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "Ui/UiShell.h"
#include "NetConnect/NetConnectAgent.h"
#include "TextCtrlCmd/TextCtrlCmd.h"
#include "KPakList.h"
#include "Ui/Elem/TextPic.h"
#include "Ui/Elem/UiCursor.h"
#include "Ui/Elem/SpecialFuncs.h"
#include "Ui/FilterTextLib.h"
#include "Ui/ChatFilter.h"
#include "Ui/uibase.h"
#include "ErrorCode.h"
#include "Ui/UiCase/UiRankLienTram.h"

// Export macro for optional DLL build
#ifdef S3CLIENT_BUILD_DLL
  #define S3CLIENT_API extern "C" __declspec(dllexport)
#else
  #define S3CLIENT_API extern "C"
#endif

#define ClientVersion
KMyApp		MyApp;
HINSTANCE	hInst;
KPakList	g_PakList;
CFilterTextLib g_libFilterText;
CChatFilter g_ChatFilter;


#define	QUIT_QUESTION_ID	"22"
#define	GAME_TITLE			"23"

#define REPRESENT_MODULE_2			"Represent2.dll"
#define REPRESENT_MODULE_3			"Represent3.dll"
#define CREATE_REPRESENT_SHELL_FUN	"CreateRepresentShell"
#define	GAME_FPS			18

struct iRepresentShell*	g_pRepresentShell = NULL;
struct IInlinePicEngineSink* g_pIInlinePicSink = NULL;
iCoreShell*				g_pCoreShell = NULL;
KMusic*					g_pMusic = NULL;

#define	DYNAMIC_LINK_REPRESENT_LIBRARY

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	static HMODULE		l_hRepresentModule = NULL;
	int					g_bRepresent3 = false;
#endif
	
int					g_bScreen = true;
char				g_szGameName[128] = "11/11/2019";

KClientCallback g_ClientCallback;

#define	SCREEN_WIDTH	800//
#define SCREEN_HEIGHT	600//

/*
 * Add this macro by liupeng on 2003.3.20
 * This macro is helper that can judge some legal character
 */
#define _private_IS_SPACE(c)   ((c) == ' ' || (c) == '\r' || (c) == '\n' || (c) == '\t' || (c) == 'x')
#define IS_SPACE(c)	_private_IS_SPACE(c)

// Pending request value for LienTram UI; LONG_MIN means no request
static volatile LONG g_PendingLienTram = LONG_MIN;

// Helper function to apply a pending LienTram request on the main thread
static void ApplyPendingLienTram()
{
	LONG value = InterlockedExchange(const_cast<LONG*>(&g_PendingLienTram), LONG_MIN);
	if (value != LONG_MIN)
	{
		if (value > 0)
		{
			KUiRankLienTram::OpenWindow((int)value);
		}
		else
		{
			KUiRankLienTram::CloseWindow(false);
		}
	}
}

// Helper function to show UiRankLienTram sprite
S3CLIENT_API void ShowLienTramSprite(int nLienTramValue)
{
	// Make it safe to call from any thread and at any time
	InterlockedExchange(const_cast<LONG*>(&g_PendingLienTram), (LONG)nLienTramValue);
}

#define	SCREEN_WIDTH	800//
#define SCREEN_HEIGHT	600//
//#define	SCREEN_WIDTH	1024//
//#define SCREEN_HEIGHT	768//

/*
 * Add this macro by liupeng on 2003.3.20
 * This macro is helper that can judge some legal character
 */
#define _private_IS_SPACE(c)   ((c) == ' ' || (c) == '\r' || (c) == '\n' || (c) == '\t' || (c) == 'x')
#define IS_SPACE(c)	_private_IS_SPACE(c)

// Test function to cycle through different LienTram values including 3-digit numbers
void TestLienTramDisplay()
{
	static int testCounter = 0;
	static int testValue = 1;
	
	testCounter++;
	
	// Change LienTram value every 3 seconds (assuming 18 FPS)
	if (testCounter >= (18 * 3)) 
	{
		testCounter = 0;
		ShowLienTramSprite(testValue);
		
		testValue++;
		if (testValue > 15) testValue = 1; // Cycle from 1 to 15 (including multi-digit)
	}
}

#ifndef S3CLIENT_BUILD_DLL
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	/*
	 * Add this funtion by liupeng on 2003.3.20
	 * We can find some error when start a console tracer
	 */
#ifdef	TRUE

	bool bOpenTracer = false;

    while( lpCmdLine[0] == '-' || lpCmdLine[0] == '/' )
    {
        lpCmdLine++;
		
        switch ( *lpCmdLine++ )
        {
		case 'c':
        case 'C':
            bOpenTracer = true;
            break;
        }
		
        while( IS_SPACE( *lpCmdLine ) )
        {
            lpCmdLine++;
        }
    }
	
	if ( bOpenTracer ) 
	{
		AllocConsole();
	}

#endif // End of this function

	hInst = hInstance;
	if (MyApp.Init(hInstance))
		MyApp.Run();
	
#ifdef TRUE

	if ( bOpenTracer )
	{
		FreeConsole();
	}

#endif
	Error_Box();

	return 0;
}
#else
// DLL build: provide exported API wrappers and DllMain

S3CLIENT_API BOOL S3Client_Initialize(HINSTANCE hInstance)
{
	hInst = hInstance;
	return MyApp.Init(hInstance);
}

S3CLIENT_API int S3Client_Run()
{
    MyApp.Run();
    return 1;
}

S3CLIENT_API void S3Client_Shutdown()
{
	// Trigger GameExit via KWin32App teardown if needed
	// No explicit function is available here, but if the main loop is not running,
	// the application state will be cleaned up by the host process unloading the DLL.
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// No-op; host should call S3Client_Initialize
		break;
	case DLL_PROCESS_DETACH:
		// Ensure resources are closed from GameExit path if still allocated
		break;
	}
	return TRUE;
}
#endif

KMyApp::KMyApp()
{
	m_pInlinePicSink = NULL;
}

BOOL InitRepresentShell(BOOL bFullScreen, int nWidth, int nHeight)
{
	Error_SetErrorString(g_bRepresent3 ? REPRESENT_MODULE_3 : REPRESENT_MODULE_2);
	if (g_pRepresentShell == NULL)
	{
#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
		if (l_hRepresentModule == NULL &&
			(l_hRepresentModule = LoadLibrary(g_bRepresent3 ? REPRESENT_MODULE_3 : REPRESENT_MODULE_2)) == NULL)
		{
			Error_SetErrorCode(ERR_T_LOAD_MODULE_FAILED);
			return FALSE;
		}
		fnCreateRepresentShell pCreate = (fnCreateRepresentShell)GetProcAddress(
			l_hRepresentModule, CREATE_REPRESENT_SHELL_FUN);
		if (pCreate == NULL || 
			(g_pRepresentShell = pCreate()) == NULL)
		{
			Error_SetErrorCode((pCreate == NULL) ? ERR_T_MODULE_UNCORRECT : ERR_T_MODULE_INIT_FAILED);
			return FALSE;
		}
#else
		g_pRepresentShell = CreateRepresentShell();
#endif
	}
	if(g_pRepresentShell->Create(nWidth, nHeight, bFullScreen != 0))
	{
		return TRUE;
	}
	else
	{
		Error_SetErrorCode(g_bRepresent3 ? ERR_T_REPRESENT3_INIT_FAILED : ERR_T_REPRESENT2_INIT_FAILED);
		return FALSE;
	}
}

BOOL KMyApp::GameInit()
{
	Error_SetErrorString("KMyApp::GameInit");
#ifdef KUI_USE_HARDWARE_MOUSE
	
    ShowMouse(TRUE);
    
    #else   // KUI_USE_HARDWARE_MOUSE
	
    ShowMouse(FALSE);
    
    #endif

	g_SetRootPath(NULL);
	g_SetFilePath("\\");

	KIniFile*	pSetting = g_UiBase.GetCommConfigFile();
	if (pSetting)
	{
		pSetting->GetString("Main", "GameName", "11/11/2019", g_szGameName, sizeof(g_szGameName));
        SetWindowText(g_GetMainHWnd(), g_szGameName);
	}
	g_FindDebugWindow("#32770","DebugWin");
/*#ifdef _DEBUG
	g_FindDebugWindow("#32770","DebugWin");
#endif*/

	KIniFile	IniFile;
	if (!IniFile.Load("\\config.ini"))
	{
		Error_SetErrorCode(ERR_T_FILE_NO_FOUND);
		Error_SetErrorString("\\config.ini");
		return FALSE;
	}
	
#ifdef _DEBUG
	BOOL		bCursor = FALSE;
	if (IniFile.GetInteger("Client", "ShowCursor", 0, &bCursor))
		ShowMouse(TRUE);
#endif

	IniFile.GetInteger("Client", "FullScreen", FALSE, &g_bScreen);

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	IniFile.GetInteger("Client", "Represent", 2, &g_bRepresent3);
	g_bRepresent3 = (g_bRepresent3 == 3);
#endif

	g_PakList.Open("\\package.ini");

	char	szPath[MAX_PATH];
	if (IniFile.GetString("Client", "CapPath", "", szPath, sizeof(szPath)))
	{
		if (szPath[0])
			SetScrPicPath(szPath);
	}

	IniFile.Clear();

	if (!g_libFilterText.Initialize()
		|| !g_ChatFilter.Initialize())
		return FALSE;

	if (!InitRepresentShell(g_bScreen, SCREEN_WIDTH, SCREEN_HEIGHT))
	{
		return FALSE;
	}

	if (!UiInit())
	{
		Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
		Error_SetErrorString("UiInit");
		return FALSE;
	}

	//[wxb 2003-6-23]
	m_pInlinePicSink = new KInlinePicSink;
    if (m_pInlinePicSink)
	{
		m_pInlinePicSink->Init(g_pRepresentShell);
		_ASSERT(NULL == g_pIInlinePicSink);
		g_pIInlinePicSink = m_pInlinePicSink;
	}

	UiSetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	UiPaint(0);

	// init dsound
	m_Sound.Init();

	SetMultiGame(TRUE);

	if ((g_pCoreShell = CoreGetShell()) == NULL)
	{
		Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
		Error_SetErrorString("CoreGetShell");
		return false;
	}
	g_pCoreShell->SetRepresentShell(g_pRepresentShell);
	g_pCoreShell->SetMusicInterface((KMusic*)&m_Music);
	g_pCoreShell->SetCallDataChangedNofify(&g_ClientCallback);
	g_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	g_pMusic = &m_Music;

	if (g_NetConnectAgent.Initialize() == 0)
	{
		Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
		Error_SetErrorString("NetConnectAgent");
		return FALSE;
	}
	
	m_GameCounter = 0;
	m_Timer.Start();
	
	SetMouseHoverTime(400);

	if(UiStart())
	{
		return TRUE;
	}
	else
	{
		Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
		Error_SetErrorString("UiStart");
		return FALSE;
	}
}

BOOL KMyApp::GameExit()
{
	if (m_pInlinePicSink)
	{
		//[wxb 2003-6-23]
		m_pInlinePicSink->UnInit();
		delete m_pInlinePicSink;
		m_pInlinePicSink = NULL;
		g_pIInlinePicSink = NULL;
	}

	UiExit();

	g_pMusic = NULL;
	if (g_pCoreShell)
	{
		g_pCoreShell->SetRepresentShell(NULL);
		g_pCoreShell->SetClient(NULL);
		g_pCoreShell->SetMusicInterface(NULL);
		g_pCoreShell->Release();
		g_pCoreShell = NULL;
	}

	if (g_pRepresentShell)
	{
		g_pRepresentShell->Release();
		g_pRepresentShell = NULL;
	}

	g_NetConnectAgent.Exit();

	m_Music.Close();
	m_Sound.Exit();

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	if (l_hRepresentModule)
	{
		FreeLibrary(l_hRepresentModule);
		l_hRepresentModule = NULL;
	}
#endif

	::ShowCursor(TRUE);

	g_ChatFilter.Uninitialize();
	g_libFilterText.Uninitialize();
	return TRUE;
}

BOOL KMyApp::GameLoop()
{
	static int nGameFps = 0;
	g_NetConnectAgent.Breathe();
	if (m_GameCounter * 1000 <= m_Timer.GetElapse() * GAME_FPS)
	{
		if (g_pCoreShell->Breathe() && UiHeartBeat())
		{
			// Note: UiRankLienTram manages its own timing internally
			// No need to call Breathe() manually as it's handled by the UI system

			// Flush any cross-thread ShowLienTramSprite requests
			ApplyPendingLienTram();
			
			m_GameCounter++;
			int	nElapse = m_Timer.GetElapse();
			if (nElapse)
				nGameFps = m_GameCounter * 1000 / nElapse;
		}
		else
		{
			return false;
		}
	}
	if (m_GameCounter * 1000 >= m_Timer.GetElapse() * GAME_FPS)
	{
		UiPaint(nGameFps);
		Sleep(1);
	}
	else if ((m_GameCounter % 8) == 0)
	{
		Sleep(1);
	}

	return true;
}

int KMyApp::HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nRet = 0;
	if (uMsg != WM_CLOSE)
	{
		// Add keyboard handling for testing UiRankLienTram
		if (uMsg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				// Show LienTram sprite for numbers 1-9
				ShowLienTramSprite(wParam - '0');
				break;
			case '0':
				// Hide LienTram sprite
				ShowLienTramSprite(0);
				break;
			case VK_F1:
				// Test 2-digit display (12)
				ShowLienTramSprite(12);
				break;
			case VK_F2:
				// Test 2-digit max (99)
				ShowLienTramSprite(99);
				break;
			case VK_F3:
				// Test 3-digit display (123)
				ShowLienTramSprite(123);
				break;
			case VK_F4:
				// Test 3-digit mid (456)
				ShowLienTramSprite(456);
				break;
			case VK_F5:
				// Test 3-digit max (999)
				ShowLienTramSprite(999);
				break;
			case VK_F6:
				// Test overflow (1000 -> should cap at 999)
				ShowLienTramSprite(1000);
				break;
			}
		}
		
		UiProcessInput(uMsg, wParam, lParam);
	}
	else if (g_bScreen == false && UiIsAlreadyQuit() == false)
	{
		KIniFile*	pSetting = g_UiBase.GetCommConfigFile();
		if (pSetting)
		{
			char	szMsg[128], szTitle[64];
			pSetting->GetString("InfoString", QUIT_QUESTION_ID, "", szMsg, sizeof(szMsg));
			pSetting->GetString("InfoString", GAME_TITLE, "", szTitle, sizeof(szTitle));
			if (szMsg[0] && szTitle[0])
			{
				nRet = (MessageBox(g_GetMainHWnd(), szMsg, szTitle,
					MB_YESNO | MB_ICONQUESTION) != IDYES);
			}
		}
	}
	return nRet;
}