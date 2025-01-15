#ifndef KPROTOCOL_H
#define KPROTOCOL_H

#ifndef __linux
#ifdef _STANDALONE
#include "GameDataDef.h"
#else
#include "../Sources/Core/src/GameDataDef.h"
#endif
#else
#include "GameDataDef.h"
#include <string.h>
#endif

#include "KProtocolDef.h"
#include "KRelayProtocol.h"

#pragma pack(push, enter_protocol)
#pragma	pack(1)

#define	PROTOCOL_MSG_TYPE	BYTE
#define PROTOCOL_MSG_SIZE	(sizeof(PROTOCOL_MSG_TYPE))
#define	MAX_PROTOCOL_NUM	207

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		TeamServerID;
	BYTE	HelmType;
	BYTE	ArmorType;
	BYTE	WeaponType;
	BYTE	HorseType;
	BYTE	RankID;
	KExpandRank		ExpandRank;
	DWORD	TongNameID;
	char	TongName[16];
	int		TongNationalEmblem;
	char	TongAgname[16];
	BYTE	m_Figure;
	BYTE	m_btSomeFlag;
	BYTE	Translife;
	BYTE	Viprank;
	int		Repute;
	int		FuYuan;
	BYTE	PKFlag;
	BYTE	PKValue;
	BYTE	MantleLevel;
	int		MaskType;
	BYTE	MantleType;
	int		HelmPlayer;
	PLAYERTRADE	Trade;
	char	MateName[16];
	int 	RankInWorld;
	BYTE	PacePer;
	int		FactionNumber;
	BYTE	ImagePlayer;
} PLAYER_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		TeamServerID;
	BYTE	HelmType;
	BYTE	ArmorType;
	BYTE	WeaponType;
	BYTE	HorseType;
	BYTE	RankID;
	KExpandRank		ExpandRank;
	DWORD	TongNameID;
	char	TongName[16];
	int		TongNationalEmblem;
	char	TongAgname[16];
	BYTE	m_Figure;
	BYTE	m_btSomeFlag;
	BYTE	Translife;
	BYTE	Viprank;
	int		Repute;
	int		FuYuan;
	BYTE	PKFlag;
	BYTE	PKValue;
	BYTE	MantleLevel;
	int		MaskType;
	BYTE	MantleType;
	PLAYERTRADE	Trade;
	char	MateName[16];
	int 	RankInWorld;
	BYTE	PacePer;
	BYTE	ImagePlayer;
	int		FactionNumber;
} PLAYER_NORMAL_SYNC;


typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	Camp;				// ��Ӫ
	BYTE	CurrentCamp;		// ��ǰ��Ӫ
	BYTE	m_bySeries;			// ����ϵ
	int		CurrentLife;
	int		CurrentLifeMax;
	BYTE	m_btMenuState;		// ��ӡ����׵�״̬
	BYTE	m_Doing;			// ��Ϊ
	BYTE	m_btKind;			// npc����
	BYTE	Special;			// ��ǿ��NPC�������bit��ʾ��ǿ���ͣ������Ƿ��ɫ֮����ɼ�ǿ����Ŀ�ڿͻ���ȷ����
	int		MissionGroup;
	int		Dir;
	DWORD	MapX;				// λ����Ϣ
	DWORD	MapY;				// λ����Ϣ
	DWORD	ID;					// Npc��ΨһID
	int		NpcSettingIdx;		// �ͻ������ڼ��������Դ��������ֵ�趨
	char	m_szName[64];		// ����
} NPC_SYNC;

//TamLTM fix pos NPC
typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	DWORD	MapX;
	DWORD	MapY;
	BYTE	Doing;
} NPC_POS_SYNC;
//end code

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		MapX;
	int		MapY;
	BYTE	Camp;
	int		CurrentLife;
	int		CurrentLifeMax;
	BYTE	Doing;
	BYTE	State;
	BYTE	WalkSpeed;
	BYTE	RunSpeed;
	BYTE	AttackSpeed;
	BYTE	CastSpeed;
	BYTE	StateInfo[MAX_SKILL_STATE];
} NPC_NORMAL_SYNC;


typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	DWORD		MapX;
	DWORD		MapY;
	WORD		OffX;
	WORD		OffY;
} NPC_PLAYER_TYPE_NORMAL_SYNC;



//TamLTM Fix NPC PLAYER 
// s2c_npcstand
typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		MapX;
	int		MapY;
	int		OffX;
	int		OffY;
} NPC_PLAYER_TYPE_NORMAL_STAND_SYNC;

// s2c_npcsetpos
typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		MapX;
	int		MapY;
	int		OffX;
	int		OffY;
} NPC_PLAYER_TYPE_NORMAL_SET_POS_SYNC;
//end code
                 
typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
} NPC_SIT_SYNC, NPC_DEATH_SYNC, NPC_REQUEST_COMMAND, NPC_REQUEST_FAIL, NPC_MASK_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BOOL	Rv;
} NPC_REMOVE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_WALK_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	Type;
} NPC_REVIVE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_JUMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_RUN_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nFrames;
	int		nX;
	int		nY;
} NPC_HURT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	Camp;
} NPC_CHGCURCAMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	Camp;
} NPC_CHGCAMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nSkillID;
	int		nSkillLevel;
	int		nMpsX;
	int		nMpsY;
} NPC_SKILL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	char	szMsg[MAX_SENTENCE_LENGTH];
	int		nMsgLen;
} NPC_CHAT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	int		nSkillID;
	int		nMpsX;
	int		nMpsY;
	int		nNpcX;
	int		nNpcY;
	int     nNpcRadius;
} NPC_SKILL_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int		nMpsX;
	int		nMpsY;
} NPC_WALK_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
} NPC_REVIVE_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int		nMpsX;
	int		nMpsY;
} NPC_RUN_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	BuySellInfo m_BuySellInfo;
} SALE_BOX_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BOOL	bEquipEx;
	int		nBoxIndex;
} SOME_BOX_SYNC;

typedef struct 
{
	BYTE	ProtocolType;
	int		nNpcId;
} PLAYER_DIALOG_NPC_COMMAND; //������nNpcId�Ի�������

typedef struct
{
	BYTE	ProtocolType;		// Э������
	int		m_nExp;				// ��ǰ����
} PLAYER_EXP_SYNC;				// ���ͬ������

typedef struct
{
	BYTE	ProtocolType;		// Э������
	DWORD	dwID;
	DWORD	dwTimePacker;
} PLAYER_APPLY_CREATE_TEAM;		// �ͻ�����Ҵ������飬�������������

struct PLAYER_SEND_CREATE_TEAM_SUCCESS
{
	BYTE	ProtocolType;		// Э������
	DWORD	nTeamServerID;		// �����ڷ������ϵ�Ψһ��ʶ
	PLAYER_SEND_CREATE_TEAM_SUCCESS() {nTeamServerID = -1;}
};	// ������֪ͨ��Ҷ��鴴���ɹ�

typedef struct
{
	BYTE	ProtocolType;		// Э������
	BYTE	m_btErrorID;		// ���鴴�����ɹ�ԭ��0 ͬ�� 1 ��ұ����Ѿ�����ĳһ֧���� 3 ��ǰ���ڲ������״̬
} PLAYER_SEND_CREATE_TEAM_FALSE;// ������֪ͨ�ͻ��˶��鴴�����ɹ�

typedef struct
{
	BYTE	ProtocolType;		// Э������
	DWORD	m_dwTarNpcID;		// ��ѯĿ�� npc id
} PLAYER_APPLY_TEAM_INFO;		// �ͻ���������������ѯĳ��npc��������

typedef struct
{
	BYTE	ProtocolType;		// Э������
} PLAYER_APPLY_TEAM_INFO_FALSE;	// ��������֪�ͻ��������ѯĳ��npc��������ʧ��

typedef struct PLAYER_SEND_TEAM_INFO_DATA
{
	BYTE	ProtocolType;		// Э������
	int		m_nCaptain;			// �ӳ� npc id
	int		m_nMember[MAX_TEAM_MEMBER];	// ���ж�Ա npc id
	DWORD	nTeamServerID;		// �����ڷ������ϵ�Ψһ��ʶ
	PLAYER_SEND_TEAM_INFO_DATA() {nTeamServerID = -1;};
} PLAYER_SEND_TEAM_INFO;		// ��������ͻ��˷���ĳ���������Ϣ����

