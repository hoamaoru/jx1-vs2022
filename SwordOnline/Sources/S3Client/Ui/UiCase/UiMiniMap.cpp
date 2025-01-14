/*****************************************************************************************
//	����--С��ͼ
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiMiniMap.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "UiWorldMap.h"
#include "UiCaveList.h"
#include "UiSysMsgCentre.h"
#include "UiGetString.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../core/src/KCore.h"
#include    "../../Engine/Src/KSG_StringProcess.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include <time.h>
#include "UiChatItem.h"

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*			g_pCoreShell;
extern IInlinePicEngineSink *g_pIInlinePicSink;

#define	SCHEME_INI_NOPIC			"UiMiniMapNopic.ini"
#define	SCHEME_INI_SMALL			"UiMiniMapSmall.ini"
#define	SCHEME_INI_BIG				"UiMiniMapBig.ini"
#define	SCHEME_INI_BIGEX			"UiMiniMapBigEX.ini"
#define	defMSG_FORMAT_SCENEPOS 		"%d/%d"
#define	defMSG_FORMAT_SCENEPOSF 	"%d/%d t�m"

//��ͼ�϶���ԭ��ʱ��(��)
#define MAP_RECOVER_TIME		3000
#define	NOT_DRAGING_MAP			-1
#define	WAIT_TO_BE_SET_BACK		-2
#define	IS_DRAGING_MAP			(m_OldPos.x != NOT_DRAGING_MAP)
#define	IS_WAIT_TO_SET_BACK		(m_OldPos.x == WAIT_TO_BE_SET_BACK)

KUiMiniMap*			KUiMiniMap::ms_pSelf = NULL;
static MINIMAP_MODE	s_eMapMode = MINIMAP_M_NONE;
static MINIMAP_MODE	s_eMapOldMode = MINIMAP_M_BRIEF_PIC;

KUiMiniMap::KUiMiniMap()
{
	m_OldPos.x = NOT_DRAGING_MAP;
}

//--------------------------------------------------------------------------
//	���ܣ��򿪴��ڣ�����Ψһ��һ�������ʵ��
//--------------------------------------------------------------------------
KUiMiniMap* KUiMiniMap::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiMiniMap;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->Show();
		ms_pSelf->BringToTop();
	}
	return ms_pSelf;
}

//--------------------------------------------------------------------------
//	���ܣ��������������ʾ���򷵻�ʵ��ָ��
//--------------------------------------------------------------------------
KUiMiniMap* KUiMiniMap::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	���ܣ���ʼ��
//--------------------------------------------------------------------------
void KUiMiniMap::Initialize()
{
	AddChild(&m_Shadow);
	AddChild(&m_SceneName);

	//TamLTM Bang hoi chiem linh
	AddChild(&m_TongMapInfo1);
    AddChild(&m_TongMapInfo2);
	//end code

	AddChild(&m_ScenePos);
	AddChild(&m_SwitchBtn);
	AddChild(&m_WorldMapBtn);
	AddChild(&m_BtnFlag);
	AddChild(&m_CaveMapBtn);
	m_Style &= ~WND_S_VISIBLE;
	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_NORMAL);
}

//--------------------------------------------------------------------------
//	���ܣ��رմ���
//--------------------------------------------------------------------------
void KUiMiniMap::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->StopScrollMap();
		ms_pSelf->MapMoveBack();
		if (g_pCoreShell)
			g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING, SCENE_PLACE_MAP_ELEM_NONE, 0);
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

//--------------------------------------------------------------------------
//	���ܣ����봰�ڵĽ��淽��
//--------------------------------------------------------------------------
void KUiMiniMap::LoadScheme(const char* pScheme)
{
	if (ms_pSelf)
	{
		char		szBuf[128];
		KIniFile	Ini;
		if (s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC)
			sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_NOPIC);
		else if (s_eMapMode == MINIMAP_M_BRIEF_PIC_BROWSEEX)
			sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_BIGEX);
		else
		{
			//Ban do to o giua man hinh
		//	if (GetAsyncKeyState(VK_TAB))
		//	{
			//	sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_BIGEX);
		//	}
		//	else
		//	{
				sprintf(szBuf, "%s\\%s", pScheme,
					s_eMapMode == MINIMAP_M_BRIEF_PIC ? SCHEME_INI_SMALL : SCHEME_INI_BIG);
		//	}
		}

		if (Ini.Load(szBuf))
		{
			ms_pSelf->LoadScheme(&Ini);
		}
	}
}

void KUiMiniMap::LoadScheme(KIniFile* pIni)
{
	if (pIni == NULL)
		return;
	Init(pIni, "MiniMap");
	m_Shadow.Init(pIni, "NameShadow");
	m_SceneName.Init(pIni, "SceneName");

	//TamLTM Bang hoi chiem linh
	m_TongMapInfo1.Init(pIni, "TongMapInfo1");
    m_TongMapInfo2.Init(pIni, "TongMapInfo2");
	//end code

	m_ScenePos.Init(pIni, "ScenePos");
	m_SwitchBtn.Init(pIni, "SwitchBtn");
	m_WorldMapBtn.Init(pIni, "WorldMapBtn");
	m_BtnFlag.Init(pIni, "BtnFlag");
	m_CaveMapBtn.Init(pIni, "CaveMapBtn");
	
	pIni->GetInteger("MapRect", "Left",   0, (int*)&m_MapPos.x);
	pIni->GetInteger("MapRect", "Top",    0, (int*)&m_MapPos.y);
	pIni->GetInteger("MapRect", "Width",  0, (int*)&m_MapSize.cx);
	pIni->GetInteger("MapRect", "Height", 0, (int*)&m_MapSize.cy);

	if (IsVisible() && g_pCoreShell)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
			SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER | SCENE_PLACE_MAP_ELEM_PARTNER,
			(m_MapSize.cx | (m_MapSize.cy << 16)));
	}
}

//--------------------------------------------------------------------------
//	���ܣ����ں���
//--------------------------------------------------------------------------
int KUiMiniMap::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_SwitchBtn)
			MapSetMode(s_eMapMode == MINIMAP_M_BRIEF_PIC ? MINIMAP_M_BRIEF_PIC_BROWSE : MINIMAP_M_BRIEF_PIC);
		else if (uParam == (unsigned int)(KWndWindow*)&m_WorldMapBtn)
			MapSetMode(MINIMAP_M_WORLD_MAP);
		else if (uParam == (unsigned int)(KWndWindow*)&m_CaveMapBtn)
			MapSetMode(MINIMAP_M_CAVELIST_MAP);
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnFlag) // Button la co
		{
			BOOL bFlag = g_pCoreShell->GetFlagMode();
			g_pCoreShell->SetFlagMode(!bFlag);
			g_pCoreShell->SetPaintMode(!bFlag);
		//	g_DebugLog("SetPaintMode");
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ScenePos) //Box Nhap toa do game mini
		{
			//TamLTM Nhap toa do cho mini map
			KUiGetString::OpenWindow(GSA_NORMAL, "Xin nh�p t�a ��", "",
				(KWndWindow*)this, 0, 0,5, 9);
		//	g_DebugLog("%d + %d Xin nh�p t�a �� ");
		}
		break;
	case WM_LBUTTONDOWN: // Cam co tren mini map
		int nCursorX, nCursorY;
		Wnd_GetCursorPos(&nCursorX, &nCursorY);

		if (g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DIRECT_MAP, nCursorX, nCursorY))
		{
			g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DO_DIRECT_MAP, nCursorX, nCursorY);
		}
		else
		{
			g_pCoreShell->SetPaintMode(TRUE);
			g_pCoreShell->SetFlagMode(FALSE);
			g_pCoreShell->DirectFindPos(nCursorX, nCursorY, TRUE, TRUE);
		}

		//Cam co` thi set lai
		g_pCoreShell->SetAutoRun(FALSE);
		g_pCoreShell->SetFlagAutoRun(TRUE, ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);
	//	g_DebugLog("%d + %d 5 ", ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);

		break;
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MOUSEHOVER: //Di chuyen va cam co den dia diem map mini
		Wnd_GetCursorPos(&nCursorX, &nCursorY);
		if (g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DIRECT_MAP, nCursorX, nCursorY))
			Wnd_SwitchCursor(MOUSE_CURSOR_PICK);
		else
		{
			Wnd_SwitchCursor(MOUSE_CURSOR_NORMAL);
			Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);

		//	g_DebugLog("%d + %d WM_MOUSEHOVER ", nCursorX, nCursorY); //luc Chay luc ko chay
		}
	//	g_DebugLog("debug 14"); // De im chuot se chay ham nay
		break;
	case WND_N_CHILD_MOVE:
		if (uParam == (unsigned int)(KWndWindow*)&m_SceneName ||
			uParam == (unsigned int)(KWndWindow*)&m_ScenePos ||
			uParam == (unsigned int)(KWndWindow*)&m_Shadow)
		{
			KWndWindow* pWnd = (KWndWindow*)uParam;
			int nMoveOffsetX = (short)(nParam & 0xffff);
			int nMoveOffsetY = (short)((nParam >> 16) & 0xffff);

			int x, y;
			pWnd->GetPosition(&x, &y);
			x -= nMoveOffsetX;
			y -= nMoveOffsetY;
			pWnd->SetPosition(x, y);	//��ť���Ŵ��ڱ仯������Ҫ�ȱ��ԭ��
			GetPosition(&x, &y);
			x += nMoveOffsetX;
			y += nMoveOffsetY;
			SetPosition(x, y);
		}
	//	g_DebugLog("debug 1");
		break;
	case WM_RBUTTONDOWN:
		Wnd_SetCapture(this);
		Wnd_GetCursorPos((int *)&m_OldPos.x, (int *)&m_OldPos.y);
	//	g_DebugLog("2");
		break;
	case WM_MOUSEMOVE:
		Wnd_GetCursorPos(&nCursorX, &nCursorY);	
		if(IS_DRAGING_MAP && (uParam & MK_RBUTTON))
		{
			int nCursorX, nCursorY;
			Wnd_GetCursorPos(&nCursorX, &nCursorY);
			m_nOffsetX = m_OldPos.x - nCursorX;
			m_nOffsetY = m_OldPos.y - nCursorY;
			m_OldPos.x   =  nCursorX;
			m_OldPos.y   =  nCursorY;
			MapScroll(uParam & MK_CONTROL);
		}
		else if (g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DIRECT_MAP, nCursorX, nCursorY))
			Wnd_SwitchCursor(MOUSE_CURSOR_PICK);
		else
		{
			Wnd_SwitchCursor(MOUSE_CURSOR_NORMAL);
			Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);
		}
		g_pCoreShell->CheckHoverMouseMiniMap(TRUE);
	//	g_DebugLog("%d - %d WM_MOUSEMOVE 2", nCursorX, nCursorY);
		break;
	case WM_RBUTTONUP:
		if (IS_DRAGING_MAP && ((uParam & MK_CONTROL) == 0) && g_pCoreShell)
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 0);
		StopScrollMap();
	//	g_DebugLog("debug 17");
		break;
	case WND_M_OTHER_WORK_RESULT: // enter nhap toa do
		if (nParam > 0)
		{
			int x=0,y=0,i=0;
			const char* pcszTemp = (const char*)nParam;
			char Separator = 0;
			while(pcszTemp[i])
			{
				if(pcszTemp[i] < '0' || pcszTemp[i] > '9')
				{
					Separator = pcszTemp[i];
					break;
				}
				i++;
			}
			x = KSG_StringGetInt(&pcszTemp, 255);
			KSG_StringSkipSymbol(&pcszTemp, Separator);
			y = KSG_StringGetInt(&pcszTemp, 255);
			if (g_pCoreShell->GetFlagMode())
				g_pCoreShell->SetFlagMode(FALSE);

			g_pCoreShell->SetPaintMode(TRUE);
			g_pCoreShell->DirectFindPos(x, y, FALSE, TRUE);

			//TamLTM check auto run
			g_pCoreShell->SetAutoRun(TRUE);
			g_pCoreShell->SetFlagAutoRun(FALSE, 0, 0);

		//	g_DebugLog("WND_M_OTHER_WORK_RESULT %d + %d ", x * 8 * 32, y * 16 * 32);
		}
		break;
	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
//		g_DebugLog("debug else");
		g_pCoreShell->CheckHoverMouseMiniMap(FALSE);
		break;
	}
	return nRet;
}

//ֹͣ�϶���ͼ
void KUiMiniMap::StopScrollMap()
{
	if (IS_DRAGING_MAP)
	{
		m_uLastScrollTime = IR_GetCurrentTime();
		Wnd_ReleaseCapture();
		m_OldPos.x = WAIT_TO_BE_SET_BACK;
	}
}

//��ͼ��ԭԭʼƫ��
void KUiMiniMap::MapMoveBack()
{
	m_OldPos.x = NOT_DRAGING_MAP;
	m_nOffsetX = 0;
	m_nOffsetY = 0;
	if (g_pCoreShell)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 0);
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, 0, 0);
	}

}

void KUiMiniMap::PaintWindow()
{
	KWndImage::PaintWindow();
	if (g_pCoreShell)
	{
		int nX = m_MapPos.x + m_nAbsoluteLeft;
		int nY = m_MapPos.y + m_nAbsoluteTop;
		g_pCoreShell->SceneMapOperation(GSMOI_PAINT_SCENE_MAP, nX, nY);

		if (g_pRepresentShell)
		{
			KRURect	rect;
			rect.Color.Color_dw = 0xff000000;
			rect.oEndPos.nX = rect.oPosition.nX = nX - 1;
			rect.oEndPos.nY = rect.oPosition.nY = nY - 1;
			rect.oEndPos.nZ = rect.oPosition.nZ = 0;
			rect.oEndPos.nX += m_MapSize.cx;
			rect.oEndPos.nY += m_MapSize.cy;
			g_pRepresentShell->DrawPrimitives(1, &rect, RU_T_RECT, true);
		}
	}
}

void KUiMiniMap::Breathe()
{
	if (IS_WAIT_TO_SET_BACK && g_pCoreShell &&
		IR_IsTimePassed(MAP_RECOVER_TIME, m_uLastScrollTime))
	{
		MapMoveBack();
	}
	int nCursorX, nCursorY;	
	if (g_pCoreShell->GetFlagMode())
	{
		Wnd_GetCursorPos(&nCursorX, &nCursorY);	
		g_pCoreShell->DirectFindPos(nCursorX, nCursorY, TRUE, FALSE);
	}
}

//TamLTM change map
int changeNameMap = 1;
int saveIdMap = 0;
bool isCheckClosePostItem = false;
int countClosePostItem = 0;
//�����
void KUiMiniMap::UpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo)
{
	if (ms_pSelf && pInfo)
	{
		ms_pSelf->m_SceneName.SetText(pInfo->szSceneName);
		strcpy(ms_pSelf->m_szMapName,pInfo->szSceneName);

		//TamLTM Thue suat thanh thi
		char szinfo[256];

		if (pInfo->szTongName[0] && s_eMapMode != MINIMAP_M_BRIEF_PIC)
		{
			sprintf(szinfo,"Thu� su�t:%d%%, Ch� s� v�t gi�:%d%%",pInfo->szTongT, pInfo->szTongVG);
			ms_pSelf->m_TongMapInfo1.SetText(szinfo);

			sprintf(szinfo,"Th�i th�:%s, Bang h�i chi�m l�nh:%s",pInfo->szTongNameBC, pInfo->szTongName);
			ms_pSelf->m_TongMapInfo2.SetText(szinfo);
		}
		else
		{
			ms_pSelf->m_TongMapInfo1.SetText("");
			ms_pSelf->m_TongMapInfo2.SetText("");

		/*	if (s_eMapMode == MINIMAP_M_BRIEF_PIC)
			{
				
			}
			else
			{
				sprintf(szinfo,"Thu� su�t:0%%,Ch� s� v�t gi�:0%%");
				ms_pSelf->m_TongMapInfo1.SetText(szinfo);

				sprintf(szinfo,"Th�i th�: Ch�a c�, Bang h�i chi�m l�nh: Ch�a c�");
				ms_pSelf->m_TongMapInfo2.SetText(szinfo); 
			} // */
		}
		//end code

		ms_pSelf->m_MpsID = pInfo->nSceneId;
		ms_pSelf->m_MpsX = pInfo->nScenePos0/32/8;
		ms_pSelf->m_MpsY = pInfo->nScenePos1/32/16;

	/*	//TamLTM Check post item
		changeNameMap++;
		if (changeNameMap == 2)
		{
			saveIdMap = ms_pSelf->m_MpsID;
		}

		if (saveIdMap != pInfo->nSceneId)
		{
			KUiChatItem::CloseBoxPostItem();
			changeNameMap = 1;
		}

		if (saveIdMap == pInfo->nSceneId)
		{
			if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0 && isCheckClosePostItem)
			{
				countClosePostItem = 0;
				KUiChatItem::CloseBoxPostItem();
			}
			//Truong hop bam vao train cap ma ko bam tat post item di tu dong se close
			else if (isCheckClosePostItem)
			{
				countClosePostItem++;

				if (countClosePostItem == 150)
				{
					countClosePostItem = 0;
					KUiChatItem::CloseBoxPostItem();
				}
			}
		} // */

		//end code

		char Buff[16];
		if (g_pCoreShell->GetFlagMode() && g_pCoreShell->GetPaintMode())
		{
			KUiSceneTimeInfoOften Spot;
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO_OFTEN, (unsigned int)&Spot, 0);
			if (Spot.nScenePos0 && Spot.nScenePos1)
				sprintf(Buff, defMSG_FORMAT_SCENEPOSF, Spot.nScenePos0, Spot.nScenePos1);
			else
				sprintf(Buff, defMSG_FORMAT_SCENEPOSF, ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);

		//	g_DebugLog("%d + %d ", ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);
		}
		else
		{
			sprintf(Buff, s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC ? defMSG_FORMAT_SCENEPOS : defMSG_FORMAT_SCENEPOSF, ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);
		}	
			
		ms_pSelf->m_ScenePos.SetText(Buff);		

	//	g_DebugLog("%d + %d ", ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);
	}
}

