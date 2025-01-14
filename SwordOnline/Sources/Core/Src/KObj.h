//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KObj.h
// Date:	2002.01.06
// Code:	�߳�����
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KObj_H
#define KObj_H

#ifdef	_SERVER
#define	MAX_OBJECT	20000
#else
#define	MAX_OBJECT	256
#endif

#include "KPolygon.h"

#ifndef _SERVER
#include "KSprite.h"
#include "KWavSound.h"
#include "KSprControl.h"
#endif

#include "KIniFile.h"
#include "GameDataDef.h"
#ifndef	_SERVER
#include "../../Represent/iRepresent/KRepresentUnit.h"
#endif
#define		OBJ_BAR_WIDTH		3								// ����ϰ� ��
#define		OBJ_BAR_HEIGHT		5								// ����ϰ� ��
#define		OBJ_BAR_SIZE		(OBJ_BAR_WIDTH*OBJ_BAR_HEIGHT)	// ����ϰ� ��x��

#define		OBJ_BOX_STATE_CLOSE		0		// ����״̬ ��
#define		OBJ_BOX_STATE_OPEN		1		// ����״̬ ��
#define		OBJ_DOOR_STATE_CLOSE	0		// ��״̬ ��
#define		OBJ_DOOR_STATE_OPEN		1		// ��״̬ ��
#define		OBJ_PROP_STATE_DISPLAY	0		// ����״̬ �� ����ʾ
#define		OBJ_PROP_STATE_HIDE		1		// ����״̬ �� ��ʾ

#define		OBJ_BELONG_TIME		600

enum	Obj_Bar_Type			// �����������ϰ�����
{
	Obj_Bar_Empty = 0,			// �գ����ϰ�
	Obj_Bar_Full,				// �����ϰ�
	Obj_Bar_LT,					// ��������Ϊ�ϰ�
	Obj_Bar_RT,					// ��������Ϊ�ϰ�
	Obj_Bar_LB,					// ��������Ϊ�ϰ�
	Obj_Bar_RB,					// ��������Ϊ�ϰ�
	Obj_Bar_Type_Num,			// �ϰ�������
};

typedef struct SOBJ_LIGHT_DATA
{
	int			m_nRadius;			// ����Ĺ�����
	int			m_nRed;				// ��ƫɫ
	int			m_nGreen;			// ��ƫɫ
	int			m_nBlue;			// ��ƫɫ
	int			m_nAlpha;			// alpha ֵ
	int			m_nReflectType;		// �ܹⷽʽ�����ܹ⡢���ġ�ˮƽ����б����б��
	SOBJ_LIGHT_DATA() {Release();};
	void	Release() {	m_nRadius = 0; m_nRed = 0; m_nGreen = 0; m_nBlue = 0; m_nAlpha = 0; m_nReflectType = 0; };
} OBJ_LIGHT;		// ����������

//---------------------------------------------------------------------------

#ifdef _SERVER
class ServerImage
{
public:
	int			m_nTotalFrame;						// ��֡��
	int			m_nCurFrame;						// ��ǰ֡
	int			m_nTotalDir;						// �ܷ�����
	int			m_nCurDir;							// ��ǰ����
	DWORD		m_dwTimer;							// ʱ�������
	DWORD		m_dwInterval;						// ֡����������ٶȣ�

protected:
	int			m_nDirFrames;						// ÿ����֡��

public:
	ServerImage();									// ���캯��
	void		Release();							// ���
	void		SetTotalFrame(int nTotalFrame);		// �趨��֡��
	void		SetCurFrame(int nCurFrame);			// �趨��ǰ֡
	void		SetTotalDir(int nTotalDir);			// �趨�ܷ�����
	BOOL		SetCurDir(int nDir);				// �趨��ǰ����
	void		SetInterVal(DWORD dwInterval);		// �趨֡���
	int			GetOneDirFrames();					// ��õ�����֡��

	BOOL		GetNextFrame(BOOL bLoop = TRUE);	// �Զ����֡
	BOOL		GetPrevFrame(BOOL bLoop = TRUE);	// �Զ���ǰ��֡
	BOOL		CheckEnd();							// �ж϶����Ƿ񲥷ŵ����
	void		SetDirStart();						// �趨��ǰ����ĵ�ǰ֡Ϊ��һ֡
	void		SetDirEnd();						// �趨��ǰ����ĵ�ǰ֡Ϊ���һ֡
	BOOL		SetCurDir64(int nDir);				// �趨��ǰ����(�������64����ת���������ķ���)
	int			GetCurDirFrameNo();
};
#endif
#ifdef TOOLVERSION
class CORE_API KObj
#else
class KObj
#endif
{
	friend class KObjSet;
public:
	int			m_nID;					// ���������ΨһID���ͻ������������һһ��Ӧ�����ֵΪ 0 ����Ϊ�ͻ��� obj ����������û�ж�Ӧ�� obj
	int			m_nDataID;				// �������������ݱ��е�λ��(������ģ���еı��)
	int			m_nKind;				// ����	��Ӧ Obj_Kind_XXXX
	int			m_nIndex;				// �� Object �����е�λ��
	int			m_nBelongRegion;		// �����������ĸ�region���������꣬������Ա䶯������������ʱ�Ǹ�region���ͻ��˴洢���� region id

	int			m_nSubWorldID;			// ������ID
	int			m_nRegionIdx;			// ����idx
	int			m_nMapX;				// ��ͼ�������� x
	int			m_nMapY;				// ��ͼ�������� y
	int			m_nOffX;				// ��ͼ�����е�ƫ�� x (ע����ֵΪ���ص�ֵ * 1024)
	int			m_nOffY;				// ��ͼ�����е�ƫ�� y (ע����ֵΪ���ص�ֵ * 1024)
	int			m_nDir;					// ���򣬴˷���Ϊ������߼����򣬰�64������㣬���Ǿ����ͼ����

	int			m_nState;				// ״̬
	int			m_nLifeTime;			// �������ʧʱ�䣨��Ҫ����ʬ���ࡢװ���ࡢ������ȣ�
	int			m_nBornTime;			// ��������ʱ��box��״̬����lifetime�Զ��رգ�prop����״̬����lifetime�Զ���ʾ��
	int			m_nObjValue;

	char		m_szName[80];			// �������
//	char		m_szScriptName[80];		// �����ű��ļ���
	DWORD		m_dwScriptID;			// �����ű��ļ�ID
	int			m_nColorID;				// ���������ʾʱ����ɫid���ͻ���ͨ������ҵ���Ӧ��ɫ

	int			m_nItemDataID;			// ������Ʒ��ָ����Ʒ�ľ�������
	int			m_nItemWidth;			// ��Ʒ���������������е�ʱ��
	int			m_nItemHeight;			// ��Ʒ�������������е�ʱ��
	int			m_nMoneyNum;			// ����ǵ��ڵ��ϵ�Ǯ��Ǯ������
	int 		m_nGenre;
	int 		m_nDetailType;
	BOOL		m_bOverLook;

	//TamLTM Bang hoi chiem linh
	BOOL        m_bByPlayer;

#ifdef _SERVER
	BOOL		m_bPickExecute;
	// ����Ǵ�����������װ����Ǯ��˭�������(��¼����player index) if == -1 ���������κ��ˣ��κ��˶��ܼ�
	int			m_nBelong;
	// m_nBelong �����õ�ʱ��(�Ǹ�����ʱ)
	int			m_nBelongTime;

	ServerImage m_cImage;				// ��������ͼ�����
#endif

#ifndef _SERVER
	char		m_szImageName[80];		// ͼ���ļ���
	char		m_szSoundName[80];		// �����ļ���
	KSprControl	m_cImage;				// ���ͼ��

	char		m_szImageDropName[80];	// �����item���ͣ���������ʱ�򲥷ŵĶ����ļ���
	KSprControl	m_cImageDrop;			// �����item���ͣ���������ʱ�򲥷ŵĶ�����ͼ�����
	int			m_nDropState;			// �����item���ͣ���ǰ�ǵ�������״̬(1)���Ƿ����ڵذ���״̬(0)
	KRUImage	m_Image;
	unsigned int	m_SceneID;

	int			m_nLayer;				// ���λ�ڵڼ���
	int			m_nHeight;				// ������߼��߶�
	OBJ_LIGHT	m_sObjLight;			// ��������ȡ�������ص�����
	DWORD		m_dwNameColor;			// ������ֵ���ɫ
#endif

	KPolygon	Polygon;				// ��������
	BYTE		m_btBar[OBJ_BAR_SIZE];	// ��������ϰ���Ϣ

private:
	DWORD		m_dwTimer;				// ʱ�������(������������)

#ifndef _SERVER
	KCacheNode*	m_pSoundNode;			// ��Чָ��
	KWavSound*	m_pWave;				// ��Чwavָ��

	BOOL		m_bDrawFlag;			// �����Ƿ����⴦��FALSE ����   TRUE �����ƣ�
#endif

public:
	KObj();
	void		GetMpsPos(int *pX, int *pY);
	void		Release();
	void		Activate();
	void		ExecScript(int nPlayerIdx);
	void		SetIndex(int nIndex);
	void		SetWorldID(int nID);			// ���������Ψһ ID
	void		SetLifeTime(int nLifeTime);
	void		SetObjValue(int nValue) {m_nObjValue = nValue;};
	int			GetObjValue() {return m_nObjValue;};
	BOOL		SetDir(int n64Dir);
	void		SetState(int nState, int nPlaySoundFlag = 0);
	void		SetImageDir(int nDir);
	void		Remove(BOOL bSoundFlag);
	void		SetScriptFile(char *lpszScriptFile);
	int			GetKind();

#ifdef _SERVER
	void		SetObjPickExecute(BOOL bFlag) {m_bPickExecute = (BOOL)bFlag;};
	BOOL		GetObjPickExecute() {return m_bPickExecute;};
	void		SetItemDataID(int nItemDataID);
	BOOL		SyncAdd(int nClient);
	void		SyncState();
	void		SyncDir();
	void		SyncRemove(BOOL bSoundFlag);
	int			GetItemDataID();
	void		SetItemBelong(int nPlayerIdx);
	void		SetEntireBelong(int nPlayerIdx);
#endif

#ifndef _SERVER
	void		Draw();
	void		DrawInfo(int nFontSize = 12);
	void		DrawBorder();
#endif

	// ͹�����ת��Ϊ�ϰ���Ϣ
	void		PolygonChangeToBar(
					KPolygon Polygon,		// ͹�����
					int nGridWidth,			// ���ӳ�
					int nGridHeight,		// ���ӿ�
					int nTableWidth,		// ���
					int nTableHeight,		// ����
					BYTE *lpbBarTable);		// �������
	int			GetDistanceSquare(int nNpcIndex);
private:
	void		BoxOpen();
	void		BoxClose();
	void		DoorOpen();
	void		DoorClose();
#ifndef _SERVER
	void		PlayLoopSound();
	void		PlayRandSound();
	int			GetSoundPan();
	int			GetSoundVolume();
	void		PlaySound();
#endif
};

extern	KObj	Object[MAX_OBJECT];
//---------------------------------------------------------------------------
#endif