typedef struct PLAYER_SEND_SELF_TEAM_INFO_DATA
{
	BYTE	ProtocolType;							// Э������
	BYTE	m_btState;								// ����״̬
	DWORD	m_dwNpcID[MAX_TEAM_MEMBER + 1];			// ÿ����Ա��npc id ���ӳ����ڵ�һλ��
	char	m_szNpcName[MAX_TEAM_MEMBER + 1][32];	// ÿ����Ա�����֣��ӳ����ڵ�һλ��
	DWORD	nTeamServerID;							// �����ڷ������ϵ�Ψһ��ʶ
	DWORD	m_nLeadExp;							// ��ҵ�ͳ��������
	BYTE	m_btLevel[MAX_TEAM_MEMBER + 1];			// ÿ����Ա�ĵȼ����ӳ����ڵ�һλ��
	PLAYER_SEND_SELF_TEAM_INFO_DATA() {memset(m_szNpcName, 0, 32 * (MAX_TEAM_MEMBER + 1)); nTeamServerID = -1;};
} PLAYER_SEND_SELF_TEAM_INFO;						// ��������ͻ��˷��Ϳͻ�������������Ϣ����

typedef struct
{
	BYTE	ProtocolType;		// Э������
	DWORD	dwID;
	DWORD	dwTimePacker;
	BYTE	m_btStateClose;
	BYTE	m_btFlag;		// �򿪻�ر�
} PLAYER_TEAM_OPEN_CLOSE;		// ����ӳ�����������뿪�š��رն����Ƿ�������ճ�Ա״̬

typedef struct
{
	BYTE	ProtocolType;		// Э������
	DWORD	m_dwTarNpcID;		// Ŀ�����ӳ�npc id ���� ������ npc id
} PLAYER_APPLY_ADD_TEAM;		// �����������������ĳ��������߷�������ĳ���ӳ�ת��ĳ����ҵļ�������

typedef struct
{
	BYTE	ProtocolType;		// Э������
	DWORD	m_dwNpcID;			// ������������npc id
} PLAYER_ACCEPT_TEAM_MEMBER;	// ���֪ͨ����������ĳ����������

typedef struct PLAYER_TEAM_ADD_MEMBER_DATA
{
	BYTE	ProtocolType;		// Э������
	BYTE	m_btLevel;			// �����ߵȼ�
	DWORD	m_dwNpcID;			// ������npc id
	char	m_szName[32];		// ����������
	PLAYER_TEAM_ADD_MEMBER_DATA() {memset(m_szName, 0, 32);};
} PLAYER_TEAM_ADD_MEMBER;		// ������֪ͨ�����еĸ���������³�Ա����

typedef struct
{
	BYTE	ProtocolType;		// Э������
	BOOL	bMySelf;
} PLAYER_APPLY_LEAVE_TEAM;		// �ͻ�������������

typedef struct
{
	BYTE	ProtocolType;		// Э������
	DWORD	m_dwNpcID;			// ���npc id
} PLAYER_LEAVE_TEAM;			// ������֪ͨ����Աĳ�����

typedef struct
{
	BYTE	ProtocolType;		// Э������
	DWORD	m_dwNpcID;			// ���npc id
} PLAYER_TEAM_KICK_MEMBER;		// �ӳ��߳�ĳ����Ա

typedef struct
{
	BYTE	ProtocolType;		// Э������
	DWORD	m_dwNpcID;			// Ŀ��npc id
	BOOL	m_bMySelf;
} PLAYER_APPLY_TEAM_CHANGE_CAPTAIN;// �ӳ��������������Լ��Ķӳ���ݽ�����Ķ�Ա

typedef struct
{
	BYTE	ProtocolType;		// Э������
	DWORD	m_dwCaptainID;		// �¶ӳ�npc id
	DWORD	m_dwMemberID;		// �¶�Աnpc id
	BOOL	m_bMySelf;
} PLAYER_TEAM_CHANGE_CAPTAIN;	// ������֪ͨ����Ա�����ӳ�

typedef struct
{
	BYTE	ProtocolType;		// Э������
} PLAYER_APPLY_TEAM_DISMISS;	// ������������ɢ����

typedef struct
{
	BYTE	ProtocolType;		// Э������
	BYTE	m_btPKFlag;			// pk ����
} PLAYER_SET_PK;				// �����������򿪡��ر�PK

typedef struct
{
	BYTE	ProtocolType;			// Э������
	BYTE	m_btCamp;				// ����Ӫ
	BYTE	m_btCurFaction;			// ��ǰ����
	BYTE	m_btFirstFaction;		// �״μ�������
	int		m_nAddTimes;			// �������ɴ���
} PLAYER_FACTION_DATA;				// �����������ͻ���������Ϣ

typedef struct
{
	BYTE	ProtocolType;			// Э������
} PLAYER_LEAVE_FACTION;				// ������֪ͨ����뿪����

typedef struct
{
	BYTE	ProtocolType;			// Э������
	int		m_nMissionGroup;				// ����Ӫ
} PLAYER_MISSION_DATA;				// �����������ͻ���������Ϣ

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btState;
	int		m_nID;
	int		m_nDataID;
	int		m_nXpos;
	int		m_nYpos;
	int		m_nMoneyNum;
	int		m_nItemID;
	BYTE	m_btDir;
	BYTE	m_btItemWidth;
	WORD	m_wCurFrame;
	BYTE	m_btItemHeight;
	BYTE	m_btColorID;
	int		m_nGenre;
	int		m_nDetailType;
	BYTE	m_btFlag;
	char	m_szName[OBJ_NAME_LENGTH];
} OBJ_ADD_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btState;
	int		m_nID;
} OBJ_SYNC_STATE;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btDir;
	int		m_nID;
} OBJ_SYNC_DIR;

typedef struct
{
	BYTE	ProtocolType;
	int		m_nID;
	BYTE	m_btSoundFlag;
} OBJ_SYNC_REMOVE;

typedef struct
{
	BYTE	ProtocolType;
	int		m_nID;
} OBJ_CLIENT_SYNC_ADD;

typedef struct
{
	BYTE	ProtocolType;		// Э������
	int		m_nLeadLevel;		// ͳ��������ֵ
	int		m_nLeadExp;		// ͳ��������ֵ
} PLAYER_LEAD_EXP_SYNC;			// ͬ��ͳ��������ֵ

typedef struct
{
	BYTE	ProtocolType;		// Э������
	BOOL	m_bSetLevel;
	BYTE	m_btLevel;			// ��ǰ�ȼ�
	int		m_nExp;				// ��ǰ����
	int		m_nAttributePoint;	// ʣ�����Ե�
	int		m_nSkillPoint;		// ʣ�༼�ܵ�
	int		m_nBaseLifeMax;		// ��ǰ�������ֵ
	int		m_nBaseStaminaMax;	// ��ǰ�������ֵ
	int		m_nBaseManaMax;		// ��ǰ�������ֵ
} PLAYER_LEVEL_UP_SYNC;			// �������

typedef struct
{
	BYTE	ProtocolType;		// Э������
	BYTE	m_btLevel;			// ��ǰ�ȼ�
	DWORD	m_dwTeammateID;		// ���� npc id
} PLAYER_TEAMMATE_LEVEL_SYNC;	// ���������ʱ��֪ͨ����

typedef struct
{
	BYTE	ProtocolType;		// Э������
	BYTE	m_btAttribute;		// ����(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	int		m_nAddNo;			// �ӵĵ���
} PLAYER_ADD_BASE_ATTRIBUTE_COMMAND;	// �����ӻ������Ե�

typedef struct
{
	BYTE	ProtocolType;		// Э������
	int		m_nSkillID;			// ����id
	int		m_nAddPoint;		// Ҫ�ӵĵ���
} PLAYER_ADD_SKILL_POINT_COMMAND;// �����������ĳ�����ܵĵ���

typedef struct
{
	BYTE	ProtocolType;		// Э������
	BYTE	m_btAttribute;		// ����(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	int		m_nBasePoint;		// ��������
	int		m_nCurPoint;		// ��ǰ����
	int		m_nLeavePoint;		// ʣ��δ�������Ե�
} PLAYER_ATTRIBUTE_SYNC;		// ���ͬ�����Ե�

typedef struct
{
	BYTE	ProtocolType;		// Э������
	int		m_nSkillID;			// ����id
	int		m_nSkillLevel;		// ���ܵȼ�
	int		m_nAddLevel;		// װ����ӵļ��ܵ�
	int		m_nSkillExp;		// ���ܵȼ�
	BOOL	m_bTempSkill;
	int		m_nLeavePoint;		// ʣ��δ���似�ܵ�
} PLAYER_SKILL_LEVEL_SYNC;		// ���ͬ�����ܵ�

