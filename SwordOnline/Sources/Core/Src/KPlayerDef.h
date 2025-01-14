#ifndef KPLAYERDEF_H
#define KPLAYERDEF_H

//���ݿ�����б�����ʱÿ֡��������
#define DBLOADPERTIME_ITEM		10
#define DBLOADPERTIME_FRIEND	100
#define	DBLOADPERTIME_TASK		500



#define SCRIPT_PLAYERID				"PlayerId"		//�ű��е����Idȫ�ֱ�����
#define SCRIPT_PLAYERINDEX			"PlayerIndex"	//�ű��е���ҵı��ȫ�ֱ�����
#define SCRIPT_OBJINDEX				"ObjIndex"		//�����ű��������INDEX
#define SCRIPT_SUBWORLDINDEX		"SubWorld"		//�ű��е�ǰ�����Index��ȫ�ֱ�����

#define	CLIENT_PLAYER_INDEX	1

#define		MAX_TEAM_APPLY_LIST		32		// ÿ����������ǰ�����������

//---------------------------------------------------------------------------
#define	QUEST_BEFORE	0x0000
#define	QUEST_START		0x0001
#define	QUEST_FINISHED	0xffff
//---------------------------------------------------------------------------
#ifndef _SERVER
enum MOUSE_BUTTON
{
	button_left  = 0,
	button_right,
};
#endif

enum SCRIPTACTION  
{
	SCRIPTACTION_UISHOW,			//��ʾĳ��UI����
	SCRIPTACTION_EXESCRIPT,			//ִ�нű�
};
/*
enum UISEL_DATATYPE
{
	UISEL_nInfo_sOption,		//����Ϣ�����ֱ�ʶ��ѡ��Ϊ�ַ���
	UISEL_nInfo_sOption,		//����Ϣ�����ֱ�ʶ��ѡ��Ϊ���ֱ�ʶ	
	
	UISEL_sInfo_nOption,		
	UISEL_sInfo_sOption,
};
*/
enum
{
	ATTRIBUTE_STRENGTH = 0,
	ATTRIBUTE_DEXTERITY,
	ATTRIBUTE_VITALITY,
	ATTRIBUTE_ENGERGY,
	ATTRIBUTE_RESET_ALL,
};

enum	// ���鴴��ʧ��ԭ��
{
	Team_Create_Error_InTeam = 0,	// �Ѿ��ڶ�����
	Team_Create_Error_SameName,		// ͬ������
	Team_Create_Error_Name,			// �������ַǷ�
	Team_Create_Error_TeamFull,		// ��������̫�࣬���ɲ���
	Team_Create_Error_CannotCreate,	// ��ǰ���ڲ������״̬
	Team_Create_Error_NormalPK,	// ��ǰ���ڲ������״̬
};

enum	// ���鴴��ʧ��ԭ��
{
	ChatRoom_Create_Error_Name,
	ChatRoom_Create_Error_Pass,
	ChatRoom_Create_Error_SameName,		// ͬ������
	ChatRoom_Create_Error_SameHost,		// ��������̫�࣬���ɲ���
	ChatRoom_Create_Error_RoomFull,		// ��������̫�࣬���ɲ���
};

enum	// ���鴴��ʧ��ԭ��
{
	ChatRoom_Create_Success,
	ChatRoom_Join_Host,		// ͬ������
	ChatRoom_Join_Member,		// ͬ������
	ChatRoom_Join_Fail1,
	ChatRoom_Join_Fail2,
	ChatRoom_Join_Fail3,
	ChatRoom_Leave,		// ͬ������
	ChatRoom_Delete,
	ChatRoom_ForceLeave,
	ChatRoom_BeKick,		// ��������̫�࣬���ɲ���
	ChatRoom_AddBlackList,
	ChatRoom_AddBlackList_Fail,
	ChatRoom_RemoveBlackList,
	ChatRoom_ChangePw,
};

enum SYNC_STEP
{
	STEP_BASE_INFO = 0,
	STEP_FIGHT_SKILL_LIST,
	STEP_STATE_SKILL_LIST,
	STEP_TASK_LIST,
	STEP_ITEM_LIST,
	STEP_SYNC_END
};

enum SAVE_STATUS
{
	SAVE_IDLE = 0,
	SAVE_REQUEST,
	SAVE_DOING,
};

/*enum LIXIAN_STATUS
{
	LIXIAN_IDLE = 0,
	LIXIAN_REQUEST,
	LIXIAN_DOING,
};*/
#endif
