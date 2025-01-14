#ifndef	KWorldH
#define	KWorldH

#ifdef _SERVER
#define	MAX_SUBWORLD	100
#else
#define	MAX_SUBWORLD	1
#endif
#define	VOID_REGION		-2
//-------------------------------------------------------------
#include "KEngine.h"
#include "KRegion.h"
#include "KWeatherMgr.h"

#ifdef _SERVER
#include "KMission.h"
#include "KMissionArray.h"
#define	MAX_MISSIONARRAY 2
typedef KMissionArray <KMission , MAX_MISSIONARRAY> KSubWorldMissionArray;
#define MAX_GLOBAL_MISSIONCOUNT 10
typedef KMissionArray <KMission , MAX_GLOBAL_MISSIONCOUNT> KGlobalMissionArray;
extern KGlobalMissionArray g_GlobalMissionArray;
#endif

//-------------------------------------------------------------


#ifndef TOOLVERSION
class KSubWorld
#else

class CORE_API KSubWorld
#endif
{
public:	
	int			m_nIndex;
	int			m_SubWorldID;
#ifdef _SERVER
	KSubWorldMissionArray m_MissionArray;
#endif
	KRegion*	m_Region;
#ifndef _SERVER
	int			m_ClientRegionIdx[MAX_REGION];
	char		m_szMapPath[FILE_NAME_LENGTH];
	//KLittleMap	m_cLittleMap;
#endif
	int			m_nWorldRegionWidth;			//	SubWorld�����Region
	int			m_nWorldRegionHeight;			//	SubWorld��߼���Region
	int			m_nTotalRegion;					//	SubWorld��Region����
	int			m_nRegionWidth;					//	Region�ĸ��ӿ��
	int			m_nRegionHeight;				//	Region�ĸ��Ӹ߶�
	int			m_nCellWidth;					//	Cell�����ؿ��
	int			m_nCellHeight;					//	Cell�����ظ߶�
	int			m_nRegionBeginX;				
	int			m_nRegionBeginY;
	int			m_nWeather;						//	�����仯
	DWORD		m_dwCurrentTime;				//	��ǰ֡
	KWorldMsg	m_WorldMessage;					//	��Ϣ
	KList		m_NoneRegionNpcList;			//	���ڵ�ͼ�ϵ�NPC

	//TamLTM bang hoi chiem linh
	DWORD		m_dwTongName;
	char        m_szTongName[32];
	char        m_szTongNameBC[32];
	DWORD		m_dwTongNameBC;
	int         m_nTongT;
	int         m_nTongVG;
	BOOL        m_bCheckTong;
	//end code

#ifdef _SERVER
	KWeatherMgr *m_pWeatherMgr;
#endif
private:
public:
	KSubWorld();
	~KSubWorld();
	void		Activate();
	void		GetFreeObjPos(POINT& pos);
	BOOL		CanPutObj(POINT pos);
	void		ObjChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	void		MissleChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	void		AddPlayer(int nRegion, int nIdx);
	void		RemovePlayer(int nRegion, int nIdx);
	void		Close();
	int			GetDistance(int nRx1, int nRy1, int nRx2, int nRy2);						// ���ؼ�����
	void		Map2Mps(int nR, int nX, int nY, int nDx, int nDy, int *nRx, int *nRy);		// ��������ת��������
	static void Map2Mps(int nRx, int nRy, int nX, int nY, int nDx, int nDy, int *pnX, int *pnY);		// ��������ת��������
	void		Mps2Map(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy);	// ��������ת��������
	void		GetMps(int *nX, int *nY, int nSpeed, int nDir, int nMaxDir = 64);			// ȡ��ĳ����ĳ�ٶ���һ�������
	BYTE		TestBarrier(int nMpsX, int nMpsY);
	BYTE		TestBarrier(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY);	// �����һ���Ƿ�Ϊ�ϰ�
	BYTE		TestBarrierMin(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY);	// �����һ���Ƿ�Ϊ�ϰ�
	BYTE		GetBarrier(int nMpsX, int nMpsY);											// ȡ��ĳ����ϰ���Ϣ
	DWORD		GetTrap(int nMpsX, int nMpsY);
	void		MessageLoop();
	int			FindRegion(int RegionID);													// �ҵ�ĳID��Region������
	int			FindFreeRegion(int nX = 0, int nY = 0);

	//TamLTM Bang hoi chiem linh thanh thi thon
	void		LoadTong(char* szMapTongName,int nMapTongId,int nMapTongT,int nMapTongVG,char* szMapTongNameBC,int nMapTongBCId,int nCheckMap);
	//end code

#ifdef _SERVER
	int			RevivalAllNpc();//����ͼ�����е�Npc������������Npcȫ���ָ���ԭʼ״̬
	void		BroadCast(const char* pBuffer, size_t uSize);
	void		BroadCastRegion(const void* pBuffer, size_t uSize, int &nMaxCount, int nRegionIndex, int nOX, int nOY);
	BOOL		LoadMap(int nIdx);
	void		LoadObject(char* szPath, char* szFile);
	void		NpcChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nNpcIdx);
	void		PlayerChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	BOOL		SendSyncData(int nIdx, int nClient);
	int			GetRegionIndex(int nRegionID);
	int			FindNpcFromName(const char * szName);
	int 		ExecuteScript(DWORD scriptId, char* functionName, int nParam1, int nParam2);
	int 		ExecuteScript(char* scriptFileName, char* functionName, int nParam, int worldIndex);
#endif
#ifndef _SERVER
	BOOL		LoadMap(int nIdx, int nRegion);
	void		NpcChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nNpcIdx);
	void		Paint();
	void		Mps2Screen(int *Rx, int *Ry);
	void		Screen2Mps(int *Rx, int *Ry);
#endif
private:
	void		LoadTrap();
	void		ProcessMsg(KWorldMsgNode *pMsg);
#ifndef _SERVER
	void		LoadCell();
#endif
};

#ifndef TOOLVERSION
extern KSubWorld	SubWorld[MAX_SUBWORLD];
#else 
extern CORE_API KSubWorld	SubWorld[MAX_SUBWORLD];
#endif
#endif