//typedef struct
//{
//	BYTE	ProtocolType;		// Э������
//	int		m_nItemID;			// ��Ʒid
//	int		m_nSourcePos;		// ��Դλ��
//	int		m_nTargetPos;		// Ŀ��λ��
//} PLAYER_EQUIP_ITEM_COMMAND;	// �������Ҽ����ʹ����Ʒ(װ��)

typedef struct
{
	BYTE	ProtocolType;		// Э������
	BYTE	m_btPlace;			// ҩƷλ��
	BYTE	m_btX;				// ҩƷλ��
	BYTE	m_btY;				// ҩƷλ��
	int		m_nItemID;			// ��Ʒid
} PLAYER_EAT_ITEM_COMMAND;		// �������Ҽ����ʹ����Ʒ(��ҩ)

typedef struct
{
	BYTE	ProtocolType;		// Э������
	int		m_nObjID;			// �������obj��id
	BYTE	m_btPosType;		// λ������
	BYTE	m_btPosX;			// ���� x
	BYTE	m_btPosY;			// ���� y
} PLAYER_PICKUP_ITEM_COMMAND;		// ��һ����Ʒ���������ͼ�ϵ�obj��

typedef struct
{
	BYTE			ProtocolType;		// Э������
	BOOL			m_bIsNew;
	BOOL			m_bTemp;
	DWORD			m_dwOwner;
	int				m_TradePrice;
	int				m_ID;				// ��Ʒ��ID
	BYTE			m_Nature;
	BYTE			m_Genre;			// ��Ʒ������
	int				m_Detail;			// ��Ʒ�����
	int				m_Particur;			// ��Ʒ����ϸ���
	BYTE			m_Series;			// ��Ʒ������
	BYTE			m_Level;			// ��Ʒ�ĵȼ�
	BYTE			m_btPlace;			// ����
	BYTE			m_btX;				// ����
	BYTE			m_btY;				// ����
	PlayerItem		m_BackLocal;
	int				m_ItemX;
	int				m_ItemY;
	int				m_Luck;				// MF
	int				m_MagicLevel[MAX_ITEM_MAGICLEVEL];	// ���ɲ���
	WORD			m_Version;			// װ���汾
	int				m_Durability;		// �;ö�
	UINT			m_RandomSeed;		// �������
	BYTE			m_bStack;
	int				m_StackNum;
	int				m_ExpireTime;
	KLockItem		m_LockItem;
	BOOL			m_bLockSell;
	BOOL			m_bLockTrade;
	BOOL			m_bLockDrop;
	int				m_Param;
	int				m_Mantle;
	int				m_Width;
	int				m_Height;
	int				m_Fortune;
	int				m_Kind; //TamLTM Kham nam xanh
} ITEM_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD			m_dwID;
	int				m_MagicLevel[MAX_ITEM_MAGICLEVEL];
	KMagicAttrib	m_MagicAttrib[MAX_ITEM_MAGICATTRIB];
} ITEM_SYNC_MAGIC;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_ID;				// ��Ʒ��ID
} ITEM_REMOVE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_ID;				// ��Ʒ��ID
	BYTE			m_Number;			
} PLAYER_SELL_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_Shop;
	BYTE			m_BuyIdx;			// ��ڼ�������
	BYTE			m_Number;			// new add
	//TamLTM Bang hoi chiem linh
//	BYTE			m_X;				// ����X
//	BYTE			m_Y;				// ����Y
	//end code
} PLAYER_BUY_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_nMoney1;			// װ����
	int				m_nMoney2;			// ������
	int				m_nMoney3;			// ������
} PLAYER_MONEY_SYNC;					// ������֪ͨ�ͻ���Ǯ������

typedef struct
{
	BYTE			ProtocolType;		// Э������
	BYTE			m_btDownPos;
	BYTE			m_btDownX;
	BYTE			m_btDownY;
	BYTE			m_btUpPos;
	BYTE			m_btUpX;
	BYTE			m_btUpY;
} PLAYER_MOVE_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	BYTE			m_btDownPos;
	BYTE			m_btDownX;
	BYTE			m_btDownY;
	BYTE			m_btUpPos;
	BYTE			m_btUpX;
	BYTE			m_btUpY;
} PLAYER_MOVE_ITEM_SYNC;


typedef struct
{
	BYTE			ProtocolType;		// Э������
	BYTE			m_btSrcPos;
	BYTE			m_btSrcX;
	BYTE			m_btSrcY;
	BYTE			m_btDestPos;
	BYTE			m_btDestX;
	BYTE			m_btDestY;
} ITEM_AUTO_MOVE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// Э������
} PLAYER_THROW_AWAY_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int 			m_shLife;
	int 			m_shStamina;
	int 			m_shMana;
	short			m_shAngry;
	BYTE			m_btTeamData;
} CURPLAYER_NORMAL_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	// npc����
	DWORD			m_dwID;				// Npc��ID
	BYTE			m_btLevel;			// Npc�ĵȼ�
	BYTE			m_btSex;			// �Ա�
	BYTE			m_btKind;			// Npc������
	BYTE			m_btSeries;			// Npc������ϵ
	int 			m_wLifeMax;			// Npc���������
	int 			m_wStaminaMax;		// Npc���������
	int 			m_wManaMax;			// Npc���������
	int				m_HeadImage;
	// player ����
	WORD			m_wAttributePoint;	// δ�������Ե�
	WORD			m_wSkillPoint;		// δ���似�ܵ�
	WORD			m_wStrength;		// ��ҵĻ������������������˺���
	WORD			m_wDexterity;		// ��ҵĻ������ݣ��������С�������
	WORD			m_wVitality;		// ��ҵĻ�������������������������
	WORD			m_wEngergy;			// ��ҵĻ�������������������
	WORD			m_wLucky;			// ��ҵĻ�������ֵ
	int				m_nExp;				// ��ǰ����ֵ(��ǰ�ȼ���npc����)
	BYTE			m_byTranslife;
	int				m_nLeadExp;		// ͳ��������ֵ

	// ����
	BYTE			m_btCurFaction;		// ��ǰ����
	BYTE			m_btFirstFaction;	// ��һ�μ�������ĸ�����
	int				m_nFactionAddTimes;	// ����������ɵ��ܴ���
	
	int				m_nMissionGroup;
	int				m_nRoomId;
	// ����
	WORD			m_wWorldStat;		// ��������
	WORD			m_wSectStat;		// ��������
	int				m_nKillPeopleNumber;
	// Ǯ
	int				m_nMoney1;
	int				m_nMoney2;
	DWORD			m_dwEquipExpandTime;
	BYTE			m_btRepositoryNum;
	DWORD			m_dwLeaveTongTime;
	BYTE			m_btImagePlayer;
	int				m_nExtPoint; // send xu
} CURPLAYER_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wProtocolLong;
	BYTE	m_nOperateType;				//��������
	BYTE	m_bUIId, m_bOptionNum, m_bParam1, m_bParam2;// m_bParam1,����Ϣ�����ֱ�ʶ�����ַ�����ʶ, m_bParam2,�Ƿ����������������ѡ�����
	BOOL	m_bParam;
	int		m_nParam, m_nParam1;
	int		m_nBufferLen;
	char	m_szKey[MAX_PATH];
	char	m_pContent[MAX_SCIRPTACTION_BUFFERNUM];				//�����Ʒ�
} PLAYER_SCRIPTACTION_SYNC;

typedef struct
{
	WORD	SkillId;
	BYTE	SkillLevel;
	int		SkillExp;
} SKILL_SEND_ALL_SYNC_DATA;

typedef struct
{
	BYTE						ProtocolType;
	WORD						m_wProtocolLong;
	SKILL_SEND_ALL_SYNC_DATA	m_sAllSkill[MAX_NPCSKILL];
} SKILL_SEND_ALL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	WeatherID;
} SYNC_WEATHER;

typedef struct defWORLD_SYNC
{
	BYTE	ProtocolType;
	int		SubWorld;
	int		Region;
	BYTE	Weather;
	DWORD	Frame;

	//TamLTM Bang hoi Chiem linh 
	char    TongName[32];
	char    TongNameBC[32];
	int     TongT;
	int     TongVG;
	BYTE    CheckTong;
	//end code
} WORLD_SYNC;

typedef struct 
{
	BYTE	ProtocolType;
	int		nSelectIndex;
	int		nSelectUi;
}PLAYER_SELECTUI_COMMAND;

typedef struct
{
	BYTE			ProtocolType;	// c2s_viewequip
	DWORD			m_dwNpcID;
} VIEW_EQUIP_COMMAND;

typedef struct
{
	BYTE			ProtocolType;	// c2s_viewequip
	DWORD			m_dwNpcID;
} PLAYER_TRADE_VIEW_COMMAND;

