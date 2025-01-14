// -------------------------------------------------------------------------
//	�ļ���		��	UiInit.h
//	������		��	����
//	����ʱ��	��	2002-9-10 11:25:36
//	��������	��	��ʼ����	
//
// -------------------------------------------------------------------------
#ifndef __UIINIT_H__
#define __UIINIT_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndShowAnimate.h"

class KUiInit : protected KWndShowAnimate
{
public:
	//----�������ͳһ�Ľӿں���----
	static KUiInit*		OpenWindow(bool bStartMusic = true, bool bJustLauched = false);	//�򿪴��ڣ�����Ψһ��һ�������ʵ��
	static void			CloseWindow();							//�رմ���
	static void			PlayTitleMusic();
	static void			StopTitleMusic();
	static int			LimitGameWindown(int limit);
private:
	KUiInit() {}
	~KUiInit() {}
    void    ShowCompleted();
	void	Initialize();						//��ʼ��
	void	LoadScheme(const char* pScheme);	//������淽��
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnClickButton(KWndButton* pWnd);	//��Ӧ�����ť
	KWndButton*	GetActiveBtn();
	void	OnAutoLogin();
	int		OnKeyDown(unsigned int uKey);
private:
	static KUiInit* m_pSelf;
private:
	char		m_szLoginBg[32];

	KWndButton m_EnterGame;					// ������Ϸ
	KWndButton m_GameConfig;				// ��Ϸ����
	KWndButton m_OpenRep;				// ������Ա����
	KWndButton m_DesignerList;				// ������Ա����
	KWndButton m_ExitGame;					// �˳���Ϸ
	KWndButton m_EnterGameBorder;
	KWndButton m_GameConfigBorder;
	KWndButton m_OpenRepBorder;
	KWndButton m_ExitGameBorder;
};

#endif // __UIINIT_H__