//��ʾ����
void KUiMiniMap::Show()
{
	KWndImage::Show();
	if (g_pCoreShell)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
			SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER | SCENE_PLACE_MAP_ELEM_PARTNER,
			(ms_pSelf->m_MapSize.cx | (ms_pSelf->m_MapSize.cy << 16)));	
	}
}

//���ش���
void KUiMiniMap::Hide()
{
	KWndImage::Hide();
	if (g_pCoreShell)
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING, SCENE_PLACE_MAP_ELEM_NONE, 0);
}

//TamLTM Check
bool KUiMiniMap::CloseWindowOfPostItem(bool isCheck)
{
	isCheckClosePostItem = isCheck;
	return isCheck;
}
//end code

//��ͼ��
void KUiMiniMap::MapScroll(int nbScrollScene)
{
	KSceneMapInfo MapInfo;
    if(g_pCoreShell && g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_INFO, (unsigned int)&MapInfo, 0))
	{
		int nX = MapInfo.nOrigFocusH + MapInfo.nFocusOffsetH + MapInfo.nScallH * m_nOffsetX;
		int nY = MapInfo.nOrigFocusV + MapInfo.nFocusOffsetV + MapInfo.nScallV * m_nOffsetY;
		if (nX < MapInfo.nFocusMinH)
			nX = MapInfo.nFocusMinH; 
		else if (nX > MapInfo.nFocusMaxH)
			nX = MapInfo.nFocusMaxH;
		if (nY < MapInfo.nFocusMinV)
			nY = MapInfo.nFocusMinV;
		else if (nY > MapInfo.nFocusMaxV)
			nY = MapInfo.nFocusMaxV;

		nX -= MapInfo.nOrigFocusH;
		nY -= MapInfo.nOrigFocusV;

		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, nbScrollScene);
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, (unsigned int)nX, nY);
	}
}