//�˽ṹ�Ѿ���tagDBSelPlayer�ṹ�滻
/*typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_nSelect;			// 
} DB_PLAYERSELECT_COMMAND;*/

/*
 * { Add by liupeng 2003.05.10
 *
 * #pragma pack( push, 1 )
*/

/*
 * Nonstandard extension used : zero-sized array in struct/union
 */
#pragma warning(disable: 4200)

#define KSG_PASSWORD_MAX_SIZE   64

typedef struct tagKSG_PASSWORD
{
    char szPassword[KSG_PASSWORD_MAX_SIZE];    // ���ڲ���MD5���ַ�����������32���ַ�������ĩβ'\0'����Ҫ����33���ռ䣬���ʹ��64
} KSG_PASSWORD;

#define _NAME_LEN	32

struct tagProtoHeader
{
	BYTE	cProtocol;
};

struct tagResult : public tagProtoHeader
{
	BYTE	cResult;
};

struct tagDBSelPlayer : public tagProtoHeader
{
	char	szRoleName[_NAME_LEN];
};

struct tagDBDelPlayer : public tagProtoHeader
{
	char	        szAccountName[_NAME_LEN];
    KSG_PASSWORD    Password;
	char	        szRoleName[_NAME_LEN];
};

//ɾ�����½���ɫ�ķ�����Ϣ��������
struct tagNewDelRoleResponse : public tagDBSelPlayer
{
	bool	bSucceeded;		//�Ƿ�ɹ�
};

// 2003.05.11
struct tagDBSyncPlayerInfo : public tagProtoHeader
{
	size_t	dataLength;
	char	szData[0];
};

/*
 * 2003.06.27
 * s2c_gateway_broadcast
*/

#define	AP_WARNING_ALL_PLAYER_QUIT			1
#define	AP_NOTIFY_GAMESERVER_SAFECLOSE		2
#define	AP_NOTIFY_ALL_PLAYER				3

#define	AP_PUNISH_LOCK						1
#define	AP_PUNISH_CHAT						2
#define	AP_LOCK_ACCOUNTNAME					1
#define	AP_UNLOCK_ACCOUNTNAME				0
#define	AP_LOCK_CHAT						2
#define	AP_UNLOCK_CHAT						3
#define	MAX_GATEWAYBROADCAST_LEN	260
struct tagGatewayBroadCast : public tagProtoHeader
{
	UINT	uCmdType;
	char	szData[MAX_GATEWAYBROADCAST_LEN];
};

/*
 * 2003.05.22
 * s2c_syncgamesvr_roleinfo_cipher
*/
struct tagGuidableInfo : public tagProtoHeader
{
	GUID guid;
	WORD nExtPoint;			//TamLTM fix xu;
	WORD nChangePoint;		//�仯�ĸ��͵�
	size_t	datalength;
	char	szData[0];

};

/*
 * c2s_permitplayerlogin
 */
struct tagPermitPlayerLogin : public tagProtoHeader
{
	GUID guid;

	BYTE szRoleName[_NAME_LEN];
	BYTE szAccountName[_NAME_LEN];

	/*
	 * Succeeded : true
	 * Failed	 : false
	 */
	bool bPermit;
};

struct tagPermitPlayerExchange
{
	BYTE cProtocol;
	GUID guid;
	DWORD dwIp;
	WORD wPort;
	bool bPermit;
};
/*
 * c2s_notifyplayerlogin
 */
struct tagNotifyPlayerLogin : public tagPermitPlayerLogin
{
	UINT			nIPAddr;
	unsigned short	nPort;
};

/*
 * s2c_querymapinfo
 */
struct tagQueryMapInfo : public tagProtoHeader
{	
};

/*
 * s2c_querygameserverinfo
 */
struct tagQueryGameSvrInfo : public tagProtoHeader
{	
};

/*
 * s2c_notifysvrip
 */
struct tagNotifySvrIp : public tagProtoHeader
{
	WORD	pckgID;

	BYTE	cIPType;
	DWORD	dwMapID;

	DWORD	dwSvrIP;
};

/*
 * s2c_notifyplayerexchange
 */
struct tagNotifyPlayerExchange : public tagProtoHeader
{
	GUID			guid;
	UINT			nIPAddr;
	unsigned short	nPort;
};

/*
 * c2s_requestsvrip
 */

/*
 * BYTE	cIPType
 */
#define INTRANER_IP	0
#define INTERNET_IP 1

struct tagRequestSvrIp : public tagProtoHeader
{
	WORD	pckgID;

	BYTE	cIPType;
	DWORD	dwMapID;
};

/*
 * c2c_notifyexchange
 */
struct tagSearchWay : public tagProtoHeader
{
	int		lnID;
	int		nIndex;
	DWORD	dwPlayerID;
};

/*
 * c2s_updatemapinfo
 */
struct tagUpdateMapID : public tagProtoHeader
{
	/*
	 * For example : Are your clear older information when it 
	 *		update local informatin
	 */
	BYTE cReserve;

	int cMapCount;

	BYTE szMapID[0];	// C4200 warning
};

/*
 * c2s_updategameserverinfo
 */
struct tagGameSvrInfo : public tagProtoHeader
{
	UINT			nIPAddr_Intraner;
	UINT			nIPAddr_Internet;

	unsigned short	nPort;
	WORD			wCapability;
};

/*
 * s2c_identitymapping
 */
struct tagIdentityMapping : public tagGameSvrInfo
{
	GUID guid;
};

/*
 * c2s_logiclogin
 * s2c_gmgateway2relaysvr
 * s2c_gmnotify
 */
struct tagLogicLogin : public tagProtoHeader
{
	char szName[32];//TAMLTM	
	GUID guid;
};

/*
 * s2c_logiclogout
 */
struct tagLogicLogout : public tagProtoHeader
{
	BYTE szRoleName[_NAME_LEN];
};

/*
 * c2s_registeraccount
 */
struct tagRegisterAccount : public tagProtoHeader
{
	BYTE szAccountName[_NAME_LEN];
};

/*
 * c2s_entergame
 */
struct tagEnterGame : public tagProtoHeader
{
	/*
	 * Succeeded : content is account name
	 * Failed	 : content is null
	 */

	BYTE szAccountName[_NAME_LEN];
};

struct tagEnterGame2 : public EXTEND_HEADER
{
	char szAccountName[_NAME_LEN];
	char szCharacterName[_NAME_LEN];
	DWORD	dwNameID;
	unsigned long	lnID;
};

/*
 * c2s_leavegame
 */

/*
 * BYTE cCmdType
 */
#define NORMAL_LEAVEGAME	0x0		// lock account
#define HOLDACC_LEAVEGAME	0x1A	// clear resource but don't to unlock account 

struct tagLeaveGame : public tagProtoHeader
{
	BYTE cCmdType;
	WORD nExtPoint;        //TamLTM fix xu;
	/*
	 * Succeeded : content is account name
	 * Failed	 : content is null
	 */

	char szAccountName[_NAME_LEN];
};

struct tagLeaveGame2 : public EXTEND_HEADER
{
	BYTE cCmdType;
	char szAccountName[_NAME_LEN];
};
/*
*  c2s_registerfamily
*/
struct tagRegisterFamily : public tagProtoHeader
{
	BYTE bRegister;		//1 is Register, 0 is unRegister
	BYTE nFamily;
	BYTE RelayMethod;
};

/*
 * c2s_gmsvr2gateway_saverole
 */
struct tagGS2GWSaveRole : public tagProtoHeader
{
	size_t	datalength;
	BYTE	szData[0];
};

/*
 * #pragma pack( pop )
 *
 * } End of the struct define
 */

typedef struct
{
	char	szName[32];
	BYTE	Sex;
	BYTE	Series;
	int		Faction;
	int		Level;
} RoleBaseInfo/* client */, S3DBI_RoleBaseInfo /* server */;

typedef struct
{
	BYTE				ProtocolType;
	RoleBaseInfo		m_RoleList[MAX_PLAYER_IN_ACCOUNT];
} ROLE_LIST_SYNC;

//����RoleDBManager/kroledbheader.h
//�����滻�����ROLE_LIST_SYNC,ROLE_LIST_SYNC�ṹ������Ҫ��
struct TProcessData
{
	unsigned char	nProtoId;
	size_t			nDataLen;//TRoleNetMsgʱ��ʾ��Block��ʵ�����ݳ���,TProcessDataʱ��ʾStream��ʵ�����ݳ���
	unsigned long	ulIdentity;
	bool			bLeave;
	char			szAccountName[32];
	char			pDataBuffer[1];//ʵ�ʵ�����
};

