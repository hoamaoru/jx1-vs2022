/*****************************************************************************************
//	����--���߽���
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../elem/WndButton.h"
#include "../elem/WndImage.h"
#include "../elem/WndText.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"
#include "../../../core/src/GameDataDef.h"

struct KUiObjAtRegion;

class KUiItem : public KWndShowAnimate
{
public:
	static KUiItem* OpenWindow(bool bFlag = true);				//�򿪴��ڣ�����Ψһ��һ�������ʵ��
	static KUiItem* GetIfVisible();				//�������������ʾ���򷵻�ʵ��ָ��
	static void		CloseWindow(bool bDestroy);	//�رմ��ڣ�ͬʱ����ѡ���Ƿ�ɾ������ʵ��
	static void		LoadScheme(const char* pScheme);//������淽��
	static void		OnNpcTradeMode(bool bTrue);
	void			UpdateItem(KUiObjAtRegion* pItem, int bAdd);//��Ʒ�仯����
	void			Breathe();									//�����
private:
	KUiItem() {}
	~KUiItem() {}
	void	Initialize();							//��ʼ��
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//���ں���
	void	UpdateData();
	void	OnClickItem(KUiDraggedObject* pItem, bool bDoImmed);
	void	OnRepairItem(KUiDraggedObject* pItem);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnGetMoney(int nMoney);
	void	OnBreakItem(KUiDraggedObject* pItem, bool bDoImmed);
	void	OnSetPrice(KUiDraggedObject* pItem);
private:
	static KUiItem*		m_pSelf;
private:
	int					m_nMoney;
	KWndText256			m_Money;
	KWndText256			m_ExtPoint;
	int					m_nExtPoint;

	KWndButton			m_GetMoneyBtn;
	KWndImage			m_TitleIcon, m_MoneyIcon, m_GoldIcon;
	KWndButton			m_CloseBtn;
	KWndObjectMatrix	m_ItemBox;
	KWndButton			m_OpenStatusPadBtn;
	KWndButton			m_MakeAdvBtn;
	KWndButton			m_MarkPriceBtn;
	KWndButton			m_MakeStallBtn;
	BYTE				m_byMark;

	char				m_szAdvStr[16];
};