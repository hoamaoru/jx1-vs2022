// -------------------------------------------------------------------------
//	�ļ���		��	UiTrade.h
//	������		��	Wooy(Wu yue)
//	����ʱ��	��	2002-12-21
//	��������	��	npc����ϵͳ����
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiPlayerShop.h"
#include "UiItem.h"
#include "UiTradeConfirmWnd.h"
#include "../../../core/src/CoreObjGenreDef.h"
#include "../../../core/src/coreshell.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "UiInformation.h"
#include "UiSysMsgCentre.h"
#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI		"UiPlayerShop.ini"

bool UiCloseWndsInGame(bool bAll);

KUiPlayerShop* KUiPlayerShop::m_pSelf = NULL;

KUiPlayerShop::KUiPlayerShop()
{
	m_pObjsList = NULL;
	m_nObjCount = 0;
	m_nPageCount = 0;
}

//--------------------------------------------------------------------------
//	���ܣ��������������ʾ���򷵻�ʵ��ָ��
//--------------------------------------------------------------------------
KUiPlayerShop* KUiPlayerShop::GetIfVisible()
{
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	���ܣ��򿪴��ڣ�����Ψһ��һ�������ʵ��
//--------------------------------------------------------------------------
KUiPlayerShop* KUiPlayerShop::OpenWindow(KUiPlayerItem* pDest)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiPlayerShop;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiCloseWndsInGame(false);
		g_UiBase.SetStatus(UIS_S_TRADE_BUY);

		if(KUiItem::GetIfVisible() ==FALSE)
			KUiItem::OpenWindow();
		KUiItem::OnNpcTradeMode(true);
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData(pDest);
		m_pSelf->BringToTop();
		m_pSelf->Show();
		m_pSelf->m_Name.SetText(pDest->Name,sizeof(pDest->Name));	
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	���ܣ��رմ��ڣ�ͬʱ����ѡ���Ƿ�ɾ������ʵ��
//--------------------------------------------------------------------------
void KUiPlayerShop::CloseWindow()
{
	if (m_pSelf)
	{
		g_pCoreShell->OperationRequest(GOI_PLAYER_RLTRADE,
										m_pSelf->m_Dest.uId, 0);
			
		KUiItem::OnNpcTradeMode(false);
		KUiTradeConfirm::CloseWindow(true);
		g_UiBase.SetStatus(UIS_S_IDLE);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

//��ʼ��
void KUiPlayerShop::Initialize()
{
	AddChild(&m_ItemsBox);
	AddChild(&m_Name);
	AddChild(&m_CloseBtn);
	
	m_nCount = 0;
	m_ItemsBox.SetContainerId((int)UOC_NPC_SHOP);
	Wnd_AddWindow(this);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}

//������淽��
void KUiPlayerShop::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{		
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
		{
			m_pSelf->KWndShowAnimate::Init(&Ini, "Main");
			m_pSelf->m_ItemsBox.Init(&Ini, "ItemBox");
			m_pSelf->m_Name.Init(&Ini,"TextName");
			m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");

			m_pSelf->m_ItemsBox.EnablePickPut(false);
		}
	}
}

void KUiPlayerShop::CancelTrade()
{
	CloseWindow();
}

//���ں���
int	KUiPlayerShop::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam, nParam);
		break;
	case WND_N_LEFT_CLICK_ITEM:
		OnBuyItem((KUiDraggedObject*)uParam);
		break;
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
		{
			if (g_UiBase.GetStatus() == UIS_S_TRADE_BUY)
			{
				CloseWindow();
			}
			else
			{
				g_UiBase.SetStatus(UIS_S_TRADE_BUY);
			}
		}
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiPlayerShop::OnBuyItem(KUiDraggedObject* pItem)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;

	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_NPC_SHOP;

	KUiItemBuySelInfo	Price = { 0 };
	if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
		(unsigned int)(&Obj), (int)(&Price)))
	{
		KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_BUYPLAYER);
	}
}

void KUiPlayerShop::OnClickButton(KWndButton* pWnd, int bCheck)
{
	if (Wnd_GetDragObj(NULL))
		return;
	if (pWnd == &m_CloseBtn)
		CloseWindow();
}


void KUiPlayerShop::UpdateData(KUiPlayerItem* pDest)
{

 		m_Dest = *pDest;

		UpdateItem();

}

void KUiPlayerShop::Clear()
{
	m_nObjCount  = 0;
	m_nPageCount = 0;
	if (m_pObjsList)
	{
		free(m_pObjsList);
		m_pObjsList = NULL;
	}
}


void KUiPlayerShop::UpdateItem()
{
	Clear();	
	
	m_ItemsBox.Clear();	

	int nCount = g_pCoreShell->GetGameData(GDI_TRADE_PLAYER_ITEM, 0, 0);
	if (nCount == 0)
		return;
	m_pObjsList = NULL;
	if (m_pObjsList = (KUiObjAtContRegion*)malloc(sizeof(KUiObjAtContRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_TRADE_PLAYER_ITEM, (unsigned int)m_pObjsList, nCount);//���߳�ִ�У�nCountֵ����

		for (int i = 0; i < nCount; i++)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = m_pObjsList[i].Obj.uGenre;
			Obj.uId = m_pObjsList[i].Obj.uId;
			Obj.DataX = m_pObjsList[i].Region.h;
			Obj.DataY = m_pObjsList[i].Region.v;
			Obj.DataW = m_pObjsList[i].Region.Width;
			Obj.DataH = m_pObjsList[i].Region.Height;
			m_ItemsBox.AddObject(&Obj, 1);
		}
		free(m_pObjsList);
		m_pObjsList = NULL;
	}
	m_nCount = nCount;
}