struct tagRoleEnterGame
{
	BYTE			ProtocolType;
	bool			bLock;
	char			Name[_NAME_LEN];
};

//�½���ɫ����Ϣ�ṹ
//ע�ͣ��½�����Ϣc2s_newplayer�����͵Ĳ���ΪTProcessData�ṹ���������ݣ�����TProcessData::pDataBufferҪ��չΪNEW_PLAYER_COMMAND
struct NEW_PLAYER_COMMAND
{
	BYTE			m_btRoleNo;			// ��ɫ���
	BYTE			m_btSeries;			// ����ϵ
	unsigned short	m_NativePlaceId;	//������ID
	char			m_szName[32];		// ����
};

typedef struct 
{
	BYTE			ProtocolType;
	BYTE			m_LogoutType;
} LOGOUT_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			szAccName[32];
} LOGIN_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	WORD			m_wLength;
	char			m_szSentence[MAX_SENTENCE_LENGTH];
} TRADE_APPLY_OPEN_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
} TRADE_APPLY_CLOSE_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	BYTE			m_btState;			// if == 0 close if == 1 open if == 2 trading
	DWORD			m_dwNpcID;			// ����ǿ�ʼ���ף��Է��� npc id
	BOOL			m_bFolkGame;
} TRADE_CHANGE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	WORD			m_wLength;
	DWORD			m_dwID;
	BYTE			m_btState;
	char			m_szSentence[MAX_SENTENCE_LENGTH];
} NPC_SET_MENU_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	DWORD			m_dwID;
	BOOL			m_bFolkGame;
} TRADE_APPLY_START_COMMAND;

// ������ת����������
typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_nDestIdx;			// �������ڷ������˵�player idx
	DWORD			m_dwNpcId;			// �����ߵ� npc id
	BOOL			m_bFolkGame;
} TRADE_APPLY_START_SYNC;

// ���ܻ�ܾ����˵Ľ�������
typedef struct
{
	BYTE			ProtocolType;		// Э������
	BYTE			m_bDecision;		// ͬ�� 1 ��ͬ�� 0
	int				m_nDestIdx;			// ���׶Է��ڷ������˵�player idx
	BOOL			m_bFolkGame;
} TRADE_REPLY_START_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_nMoney;
} TRADE_MOVE_MONEY_COMMAND;		// c2s_trademovemoney

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_nMoney;
} TRADE_MONEY_SYNC;				// s2c_trademoneysync

typedef struct
{
	BYTE			ProtocolType;		// Э������
	BYTE			m_btDecision;		// ȷ������ 1  �˳����� 0  ȡ��ȷ�� 4  �������� 2  ȡ������ 3
	BYTE			m_btFolkGame;
} TRADE_DECISION_COMMAND;				// ����ִ�л�ȡ�� c2s_tradedecision

typedef struct
{
	BYTE			ProtocolType;		// Э������
	BYTE			m_btDecision;		// ����ok 1  ����ȡ�� 0  ���� 2  ȡ������ 3
} TRADE_DECISION_SYNC;					// s2c_tradedecision

typedef struct
{
	BYTE			ProtocolType;		
	BYTE			m_byDir;			// ȡǮ�ķ���0�棬1ȡ��
	DWORD			m_dwMoney;			// Ǯ��
} STORE_MONEY_COMMAND;

//rut tien
typedef struct
{
	BYTE			ProtocolType;		
	BYTE			m_byDir;			// ȡǮ�ķ���0�棬1ȡ��
	DWORD			m_dwMoney;			// Ǯ��
} WITHDRAWA_MONEY_COMMAND; // rut tien;
//end code

typedef struct
{
	BYTE			ProtocolType;		// Э������
	DWORD			dwID;
	DWORD			dwTimePacker;
	DWORD			m_dwNpcID;
} TEAM_INVITE_ADD_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	WORD			m_wLength;			// ����
	int				m_nIdx;
	char			m_szName[32];
} TEAM_INVITE_ADD_SYNC;

typedef struct
{
	BYTE			ProtocolType;		//
	int				m_nAuraSkill;
} SKILL_CHANGEAURASKILL_COMMAND;		//�����⻷����

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btResult;
	int				m_nIndex;
	DWORD			dwID;
	DWORD			dwTimePacker;
} TEAM_REPLY_INVITE_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BOOL			m_bSelfAskFolkGame;
	BOOL			m_bDestReplyFolkGame;
	BYTE			m_btSelfLock;
	BYTE			m_btDestLock;
	BYTE			m_btSelfOk;
	BYTE			m_btDestOk;
} TRADE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	DWORD			m_dwSkillID;		// ����
	int				m_nLevel;
	int				m_nTime;			// ʱ��
	BOOL			m_bOverLook;
	KMagicAttrib	m_MagicAttrib[MAX_SKILL_STATE];
} STATE_EFFECT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BOOL	bNegative;
} IGNORE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwTime;
} PING_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwReplyServerTime;
	DWORD			m_dwClientTime;
} PING_CLIENTREPLY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btSitFlag;
} NPC_SIT_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
} NPC_HORSE_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	m_dwID;
	BOOL	m_bRideHorse;
}	NPC_HORSE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				m_dwRegionID;
	int				m_nObjID;
} OBJ_MOUSE_CLICK_SYNC;

typedef struct tagSHOW_MSG_SYNC
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	WORD			m_wMsgID;
	LPVOID			m_lpBuf;
	tagSHOW_MSG_SYNC() {m_lpBuf = NULL;};
	~tagSHOW_MSG_SYNC() {Release();}
	void	Release() {if (m_lpBuf) delete []m_lpBuf; m_lpBuf = NULL;}
} SHOW_MSG_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btFlag;
} PK_APPLY_NORMAL_FLAG_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btFlag;
} PK_NORMAL_FLAG_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwNpcID;
	BOOL			m_bRefuse;
	BOOL			m_bSpar;
} PK_APPLY_ENMITY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	BYTE			m_btState;
	DWORD			m_dwNpcID;
	BOOL			m_bAim;
	BOOL			m_bSpar;
	char			m_szName[32];
} PK_ENMITY_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	BYTE			m_btState;
	DWORD			m_dwNpcID;
	char			m_szName[32];
} PK_EXERCISE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				m_nPKValue;
} PK_VALUE_SYNC;

typedef struct
{
	int		m_nID;
	BYTE	m_btNature;
	BYTE	m_btGenre;			// ��Ʒ������
	int		m_btDetail;			// ��Ʒ�����
	int		m_btParticur;		// ��Ʒ����ϸ���
	BYTE	m_btSeries;			// ��Ʒ������
	BYTE	m_btLevel;			// ��Ʒ�ĵȼ�
	BYTE	m_btLuck;			// MF
	int		m_btMagicLevel[MAX_ITEM_MAGICLEVEL];	// ���ɲ���
	WORD	m_wVersion;			// װ���汾
	DWORD	m_dwRandomSeed;		// �������
} SViewItemInfo;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwNpcID;
	BYTE			m_Avatar;
	SViewItemInfo	m_sInfo[itempart_num];
} VIEW_EQUIP_SYNC;				// s2c_viewequip

typedef struct//�ýṹ����ͳ�Ƶ���ҵĻ�������
{
	char	Name[20];
	int		nValue;
	BYTE	bySort;
}TRoleList;

// ��Ϸͳ�ƽṹ
typedef struct
{
	TRoleList MoneyStat[10];			//��Ǯ��������б�ʮ����ң����ɴﵽ100����
	TRoleList LevelStat[10];			//������������б�ʮ����ң����ɴﵽ100����
	TRoleList KillerStat[10];			//ɱ����������б�
	TRoleList ReputeStat[10];
	TRoleList FuYuanStat[10];
	TRoleList AccumStat1[10];
	TRoleList AccumStat2[10];
	TRoleList HonorStat[10];
	TRoleList TimeStat[10];
	TRoleList TongLvStat[10];
	TRoleList TongMnStat[10];
	TRoleList TongEffStat[10];
	//[���ɺ�][�����]������[0]��û�м������ɵ����
	TRoleList MoneyStatBySect[MAX_FACTION+1][10];	//�����ɽ�Ǯ��������б�
	TRoleList LevelStatBySect[MAX_FACTION+1][10];	//�����ɼ�����������б�

	//[���ɺ�]������[0]��û�м������ɵ����
	int SectPlayerNum[MAX_FACTION+1];				//�������ɵ������
	int SectMoneyMost[MAX_FACTION+1];				//�Ƹ�����ǰһ������и�������ռ������
	int SectLevelMost[MAX_FACTION+1];				//��������ǰһ������и�������ռ������
} TGAME_STAT_DATA;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	bSleep;
	DWORD	NpcID;
} NPC_SLEEP_SYNC;

