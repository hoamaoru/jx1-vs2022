// -------------------------------------------------------------------------
//	�ļ���		��	UiGive.h
//	��������	��	����ִ�����
// -------------------------------------------------------------------------
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"
#include "../Elem/WndMessageListBox.h"
#include "../../../core/src/gamedatadef.h"
struct KUiObjAtRegion;
class KUiGive : protected KWndImage
{
public:
	//----�������ͳһ�Ľӿں���----
	static KUiGive*	OpenWindow(const char* szName, char* szInitString);	//�򿪴��ڣ�����Ψһ��һ�������ʵ��
	static KUiGive*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);	//������淽��
	static void			CloseWindow(bool bDestroy);		//�رմ���
	void	OnClickItem(KUiDraggedObject* pItem);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);//��Ʒ�仯����
private:
	KUiGive();
	~KUiGive() {}
	void	Initialize();								//��ʼ��
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData(const char* szName, char* szInitString);
	void	OnOk(); //TamLTM Kham Xanh
private:
	static KUiGive*	m_pSelf;
	KWndText32			m_Title;
	KWndMessageListBox	m_ContentList;
	KWndScrollBar		m_ContentScroll;	//��ѡ���ֺ͹�����
	KWndObjectMatrix	m_ItemBox;
	KWndButton			m_Confirm;
	KWndButton			m_Cancel;
};

