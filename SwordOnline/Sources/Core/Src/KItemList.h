#ifndef	KItemListH
#define	KItemListH

#include "KLinkArray.h"
#include "KItem.h"
#define	MAX_ITEM_ACTIVE	2


//#define		EQUIPMENT_ROOM_WIDTH		6
//#define		EQUIPMENT_ROOM_HEIGHT		10
//#define		REPOSITORY_ROOM_WIDTH		6
//#define		REPOSITORY_ROOM_HEIGHT		10
//#define		TRADE_ROOM_WIDTH			6
//#define		TRADE_ROOM_HEIGHT			10


#define	REQUEST_EAT_MEDICINE	1
#define	REQUEST_EAT_OTHER		2

typedef struct
{
	int		nPlace;
	int		nX;
	int		nY;
} ItemPos;


class CORE_API KItemList
{
private:
	int			m_PlayerIdx;
	int			m_Hand;
	int			m_nBackHand;
	int			m_CompoundItem[MAX_COMPOUND_ITEM];

	static int	ms_ActiveEquip[itempart_num][MAX_ITEM_ACTIVE];	// ĳ�����ֵ�װ���������װ��
	static int	ms_ActivedEquip[itempart_num][MAX_ITEM_ACTIVE];	// ĳ�����ֵ�װ����ʲôװ������
	KLinkArray	m_FreeIdx;
	KLinkArray	m_UseIdx;
	int			m_nListCurIdx;									// ���� GetFirstItem �� GetNextItem
	BOOL		m_bMaskLock;
	int			m_BuildItem[MAX_PART_BUILD]; //TamLTM kham nam xanh
#ifndef _SERVER
	BOOL		m_bLockOperation;
#endif
public:
	PlayerItem	m_Items[MAX_PLAYER_ITEM];						// ���ӵ�е����е��ߣ�����װ���ŵĺ�������ŵģ���Ӧ��Ϸ�����е��������������
	PlayerItem	m_sBackItems[MAX_PLAYER_ITEM];					// ���׹����� m_Items �ı���
	KInventory	m_Room[room_num];
	int			m_EquipItem[itempart_num];						// ���װ���ĵ��ߣ���Ӧ��Ϸ�����е��������������
	int			FindSame(int nGameIdx);							// nGameIdxָ��Ϸ�����е�������ı��
	int			FindSame(DWORD dwID);
private:
	int			FindFree();
	BOOL		Fit(int nIdx, int nPlace);						// ����Ƿ����װ����
	BOOL		Fit(KItem* pItem, int nPlace);
	int			GetEquipPlace(int nType);						// ȡ��ĳ����װ��Ӧ�÷ŵ�λ��
	int			GetEquipEnhance(int nPlace);
	int			GetGoldEquipEnhance(int nPlace);
	int			GetActiveEquipPlace(int nPlace, int nCount);	// ȡ��nPlace��װ������ĵ�nCount��װ��λ��
	void		InfectionNextEquip(int nPlace, BOOL bEquip = FALSE);
#ifdef _SERVER
	BOOL		FindSameToRemove(int nItemNature, int nItemGenre, int nDetailType, int nLevel, int nSeries, int nRoom, int *pnIdx);
#endif
	friend	class KPlayer;

public:
	KItemList();
	~KItemList();
	int			Init(int nIdx);
	int			GetEquipment(int nIdx) { return m_EquipItem[nIdx]; }
	int			GetActiveAttribNum(int nIdx);
	int			GetGoldActiveAttribNum(int nIdx);
	int			GetWeaponType();								// ȡ�����������ͣ���������(equip_meleeweapon)����Զ������(equip_rangeweapon)
	int			GetWeaponParticular();							// ȡ�������ľ������ͣ����ڼ��㲻ͬ���˺���ǿ
	void		GetWeaponDamage(int* nMin, int* nMax);			// ȡ���������˺�
	int			Add(int nIdx, int nPlace, int nX, int nY, bool bAutoStack = true);		// nIdxָ��Ϸ�����е�������ı��
	int			Add(int nIdx, POINT ItemSize, bool bAutoStack = true);
	BOOL		SearchPosition(POINT ItemSize, ItemPos* pPos);
	BOOL		SearchPosition(int nWidth, int nHeight, ItemPos* pPos, bool bOverLookHand = false);
	BOOL		CanCombie(int Source,int Dest);