//////////////
//�������
typedef struct
{
	BYTE		ProtocolType;
	DWORD		dwLadderID;
	TRoleList	StatData[10];
} LADDER_DATA;

typedef struct
{
	BYTE		ProtocolType;
	WORD		wSize;
	int			nCount;
	DWORD		dwLadderID[0];
} LADDER_LIST;

typedef struct
{
	BYTE		ProtocolType;
	DWORD		dwLadderID;
} LADDER_QUERY;


///////////////////
//chat ���

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	DWORD	packageID;
	char	someone[_NAME_LEN];
	BYTE	sentlen;
	char	item[MAX_SENTENCE_LENGTH];
} CHAT_SOMEONECHAT_CMD, CHAT_SOMEONECHAT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	DWORD	packageID;
	BYTE	filter;
	DWORD	channelid;
	BYTE	cost;	//0: �����ƣ�1: 10Ԫ/�䣬2: <10Lv ? ����˵ : MaxMana/2/��, 3: MaxMana/10/��,4: <20Lv ? ����˵ : MaxMana*4/5/��
	BYTE	sentlen;
	char	item[MAX_SENTENCE_LENGTH];
	BYTE	someflag;
} CHAT_CHANNELCHAT_CMD;

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	DWORD	packageID;
	char	someone[_NAME_LEN];
	DWORD	channelid;
	BYTE	sentlen;
	char	item[MAX_SENTENCE_LENGTH];
	BYTE	someflag;
} CHAT_CHANNELCHAT_SYNC;

enum 
{
	codeSucc,
	codeFail,
	codeStore
};

typedef struct
{
	BYTE	ProtocolType;
	DWORD	packageID;
	BYTE	code;
	char	item[MAX_SENTENCE_LENGTH];
	BYTE	someflag;
} CHAT_FEEDBACK;

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	WORD	wChatLength;
} CHAT_EVERYONE;


typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	WORD	wChatLength;
	BYTE	byHasIdentify;
	WORD	wPlayerCount;
} CHAT_GROUPMAN;


typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	DWORD	nameid;
	unsigned long lnID;
	WORD	wChatLength;
} CHAT_SPECMAN;


enum 
{
	tgtcls_team,
	tgtcls_fac,
	tgtcls_tong,
	tgtcls_msgr,
	tgtcls_cr,
	tgtcls_scrn,
	tgtcls_bc
};

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	DWORD	nFromIP;
	DWORD	nFromRelayID;
	DWORD	channelid;
	BYTE	TargetCls;
	DWORD	TargetID;
	WORD	routeDateLength;
} CHAT_RELEGATE;


///////////////////////////////////////
// tong ���

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
} S2C_TONG_HEAD;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
} STONG_PROTOCOL_HEAD;

typedef struct
{
	int		m_nPlayerIdx;
	int		m_nCamp;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_APPLY_CREATE;

typedef struct
{
	int		m_nPlayerIdx;
	DWORD	m_dwNpcID;
} STONG_SERVER_TO_CORE_APPLY_ADD;

typedef struct
{
	int		m_nSelfIdx;
	int		m_nTargetIdx;
	DWORD	m_dwNameID;
} STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION;

typedef struct
{
	int		m_nCamp;
	int		m_nPlayerIdx;
	DWORD	m_dwPlayerNameID;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_CREATE_SUCCESS;

typedef struct
{
	int		m_nSelfIdx;
	int		m_nTargetIdx;
	DWORD	m_dwNameID;
} STONG_SERVER_TO_CORE_REFUSE_ADD;

typedef struct
{
	int		m_nSelfIdx;
	int		m_nInfoID;
	int		m_nParam1;
	int		m_nParam2;
	int		m_nParam3;
	char	m_szName[32];
} STONG_SERVER_TO_CORE_GET_INFO;

typedef struct
{
	int		m_nPlayerIdx;
	DWORD	m_dwPlayerNameID;
	BYTE	m_btCamp;
	DWORD	m_dwMoney;
	BYTE	m_btLevel;
	DWORD	m_dwTotalEff;
	BOOL	m_bRecruit;
	int		m_nMemberNum;
	int		m_nTongParam;
	int 	m_nTongJiyuParam;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	char	m_szMasterName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_ADD_SUCCESS;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_BE_INSTATED;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_BE_KICKED;

typedef struct
{
	int		m_nPlayerIdx;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_BE_CHANGED_AGNAME;

typedef struct
{
	DWORD	m_dwTongNameID;
	int		m_nMoney;
	int		m_nCamp;
} STONG_SERVER_TO_CORE_BE_CHANGED_CAMP;

typedef struct
{
	DWORD	m_dwTongNameID;
	BYTE 	m_btLevel;
} STONG_SERVER_TO_CORE_BE_CHANGED_LEVEL;

typedef struct
{
	DWORD	m_dwTongNameID;
	int  	m_nMoney;
} STONG_SERVER_TO_CORE_BE_CHANGED_MONEY;

typedef struct
{
	DWORD	m_dwTongNameID;
	int  	m_nEff;
} STONG_SERVER_TO_CORE_BE_CHANGED_TONG_EFF;

typedef struct
{
	DWORD	m_dwTongNameID;
	BOOL 	m_bRecruit;
} STONG_SERVER_TO_CORE_BE_CHANGED_RECRUIT;

typedef struct
{
	DWORD	m_dwTongNameID;
	int 	m_nTongParam;
} STONG_SERVER_TO_CORE_BE_CHANGED_TONGPARAM;

typedef struct
{
	DWORD	m_dwTongNameID;
	int  	m_nMoney;
	int 	m_nTongJiyuParam;
	char	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
} STONG_SERVER_TO_CORE_BE_CHANGED_JIYU;

typedef struct
{
	int		m_nPlayerIdx;
	int 	m_nTotalEff;
	DWORD	m_dwTotalEff;
	DWORD	m_dwTongNameID;
} STONG_SERVER_TO_CORE_BE_CHANGED_EFF;

typedef struct
{
	int		m_nPlayerIdx;
	BOOL	m_bSuccessFlag;
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_LEAVE;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	DWORD	m_dwTongNameID;
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	DWORD	m_dwTongNameID;
	char	m_szName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_CHECK_GET_AGNAME_POWER;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	DWORD	m_dwTongNameID;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_CHANGE_AS;

typedef struct
{
	DWORD	m_dwTongNameID;
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_CHANGE_MASTER;

typedef struct
{
	DWORD	m_dwTongNameID;
	DWORD	m_dwMoney;
	DWORD	m_nMoney;
	BYTE	nType;
	int		m_nPlayerIdx;
	int		m_nSelect;
	int 	m_nNumMember;
} STONG_SERVER_TO_CORE_MONEY;

typedef struct
{
	DWORD	m_dwTongNameID;
	int 	m_nValue;
	int 	m_nValueSync;
	BYTE	nType;
	int		m_nPlayerIdx;
	int		m_nSelect;
} STONG_SERVER_TO_CORE_EFF;

typedef struct
{
	DWORD	m_dwParam;
	int		m_nFlag;
	int		m_nCamp;
	BYTE	m_btLevel;
	int		m_nFigure;
	DWORD	m_dwMemberNum;
	BYTE	m_btManagerNum;
	BYTE	m_btDirectorNum;
	int		m_nPos;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szMaster[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
	int		m_nJoinTm;
	DWORD	m_nMoney;
	DWORD	m_dwTotalEff;
	int 	m_nSaveEff;
	BOOL 	m_bRecruit;
	int		m_nTongParam;
	int		m_nTongJiyuParam;
	char 	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
} STONG_SERVER_TO_CORE_LOGIN;

// ������뽨����� ����չЭ��
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btCamp;
	BYTE 	m_btSex;
	BYTE 	m_btLevel;
	char	m_szName[defTONG_NAME_LENGTH_32 + 1];
} TONG_APPLY_CREATE_COMMAND;

// ������������ ����չЭ��
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwNpcID;
} TONG_APPLY_ADD_COMMAND;

// ������������ ����չЭ��
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btCurFigure;
	BYTE	m_btCurPos;
	BYTE	m_btNewFigure;
	BYTE	m_btNewPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_INSTATE_COMMAND;

// ��Ὠ��ʧ�� ��չЭ��
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btFailId;
} TONG_CREATE_FAIL_SYNC;

// ת������������ ��չЭ��
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int		m_nPlayerIdx;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_ADD_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	char	m_cTongName[defTONG_NAME_LENGTH_32];
} JOIN_TONG_SYNC;

// ֪ͨ��ҽ������ɹ� ����ͨЭ��
typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btCamp;
	char	m_szName[defTONG_NAME_LENGTH_32 + 1];
} TONG_CREATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	DWORD			m_dwID;
} TONG_JOIN_REPLY;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	char			m_cTongName[defTONG_NAME_LENGTH_32];
} S2C_JOIN_TONG;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_Index;
	DWORD			m_dwID;
} S2C_TONG_JOIN_REPLY;

