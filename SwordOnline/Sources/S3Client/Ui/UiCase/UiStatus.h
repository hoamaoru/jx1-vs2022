/*******************Editer	: duccom0123 EditTime:	2024/06/12 11:48:45*********************
//	����--״̬����
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
------------------------------------------------------------------------------------------
	����11��װ���ڷ�λ�ã�����2��4���·�2��3��ͷ��2��2������2��1��
	����1��1��Ь��2��2����ָ1��1����׹1��2������1��1����2��3��
*****************************************************************************************/
#pragma once

#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../../../Core/Src/GameDatadef.h"
#include "../Elem/WndShowAnimate.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndPureTextBtn.h"	

enum UI_PLAYER_ATTRIBUTE;
struct KUiPlayerRuntimeInfo;
struct KUiObjAtRegion;
struct KUiPlayerAttribute;
struct KPopupMenuData;
#define _ITEM_COUNT 17

class KUiStatus : public KWndShowAnimate
{
public:
	//----�������ͳһ�Ľӿں���----
	static KUiStatus*	OpenWindow(bool bShow = true);					//�򿪴��ڣ�����Ψһ��һ�������ʵ��
	static KUiStatus*	GetIfVisible();					//�������������ʾ���򷵻�ʵ��ָ��
	static void			CloseWindow(bool bDestroy);		//�رմ��ڣ�ͬʱ����ѡ���Ƿ�ɾ������ʵ��
//	static void			LoadConfig(KIniFile* pIni);		//�����Զ�������
	static void			LoadScheme(const char* pScheme);//������淽��
	
	void	UpdateBaseData();							//���»������ݣ������Ȳ��ױ����ݣ�
	void	UpdateData();
	void	Breathe();
	void	UpdateRuntimeInfo(KUiPlayerRuntimeInfo* pInfo);
	void	UpdateAllEquips();
	void	UpdateRuntimeAttribute(KUiPlayerAttribute* pInfo);
	void	UpdateEquip(KUiObjAtRegion* pEquip, int bAdd);	//װ���仯����
	void	SwitchExpand(BOOL bShow);

private:
	KUiStatus() {}
	~KUiStatus() {}
	void	Initialize();								//��ʼ��	
	void	UseRemainPoint(int ntype, int nPoint);	//����ĳ������
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//���ں���
	void	LoadScheme(class KIniFile* pIni);			//������淽��
	void	OnEquiptChanged(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);//��Ӧ�����������װ���ĸı�
	void	OnUnequip(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	
private:
	static KUiStatus*	m_pSelf;

private:
	KWndText32	m_Name;
	KWndText32	m_Title;
	
	KWndText32	m_Life;
	KWndText32	m_Mana;
	KWndText32	m_Experience;
	KWndText32	m_Stamina;

	int			m_nRemainPoint;
	KWndText32	m_RemainPoint;

	KWndText32	m_Strength, m_Dexterity, m_Vitality, m_Energy;
	KWndButton	m_AddStrength, m_AddDexterity, m_AddVitality, m_AddEnergy;

	KWndText32	m_LeftDamage, m_RightDamage, m_Attack, m_Defence, m_MoveSpeed, m_AttackSpeed;
	KWndText32	m_PhyDef, m_CoolDef, m_LightDef, m_FireDef, m_PoisonDef;
	KWndText32	m_PhyDefPlus, m_CoolDefPlus, m_LightDefPlus, m_FireDefPlus, m_PoisonDefPlus;
	KWndText32	m_Level, m_StatusDesc;
	KWndText32	m_PKValue;
	KWndText32	m_Repute;
	KWndText32	m_FuYuan;
	KWndText32	m_TransLife;
	KWndText32	m_WorldRank;
	KWndImage	m_Avt;

	KWndButton	m_OpenItemPad;
	KWndButton	m_Close;

	KWndButton	m_EquipExpandBtn;
	KWndImage	m_EquipExpandImg;
	KWndButton	m_MaskFeature;
	KWndButton	m_UnlockBtn;
	KWndButton	m_BtnBind;
	KWndButton	m_BtnUnBind;	
//	KWndButton  m_Avatar;
//	KWndButton	m_ClickHere;
//	KWndText80	m_ClickHere1;
	KWndText80	m_chooseavatar;
	KWndButton  m_Avatar;
//	int 		m_nNumicon;
	int			nNumIcon;
	//װ����Ʒ
	KWndObjectBox	m_EquipBox[_ITEM_COUNT];
};
