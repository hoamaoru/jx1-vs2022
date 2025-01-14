//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerTask.cpp
// Date:	2002.10.05
// Code:	�߳�����
// Desc:	PlayerTask Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KPlayerTask.h"
#include	"KPlayer.h"
#include	"KPlayerChat.h"
#include	"KPlayerPK.h"
#include 	"GameDataDef.h"
#include	"KTaskFuns.h"
#ifdef _SERVER
int			g_TaskGlobalValue[TASKGLOBALVALUENUM]; //ȫ�ֵı��������ڷ������ű�ϵͳ

KTabFile	g_WayPointTabFile;
KTabFile	g_StationTabFile;
KTabFile	g_DockTabFile;
KTabFile	g_StationPriceTabFile;
KTabFile	g_WayPointPriceTabFile;
KTabFile	g_DockPriceTabFile;

int	*		g_pStationPriceTab = NULL;
int	*		g_pWayPointPriceTab = NULL;
int	*		g_pDockPriceTab = NULL;

#endif
//---------------------------------------------------------------------------
//	���ܣ����캯��
//---------------------------------------------------------------------------
KPlayerTask::KPlayerTask()
{	
	#ifdef _SERVER
		m_nPlayerIndex = 0;
	#endif
	Release();
}

#ifdef _SERVER
void KPlayerTask::Init(int nPlayerIdx)
{
	this->m_nPlayerIndex = nPlayerIdx;
}
#endif



//---------------------------------------------------------------------------
//	���ܣ����
//---------------------------------------------------------------------------
//=================

void KPlayerTask::SetSaveVal(int nTaskID, char* szTaskVal, BOOL bSync)
{	
	if (nTaskID < 0 || nTaskID >= MAX_TASK)
		return;
	
	strcpy(szSave[nTaskID], szTaskVal);

#ifdef _SERVER
	if(bSync || (nTaskID >= TASKVALUE_BASEDATA_PASSWORD && nTaskID < TASKVALUE_STATTASK_NUM))
		SyncTaskValue(nTaskID);
#endif

}

void KPlayerTask::SetSaveVal(int nTaskID, int nTaskVal, BOOL bSync)
{	
	if (nTaskID < 0 || nTaskID >= MAX_TASK)
		return;

	itoa(nTaskVal, szSave[nTaskID], 10);

#ifdef _SERVER
	if(bSync || (nTaskID >= TASKVALUE_BASEDATA_PASSWORD && nTaskID < TASKVALUE_STATTASK_NUM))
		SyncTaskValue(nTaskID);
#endif
}

#ifdef _SERVER
BOOL	KPlayerTask::SyncTaskValue(int nTaskID)
{	
	if (nTaskID < 0 || nTaskID >= MAX_TASK)
		return FALSE;

	S2C_SYNCTASKVALUE SyncTaskValue;
	SyncTaskValue.ProtocolType = s2c_synctaskvalue;
	SyncTaskValue.nTaskId = nTaskID;
	strcpy(SyncTaskValue.szTaskValue, szSave[nTaskID]);

//	g_DebugLog("s2c_synctaskvalue %d", s2c_synctaskvalue); //TamLTM Debug error packet

	if (g_pServer && Player[m_nPlayerIndex].m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&SyncTaskValue, sizeof(S2C_SYNCTASKVALUE));

	return TRUE;
}
#endif

int		KPlayerTask::GetSaveVal(int nNo)
{	
	if (nNo < 0 || nNo >= MAX_TASK)
		return 0;

	int nResult = 0;
	if (szSave[nNo][0])
		nResult = atoi(szSave[nNo]);
	return nResult;
}


const char*		KPlayerTask::GetSaveStr(int nNo)
{	
	if (nNo < 0 || nNo >= MAX_TASK)
		return "";

	return szSave[nNo];
}
//---------------------------------------------------------------------------
//	���ܣ������ʱ���̿��Ʊ���
//---------------------------------------------------------------------------
void	KPlayerTask::ClearTempVar()
{
	memset(szClear, 0, sizeof(szClear));
}


//---------------------------------------------------------------------------
//	���ܣ��õ���ʱ���̿��Ʊ���ֵ
//---------------------------------------------------------------------------
int		KPlayerTask::GetClearVal(int nNo)
{
	if (nNo < 0 || nNo >= MAX_TEMP_TASK)
		return 0;

	int nResult = 0;
	if (szClear[nNo][0])
		nResult = atoi(szClear[nNo]);
	return nResult;
}

const char*		KPlayerTask::GetClearStr(int nNo)
{
	if (nNo < 0 || nNo >= MAX_TEMP_TASK)
		return "";

	return szClear[nNo];
}
//---------------------------------------------------------------------------
//	���ܣ��趨��ʱ���̿��Ʊ���ֵ
//---------------------------------------------------------------------------
void	KPlayerTask::SetClearVal(int nNo, char* szVal)
{
	if (nNo < 0 || nNo >= MAX_TEMP_TASK)
		return;

	strcpy(szClear[nNo], szVal);
}

void	KPlayerTask::Release()
{
	memset(szSave, 0, sizeof(szSave));
	memset(szClear, 0, sizeof(szClear));
}