// ֪ͨ��Ҽ����� ����չЭ��
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btCamp;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szMaster[defTONG_NAME_LENGTH_32];
} TONG_ADD_SYNC;

// ��������ɢ��� ����չЭ��
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
} TONG_APPLY_DISMISS_COMMAND;

// ��Ұ�ᱻ��ɢ ����ͨЭ��
typedef struct
{
	BYTE	ProtocolType;
} TONG_DISMISS_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int		m_nPlayerIdx;
	DWORD	m_dwNameID;
	BYTE	m_btFlag;			// �Ƿ���� TRUE ���� FALSE ������
} TONG_ACCEPT_MEMBER_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btInfoID;
	int		m_nParam1;
	int		m_nParam2;
	int		m_nParam3;
	char	m_szBuf[64];
} TONG_APPLY_INFO_COMMAND;

typedef struct
{
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
	int 	m_nMasterJoinTm;
	int 	m_nDirectorJoinTm[defTONG_MAX_DIRECTOR];
	int 	m_nManagerJoinTm[defTONG_MAX_MANAGER];
	BOOL	m_bOnline;
	int		m_nMasterEff;
	int		m_nDirectorEff[defTONG_MAX_DIRECTOR];
	int 	m_nManagerEff[defTONG_MAX_MANAGER];
} TONG_ONE_LEADER_INFO;

typedef struct
{
	char	m_szName[defTONG_NAME_LENGTH_32];
	int 	m_nMemberJoinTm;
	BOOL	m_bOnline;
	BYTE	m_btSex;
	int 	m_nMemberEff;
} TONG_ONE_MEMBER_INFO;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwNpcID;
	DWORD	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	DWORD	m_dwTotalEff;
	int 	m_nSaveEff;
	BOOL	m_bRecruit;
	int 	m_nTongParam;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	int		m_nTongJiyuParam;
	char 	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	TONG_ONE_LEADER_INFO	m_sMember[1 + defTONG_MAX_DIRECTOR];
} TONG_HEAD_INFO_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	BYTE	m_btStateNo;
	BYTE	m_btCurNum;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	TONG_ONE_LEADER_INFO	m_sMember[defTONG_ONE_PAGE_MAX_NUM];
} TONG_MANAGER_INFO_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	BYTE	m_btStateNo;
	BYTE	m_btCurNum;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	char	m_szMaleAgname[defTONG_NAME_LENGTH_32];
	char	m_szFemaleAgname[defTONG_NAME_LENGTH_32];
	TONG_ONE_MEMBER_INFO	m_sMember[defTONG_ONE_PAGE_MAX_NUM];
} TONG_MEMBER_INFO_SYNC;

// ��������ڰ���е���Ϣ ����չЭ��
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btJoinFlag;
	BYTE	m_btFigure;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szMasterName[defTONG_NAME_LENGTH_32];
	int		m_nTongJiyuParam;
	char 	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
	DWORD	m_dwMoney;
	DWORD	m_dwTotalEff;
	BOOL	m_bRecruit;
	int		m_nTongParam;
} TONG_SELF_INFO_SYNC;


typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btSuccessFlag;
	BYTE	m_btOldFigure;
	BYTE	m_btOldPos;
	BYTE	m_btNewFigure;
	BYTE	m_btNewPos;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_INSTATE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_KICK_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btSuccessFlag;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_KICK_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_LEAVE_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_CHANGE_MASTER_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} TONG_APPLY_CHANGE_AGNAME_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	int		m_btSex;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} TONG_APPLY_CHANGE_SEX_AGNAME_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btCamp;
	int 	m_nMoney;
} TONG_APPLY_CHANGE_CAMP_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int 	m_nExtPoint; //TamLTM fix xu;
} APPLY_GET_EXTPOINT_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int 	m_nParam;
} APPLY_GET_PARAM_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_CHANGE_MASTER_FAIL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_CHANGE_AGNAME_FAIL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_CHANGE_CAMP_FAIL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btCamp;
} TONG_CHANGE_CAMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btLevel;
} TONG_CHANGE_LEVEL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_bRecruit;
} TONG_CHANGE_RECRUIT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int 	m_nTongParam;
} TONG_CHANGE_TONGPARAM_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_bRecruit;
} TONG_CHANGE_RECRUIT_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	int		m_nTongParam;
} TONG_CHANGE_TONGPARAM_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int		m_nTongJiyuParam;
	char 	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
} TONG_CHANGE_JIYU_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int 	m_nSaveEff;
	int 	m_dwTotalEff;
} TONG_CHANGE_EFF_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	char	m_szName[defTONG_NAME_LENGTH_32];
	int		m_nMoney;
	int		m_nTongJiyuParam;
	char 	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
} TONG_APPLY_CHANGE_INFO_COMMAND;

// tong ��� end

typedef struct
{
	BYTE	ProtocolType;
	DWORD	dwItemID;
	int		nChange;
} ITEM_DURABILITY_CHANGE;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		Rank;
}	TITLE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	KExpandRank		ExpandRank;
}	EXPANDTITLE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	dwItemID;
	BYTE	m_Number;
} ITEM_REPAIR;

typedef struct
{
	BYTE	ProtocolType;
	WORD	nTaskId;
	char	szTaskValue[16];
}S2C_SYNCTASKVALUE;

typedef struct /*tagS2C_PLAYER_SYNC*/
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	WORD			m_wMsgID;
	LPVOID			m_lpBuf;
/*	tagS2C_PLAYER_SYNC() {m_lpBuf = NULL;};
	~tagS2C_PLAYER_SYNC() {Release();}
	void	Release() {/*if (m_lpBuf) delete []m_lpBuf; m_lpBuf = NULL;}*/
} S2C_PLAYER_SYNC;

typedef struct /*tagPLAYER_COMMAND*/
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	WORD			m_wMsgID;
	LPVOID			m_lpBuf;
/*	tagPLAYER_COMMAND() {m_lpBuf = NULL;};
	~tagPLAYER_COMMAND() {Release();}
	void	Release() {/if (m_lpBuf) delete []m_lpBuf; m_lpBuf = NULL;}*/
} PLAYER_COMMAND;

typedef struct /*tagPLAYER_LOCK_ITEM //Lock dinh item */
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	WORD			m_wMsgID;
	LPVOID			m_lpBufLockItem;
/*	tagPLAYER_LOCK_ITEM() { m_lpBufLockItem = NULL; };
	~tagPLAYER_LOCK_ITEM() { Release(); }
	void	Release() { if (m_lpBufLockItem) delete[]m_lpBufLockItem; m_lpBufLockItem = NULL; }*/

} PLAYER_LOCK_ITEM;

typedef struct /*tagPLAYER_UNLOCK_ITEM*/
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	WORD			m_wMsgID;
	LPVOID			m_lpBufUnLockItem;
/*	tagPLAYER_UNLOCK_ITEM() {m_lpBufUnLockItem = NULL;};
	~tagPLAYER_UNLOCK_ITEM() {Release();}
	void	Release() { if (m_lpBufUnLockItem) delete []m_lpBufUnLockItem; m_lpBufUnLockItem = NULL; }*/
} PLAYER_UNLOCK_ITEM;

typedef struct
{
	BYTE	ProtocolType;
	char	szBuf[32];
} C2S_BUF_COMMAND;

typedef struct // protocol xu
{
	BYTE			ProtocolType;
	int				m_nExtPointValue;
	int 			m_nChangeExtPoint;
} EXTPOINT_VALUE_SYNC;

typedef struct
{	
	BYTE	ProtocolType;
	char	m_szName[FILE_NAME_LENGTH];
	char	m_szInitString[MAX_MESSAGE_LENGTH];
}PLAYER_GIVE;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	dwItemID;
	int		nNum;
	BOOL	bIsBreakAll;
}PLAYER_BREAK_COMMAND;