	BOOL		CompareRemoveItem(int Source,int Dest);
	BOOL		Remove(int nIdx);								// nIdxָ��Ϸ�����е�������ı��
	void		RemoveAll();
	BOOL		CanEquip(int nIdx, int nPlace = -1);			// nIdxָ��Ϸ�����е�������ı��
	BOOL		CanEquip(KItem* pItem, int nPlace = -1);
	BOOL		EnoughAttrib(void* pData);
	BOOL		Equip(int nIdx, int nPlace = -1);				// nIdxָ��Ϸ�����е�������ı��
	BOOL		UnEquip(int nIdx, int nPlace = -1);				// nIdxָ��Ϸ�����е�������ı��
	BOOL		NowEatItem(int nIdx);							// nIdxָ��Ϸ�����е�������ı��
	PlayerItem*	GetFirstItem();
	PlayerItem*	GetNextItem();
	int			SearchID(int nID);
	void		ExchangeMoney(int nSrcRoom, int DesRoom, int nMoney);
	void		WithDrawaMoney(int nSrcRoom, int DesRoom, int nMoney); // rut tien + pass ruong;
	void		ExchangeItem(ItemPos* SrcPos,ItemPos* DesPos);
	int			GetMoneyAmount();					// �õ���Ʒ���ʹ��������Ǯ��
	int			GetRepositoryMoney();
	int			GetEquipmentMoney();				// �õ���Ʒ���ʹ������Ǯ��
	int			GetTradeMoney();				// �õ���Ʒ���ʹ������Ǯ��
	BOOL		AddMoney(int nRoom, int nMoney);
	BOOL		CostMoney(int nMoney);
	BOOL		DecMoney(int nMoney);
	void		SetMoney(int nMoney1, int nMoney2, int nMoney3);
	void		SetRoomMoney(int nRoom, int nMoney);
	int			GetMoney(int nRoom) { return m_Room[nRoom].GetMoney(); }	// ȡ�ÿռ��Ǯ
	void		SetPlayerIdx(int nIdx);				// �趨�������
	int			Hand() { return m_Hand; };
	void		ClearRoom(int nRoom);
	void		BackupTrade();
	void		RecoverTrade();
	void		StartTrade();
	int			GetHorseIdx() { return m_EquipItem[itempart_horse];};
	BOOL		CompareItemList(const KBASICPROP_EQUIPMENT*);
	int			GetItemNum(int nItemGenre,int nDetailType,int nParticularType,int nLevel);
	BOOL		GetMaskLock() {return m_bMaskLock;};
	void		SetMaskLock(BOOL bFlag);
	int			GetCompoundItem(int nIdx) { return m_CompoundItem[nIdx]; }
	BOOL		PutCompound(int nIdx, int nPlace = -1);	
	void		DropCompound(int nIdx, int nPlace = -1);
	void    	ClearRoomItemOnly(int nRoom)
	{
		if (nRoom >= 0 && nRoom < room_num)
			this->m_Room[nRoom].ClearItem();
	}
	void		RemoveRoom(int nRoom);
	int			CalcFreeItemCellCount(int nWidth, int nHeight, int nRoom);
	int			GetBuildItem(int nIdx) { return m_BuildItem[nIdx];} //TamLTM kham nam xanh
	BOOL		BuildItem(int nIdx, int nPlace = -1);	//TamLTM kham nam xanh
	void		UnBuildItem(int nIdx, int nPlace = -1); //TamLTM kham nam xanh
	BOOL		IsEnoughToActive();
	int			GetPlayerFortune();
	int			HaveDamageItem(int nDur = 3);
	BOOL		CheckCanPlaceInEquipment(int nWidth, int nHeight, int *pnP, int *pnX, int *pnY);// �ж�һ���������Ʒ�ܷ�Ž���Ʒ��
	int			CountCommonItem(int nItemNature, int nItemGenre, int nDetailType = -1, int nLevel = -1, int nSeries = -1,  int P = pos_equiproom);
	int			GetItemCountRoom(int P = pos_equiproom);
	int			FindItem(int nItemNature, int nItemGenre, int nDetailType = -1, int nLevel = -1, int nSeries = -1);
	int			FindItemByIndex(int nItemNature, int nIndex);
#ifdef	_SERVER
	void		Abrade(int nType);
	void		TradeMoveMoney(int nMoney);			// ���ô˽ӿڱ��뱣֤�����nMoney��һ����Ч��(�����Ҳ���������Ǯ��)
	void		SendMoneySync();					// ��������moneyͬ����Ϣ���ͻ���
	int			RemoveCommonItem(int nCount, int nItemNature, int nItemGenre = -1, int nDetailType = -1, int nLevel = -1, int nSeries = -1, int P = pos_equiproom);
	void		GetTradeRoomItemInfo();				// �����а� trade room �е� item �� idx width height ��Ϣд�� itemset �е� m_psItemInfo ��ȥ
	BOOL		TradeCheckCanPlace();				// �������ж��������Ʒ�ܲ�����ȫ�Ž��Լ�����Ʒ��
	BOOL		EatItem(int nPlace, int nX, int nY);		// ��ʲô�ط���ҩ
	// �Զ���һ��ҩƷ��room_equipment�ƶ���room_immediacy
	BOOL		AutoMoveItemFromEquipmentRoom(int nItemIdx, int nSrcX, int nSrcY, int nDestX, int nDestY);
	// ��ʧ������Ʒ
	void		AutoLoseItemFromEquipmentRoom(int nRate);
	// ��ʧһ���������ϵ�װ��(�����)
	void		AutoLoseEquip();
    void		AutoDurationItem(int nRate);

	BOOL		RemoveItem(int nItemGenre,int nDetailType,int nParticularType,int nLevel);
	BOOL		RemoveItem(int nIdx, int nNum);
	BOOL		Lock(int nIdx, BOOL bLock);
	BOOL		UnLock(int nIdx, BOOL bUnLock);
	void		SyncItem(int nIdx, BOOL bIsNew = FALSE, int nPlace = 0, int nX = 0, int nY = 0, int nPlayerIndex = 0);
	void		SyncItemMagicAttrib(int nIdx);

	void		BackLocal();
	void		InsertEquipment(int nIdx, bool bAutoStack = false);
	void		ExecuteScript(int nIndex);
#endif
	int			PositionToIndex(int P, int i);
	

#ifndef	_SERVER
	int			UseItem(int nIdx);					// nIdxָ��Ϸ�����е�������ı��
	BOOL		SearchEquipment(int nWidth, int nHeight);
    int			ChangeItemInPlayer(int nIdx);//edit by phong kieu mac trang bi vao nguoi	
	BOOL		SearchStoreBox(int nRepositoryNum, int nWidth, int nHeight, ItemPos* pPos);
	BOOL		AutoMoveItem(ItemPos SrcPos,ItemPos DesPos);
	void		MenuSetMouseItem();
	int			GetSameDetailItemNum(int nImmediatePos);
	void		LockOperation();										// �����ͻ��˶�װ���Ĳ���
	void		UnlockOperation();
	BOOL		IsLockOperation() { return m_bLockOperation; };
#endif
};
#endif
