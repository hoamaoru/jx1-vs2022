// -------------------------------------------------------------------------
//	�ļ���		��	UiInit.cpp
//	������		��	����
//	����ʱ��	��	2002-9-10 11:27:04
//	��������	��	��ʼ����
//
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "KFilePath.h"
#include "KMusic.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "UiSelServer.h"
#include "UiInit.h"
#include "UiNotice.h"
#include "UiLoginBg.h"
#include "UiOptions.h"
#include "UiConnectInfo.h"
#include "../UiSoundSetting.h"
#include "../../../Engine/src/KDebug.h"

extern KMusic*		g_pMusic;

#define	SCHEME_INI_INIT 	"UiInit.ini"
#define	LAUNCH_GAME_INI		"\\Ui\\LaunchGame.ini"


KUiInit* KUiInit::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	���ܣ��򿪴��ڣ�����Ψһ��һ�������ʵ��
//--------------------------------------------------------------------------
KUiInit* KUiInit::OpenWindow(bool bStartMusic, bool bJustLaunched)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiInit;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		if (bJustLaunched)
		{
			UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
			KUiLoginBackGround::OpenWindow(m_pSelf->m_szLoginBg);
			m_pSelf->Show();
			if (bStartMusic)
				PlayTitleMusic();
		}
		else
		{
			UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
			KUiLoginBackGround::OpenWindow(m_pSelf->m_szLoginBg);
			m_pSelf->Show();
			if (bStartMusic)
				PlayTitleMusic();
		}
		KUiOptions::LoadSetting(bJustLaunched, true);
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	���ܣ��رմ��ڣ�ͬʱ����ѡ���Ƿ�ɾ������ʵ��
//--------------------------------------------------------------------------
void KUiInit::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_ShowCursor(true);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}	
}

void KUiInit::PlayTitleMusic()
{
	char	szMusic[128] = "";
	KIniFile	Ini;
	if (Ini.Load(LAUNCH_GAME_INI))
	{
		int	nCount = 0;
		Ini.GetInteger("JustLaunched", "TitleMusicCount", 0, &nCount);
		if (nCount > 0)
		{
			char	szKey[16];
			sprintf(szKey, "TitleMusic_%d", rand() % nCount);
			Ini.GetString("JustLaunched", szKey, "", szMusic, sizeof(szMusic));
			if (szMusic[0])
			{
				g_pMusic->Stop();
				g_pMusic->Open((char*)szMusic);
				g_pMusic->Play(true);
			}
		}
	}
}

int limit4GameWindown = 0;
bool isChecklimit4GameWindown = false;
int KUiInit::LimitGameWindown(int limit)
{
	limit4GameWindown = limit;
//	g_DebugLog("Enter Game m_EnterGame bg %d", limit4GameWindown);
	if (limit4GameWindown > 4)
	{
		isChecklimit4GameWindown = true;
	}
	else
	{
		isChecklimit4GameWindown = false;
	}

	return limit;
}

void KUiInit::StopTitleMusic()
{
	if (g_pMusic)
	{
		g_pMusic->Stop();
		g_pMusic->Close();
	}
}

//--------------------------------------------------------------------------
//	���ܣ����봰�ڵĽ��淽��
//--------------------------------------------------------------------------
void KUiInit::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_INIT);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_EnterGameBorder.Init(&Ini, "EnterGameBorder");
		m_EnterGame.	Init(&Ini, "EnterGame");
		m_GameConfigBorder.Init(&Ini, "GameConfigBorder");
		m_GameConfig.	Init(&Ini, "GameConfig");
		m_OpenRepBorder.	Init(&Ini, "OpenRepBorder");
		m_OpenRep.	Init(&Ini, "OpenRep");
		m_DesignerList.	Init(&Ini, "DesignerList");
		m_ExitGameBorder.Init(&Ini, "ExitGameBorder");
		m_ExitGame.		Init(&Ini, "ExitGame");

		Ini.GetString("Main", "LoginBg", "", m_szLoginBg, sizeof(m_szLoginBg));
	}	
}

//--------------------------------------------------------------------------
//	���ܣ���ʼ��
//--------------------------------------------------------------------------
void KUiInit::Initialize()
{
	AddChild(&m_EnterGameBorder);
	AddChild(&m_EnterGame);
	AddChild(&m_GameConfigBorder);
	AddChild(&m_GameConfig);
	AddChild(&m_OpenRepBorder);
	AddChild(&m_OpenRep);
	AddChild(&m_DesignerList);
	AddChild(&m_ExitGameBorder);
	AddChild(&m_ExitGame);
	

	m_szLoginBg[0] = 0;
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_TOPMOST);
}