void MapToggleStatus()
{
/**********************************************************************************
	if (s_eMapMode == MINIMAP_M_NONE || s_eMapMode == MINIMAP_M_WORLD_MAP || s_eMapMode == MINIMAP_M_CAVELIST_MAP)
		MapSetMode(s_eMapOldMode);
	else if (s_eMapMode == MINIMAP_M_BRIEF_PIC)
		MapSetMode(MINIMAP_M_BRIEF_PIC_BROWSE);
	else
		MapSetMode(s_eMapOldMode);
/**********************************************************************************/
	if (s_eMapMode == MINIMAP_M_NONE || s_eMapMode == MINIMAP_M_WORLD_MAP || s_eMapMode == MINIMAP_M_CAVELIST_MAP)
	{
		MapSetMode(MINIMAP_M_BRIEF_PIC);
	}
	else if (s_eMapMode == MINIMAP_M_BRIEF_PIC)
	{
		MapSetMode(MINIMAP_M_BRIEF_PIC_BROWSE);
	}
	else if (s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC)
	{
	}
	else
		MapSetMode(MINIMAP_M_NONE);
}

MINIMAP_MODE MapGetMode()
{
	return s_eMapMode;
}
//������ʾ����ģʽ
void MapSetMode(MINIMAP_MODE eMode)
{
	//if (eMode == s_eMapMode && (eMode < MINIMAP_M_NONE || eMode > MINIMAP_M_CAVELIST_MAP))
	if (s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC && eMode != MINIMAP_M_BRIEF_PIC && eMode < MINIMAP_M_WORLD_MAP)
		return;
	if ((s_eMapMode == MINIMAP_M_WORLD_MAP || s_eMapMode == MINIMAP_M_CAVELIST_MAP) && s_eMapOldMode == MINIMAP_M_BRIEF_NOT_PIC)
		eMode = MINIMAP_M_BRIEF_NOT_PIC;
	if (eMode == MINIMAP_M_BRIEF_PIC_BROWSEEX && s_eMapMode == MINIMAP_M_BRIEF_PIC_BROWSEEX)
		eMode = MINIMAP_M_BRIEF_PIC;
	if (eMode == s_eMapMode && (eMode < MINIMAP_M_NONE || eMode > MINIMAP_M_WORLD_MAP || eMode > MINIMAP_M_CAVELIST_MAP))
		return;
	if (s_eMapMode != MINIMAP_M_WORLD_MAP)
		s_eMapOldMode = s_eMapMode;
	switch(s_eMapMode = eMode)
	{
	case MINIMAP_M_NONE:			//û����ʾ��ͼ
		KUiMiniMap::CloseWindow();
		KUiWorldmap::CloseWindow();
		KUiCaveList::CloseWindow();
		break;
	case MINIMAP_M_BRIEF_NOT_PIC:
	case MINIMAP_M_BRIEF_PIC:		//����ͼ
	case MINIMAP_M_BRIEF_PIC_BROWSE://����ͼ���/�Ŵ�ģʽ
	case MINIMAP_M_BRIEF_PIC_BROWSEEX:
		KUiWorldmap::CloseWindow();
		KUiCaveList::CloseWindow();
		if (KUiMiniMap::GetIfVisible() == NULL)
			KUiMiniMap::OpenWindow();
		else
		{
			char Scheme[128];
			g_UiBase.GetCurSchemePath(Scheme, 256);
			KUiMiniMap::LoadScheme(Scheme);
		}
		break;
	case MINIMAP_M_WORLD_MAP:		//�����ͼ
		KUiMiniMap::CloseWindow();
		KUiWorldmap::OpenWindow();
		break;
	case MINIMAP_M_CAVELIST_MAP:
		KUiMiniMap::CloseWindow();
		KUiCaveList::OpenWindow();
		break;
	}
}