typedef struct
{
	int		m_nID;				// ��Ʒ��ID
	BYTE	m_btNature;
	BYTE	m_btGenre;			// ��Ʒ������
	int		m_btDetail;			// ��Ʒ�����
	int		m_btParticur;		// ��Ʒ����ϸ���
	BYTE	m_btSeries;			// ��Ʒ������
	BYTE	m_btLevel;			// ��Ʒ�ĵȼ�
	BYTE	m_btLuck;			// MF
	int		m_btMagicLevel[MAX_ITEM_MAGICLEVEL];	// ���ɲ���
	WORD	m_wVersion;			// װ���汾
	DWORD	m_dwRandomSeed;		// �������
	int		m_nIdx;
	UINT	m_uPrice;
	int		m_bX;
	int		m_bY;
	unsigned short 	m_nStackNum;
	int		m_ExpireTime;
	int		m_nParam;
	int		m_nFortune;
} SViewSellItemInfo;

typedef struct
{
	BYTE			ProtocolType;
	SViewSellItemInfo	m_sInfo;
} PLAYER_TRADE_ITEM_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	LockMoveItem	m_LockMove;
}PLAYER_LOCKMOVE;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	DWORD			dwID;
	int				nParam;
}PLAYER_TRADE;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	DWORD			dwID;
	BOOL			bOpen;
}PLAYER_TRADE_ACTION_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_nIdx;			// ��ڼ�������
	DWORD			m_dwID;
	BYTE			m_Place;			// ���������ĸ��ط�
	BYTE			m_X;				// ����X
	BYTE			m_Y;				// ����Y
} PLAYER_TRADE_BUY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	DWORD			m_dwID;				// ��Ʒ��ID
	int				m_nPrice;
}PLAYER_TRADE_SET_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	char			Name[32];
}PLAYER_TRADE_START_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
}PLAYER_TRADE_START_FAIL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	char	m_szName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} TONG_CHANGE_AGNAME_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	int		m_nEff;
	int 	m_nSelect;
	char	m_szName[defTONG_NAME_LENGTH_32];
	BYTE	m_btFigure;
} TONG_APPLY_SAVE_EFF_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	DWORD	m_dwMoney;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_SAVE_COMMAND;

typedef struct
{
	DWORD	m_dwTongNameID;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
	int		m_nPlayerIdx;
} STONG_SERVER_TO_CORE_AGNAME;

typedef struct
{
	BYTE	ProtocolType;
	char	MissionName[64];
	TMissionLadderSelfInfo SelfData;
	TMissionLadderInfo MissionRank[MISSION_STATNUM];
} PLAYER_MISSION_RANKDATA;

typedef struct
{
	BYTE	ProtocolType;
	int			m_nSaleType;
	BuySellInfo	m_BuySellInfo;
} S2C_SUPERSHOP;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	DWORD			m_dwID;
} SPAR_APPLY_START_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_nDestIdx;			// �������ڷ������˵�player idx
	DWORD			m_dwNpcId;			// �����ߵ� npc id
} SPAR_APPLY_START_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// Э������
	int				m_nDecision;
	BOOL			m_bRoomList;
} PLAYER_CHATROOM_DECISION_COMMAND;

typedef struct
{
	BYTE	ProtocolType;		// Э������
	BYTE	m_btErrorID;		// ���鴴�����ɹ�ԭ��0 ͬ�� 1 ��ұ����Ѿ�����ĳһ֧���� 3 ��ǰ���ڲ������״̬
} PLAYER_SEND_CREATE_CHATROOM_FALSE;// ������֪ͨ�ͻ��˶��鴴�����ɹ�

struct PLAYER_SEND_CHATROOM_STATE
{
	BYTE	ProtocolType;		// Э������
	BYTE	m_btMsgId;
	int		nId;
	char	lpszName[16];
	PLAYER_SEND_CHATROOM_STATE() {lpszName[0]=0;}
};	// ������֪ͨ��Ҷ��鴴���ɹ�

struct PLAYER_REQUEST_CHATROOM_LIST
{
	BYTE	ProtocolType;		// Э������
	char	lpszRoomHost[16];
	char	lpszRoomName[10];
	BYTE	byRoomMemNum;
	int		nRoomLeftTime;
	int		nRoomParam;
	PLAYER_REQUEST_CHATROOM_LIST() {nRoomParam=0;}
};	// ������֪ͨ��Ҷ��鴴���ɹ�

struct PLAYER_REQUEST_LIST
{
	BYTE	ProtocolType;		// Э������
	char	lpszName[_NAME_LEN];
	int		nMemIndex;
	int		nMemParam;
	BOOL	bIsRoomHost;
	PLAYER_REQUEST_LIST() {nMemParam=0;}
};	// ������֪ͨ��Ҷ��鴴���ɹ�

typedef struct	
{
	BYTE	ProtocolType;
	int		int_ID;
} CP_DATAU; // give da tau

//TamLTM da tau
typedef struct
{
	BYTE			ProtocolType;
	int				sScript;
} PLAYER_REQUEST_LOAD_DATAU;

// da tau nhiem vu
typedef struct
{
	BYTE	ProtocolType;
	int		nIdQuestIndex;
} FINISH_QUEST_SYNC;

//end code

// TamLTM kham nam
typedef struct
{
	BYTE	ProtocolType;
	char	szString[80];
} GET_STRING;

typedef struct
{
	BYTE			ProtocolType;
	int				nType;
	char			szFunc[32];
} PLAYER_UI_CMD_SCRIPT; // protocol load script

typedef struct
{
	BYTE			ProtocolType;
	DWORD			dwID;
	int				nX;
	int				nY;
} RECOVERY_BOX_CMD;

typedef struct
{ 
	BYTE ProtocolType;
	BYTE nValue;
} S2C_OTHER_BOX; // XANH

typedef struct
{
	BYTE	ProtocolType;
	BYTE	nType;
	int		nNum[2];
	char	szStr[64];
	char	szFunc[32];
} C2S_PLAYER_INPUT_INFO;
//end code

//TamLTM Open progress bar
typedef struct
{
	BYTE			ProtocolType;
	int				sScript;
} PLAYER_REQUEST_LOAD_PROGRESS_BAR;

typedef struct
{
	BYTE	ProtocolType;
	int		nIdQuestIndex;
} OPEN_PROGRESS_BAR_SYNC;

//end code

//TamLTM Uy thac offline
typedef struct
{
	BYTE			ProtocolType;
} PLAYER_REQUEST_OFFLINE;
//end code

// �ڵ�����֧����֮ǰ�����ж��Ƿ��ڽ���״̬��������ڽ��ף����ܵ�����֧����
void SendClientCmdSell(int nID, int nNumber);

// �ڵ�����֧����֮ǰ�����ж��Ƿ��ڽ���״̬��������ڽ��ף����ܵ�����֧����
void SendClientCmdBuy(int nShop, int nBuyIdx, BYTE nNumber); // TamLTM Add bang hoi chiem linh , int nX, int nY

// �ڵ�����֧����֮ǰ�����ж��Ƿ��ڽ���״̬��������ڽ��ף����ܵ�����֧����
void SendClientCmdRun(int nX, int nY);

// �ڵ�����֧����֮ǰ�����ж��Ƿ��ڽ���״̬��������ڽ��ף����ܵ�����֧����
void SendClientCmdWalk(int nX, int nY);

// �ڵ�����֧����֮ǰ�����ж��Ƿ��ڽ���״̬��������ڽ��ף����ܵ�����֧����
void SendClientCmdSkill(int nSkillID, int nX, int nY,int nX0 = -1, int nY0 = -1,int nRadius = -1);
void SendClientCmdSit(bool bFlag);
void SendClientCmdMoveItem(void* pDownPos, void* pUpPos);
void SendClientCmdQueryLadder(DWORD	dwLadderID);
void SendClientCmdRequestNpc(int nID);
void SendClientCmdStoreMoney(int nDir, int nMoney);
void SendClientCmdWithDrawaMoney(int nDir, int nMoney); // rut tien;
void SendClientCmdRevive();
void SendObjMouseClick(int nObjID, DWORD dwRegionID);
void SendClientCmdRepair(DWORD dwID);
void SendClientCmdRide();
void SendClientCmdBreak(DWORD dwID, int nNum, BOOL bIsBreakAll);
void SendClientCPSetImageCmd(int ID);
void SendClientDaTauCmd(int szScript); //TamLTM da tau
void SendClientCPActionCheatCmd(char* szFunc); // Ma doc
//TamLTM kham nam xanh
void SendUiCmdScript(int nName,char*szFunc);
void SendClientRecoveryBox(DWORD dwID, int nX, int nY);
void SendClientCmdInputBox(BYTE nType,int* nNum,char* szStr,char*szFunc);
//end code
void SendClientOpenProgressBarCmd(int szScript); //TamLTM open progress bar
void SendClientOffline(); //TamLTM Uy thac offline

extern	int	g_nProtocolSize[MAX_PROTOCOL_NUM];
#pragma pack(pop, enter_protocol)
#endif