//--------------------------------------------------------------------------
//	���ܣ�������Ϣ����
//--------------------------------------------------------------------------
int KUiInit::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam);
		break;
	case WM_KEYDOWN:
		nRet = OnKeyDown(uParam);
		break;
	case WM_SYSKEYDOWN:
		if (uParam == 'A')
			OnAutoLogin();
		break;
	case WND_M_OTHER_WORK_RESULT:
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);	
	}
	return nRet;
}

int KUiInit::OnKeyDown(unsigned int uKey)
{
	int	nRet = 1;
	KWndButton* pActive = NULL;
	KWndButton* pToActive = NULL;
	if (uKey == VK_RETURN)
	{
		if (pActive = GetActiveBtn())
			OnClickButton(pActive);
	}
	else if (uKey == VK_UP)
	{
		pActive = GetActiveBtn();
		if (pActive == &m_EnterGame)	
			pToActive = &m_ExitGame;
		else if (pActive == &m_ExitGame)
			pToActive = &m_OpenRep;
		else if (pActive == &m_OpenRep)
			pToActive = &m_DesignerList;
		else if (pActive == &m_DesignerList)
			pToActive = &m_GameConfig;
		else if (pActive == &m_GameConfig)
			pToActive = &m_EnterGame;
		else
			pToActive = &m_ExitGame;
	}
	else if (uKey == VK_DOWN)
	{
		pActive = GetActiveBtn();
		if (pActive == &m_EnterGame)
			pToActive = &m_GameConfig;
		else if (pActive == &m_GameConfig)
			pToActive = &m_DesignerList;
		else if (pActive == &m_DesignerList)
			pToActive = &m_OpenRep;
		else if (pActive == &m_OpenRep)
			pToActive = &m_ExitGame;
		else
			pToActive = &m_EnterGame;
	}
	else
		nRet = 0;
	if (pToActive)
		pToActive->SetCursorAbove();
	return nRet;
}

//--------------------------------------------------------------------------
//	���ܣ���Ӧ�����ť
//--------------------------------------------------------------------------
void KUiInit::OnClickButton(KWndButton* pWnd)
{
	if (pWnd == &m_EnterGame)
	{
	//	g_DebugLog("Enter Game m_EnterGame %d", limit4GameWindown);
		if (isChecklimit4GameWindown)
		{
			KUiConnectInfo::OpenWindow(CI_MI_INVALID_LIMIT_WINDOWN, CI_NS_INIT_WND, 0);
			CloseWindow();
			return;
		}

		if (KUiNotice::OpenWindow())
			CloseWindow();
	}
	else if (pWnd == &m_GameConfig)
	{
		if (KUiOptions::OpenWindow(this))
			Hide();
	}
	else if (pWnd == &m_DesignerList)
	{
//		if (KUiNotice::OpenWindow())
//			CloseWindow();
	}	
	else if (pWnd == &m_OpenRep)
	{
		if (KUiNotice::OpenWindow())
			CloseWindow();
	}
	else if (pWnd == &m_ExitGame)
	{
		//TamLTM m_ExitGame
//		g_DebugLog("Exit Game m_ExitGame");
		CloseWindow();
		UiPostQuitMsg();
	}
}

void KUiInit::OnAutoLogin()
{
	g_LoginLogic.LoadLoginChoice();
	if (g_LoginLogic.IsAutoLoginEnable())
	{
		KIniFile*	pSetting = g_UiBase.GetCommConfigFile();
		int nAutoLogin = false;
		if (pSetting)
		{
			pSetting->GetInteger("Main", "AutoLogin", 0, &nAutoLogin);
			g_UiBase.CloseCommConfigFile();
		}
		if (nAutoLogin == 6323)
		{
			CloseWindow();
			KUiConnectInfo::OpenWindow(CI_MI_CONNECTING, LL_S_IN_GAME);
			g_LoginLogic.AutoLogin();
		}
	}
}

KWndButton*	KUiInit::GetActiveBtn()
{
	KWndButton* pBtn = NULL;
	if (m_EnterGame.IsButtonActive())
		pBtn = &m_EnterGame;
	else if (m_GameConfig.IsButtonActive())
		pBtn = &m_GameConfig;
	else if (m_DesignerList.IsButtonActive())
		pBtn = &m_DesignerList;		
	else if (m_OpenRep.IsButtonActive())
		pBtn = &m_OpenRep;	
	else if (m_ExitGame.IsButtonActive())
		pBtn = &m_ExitGame;		

	return pBtn;
}

void KUiInit::ShowCompleted()
{
    m_EnterGame.SetCursorAbove();
}
