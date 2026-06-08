//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KPakList.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Pack Data List Class
//---------------------------------------------------------------------------
#include <windows.h>
#include "KWin32.h"
#include "KDebug.h"
#include "KFilePath.h"
#include "KIniFile.h"
#include "KPakList.h"
#include "crtdbg.h"
#include <stdio.h>

// Define PAK_LOG_FILENAMES to enable runtime filename logging.
// Tắt logging: comment out dòng dưới, rebuild.
#define PAK_LOG_FILENAMES

#ifdef PAK_LOG_FILENAMES
#include <unordered_set>
#include <string>

static HANDLE                        s_hPakLogMutex = NULL;
static FILE*                         s_pPakLogFile  = NULL;
static std::unordered_set<std::string> s_loggedFiles;

static void PakLog_Init()
{
    s_hPakLogMutex = CreateMutexA(NULL, FALSE, NULL);
    char szPath[MAX_PATH];
    GetModuleFileNameA(NULL, szPath, MAX_PATH);
    char* slash = strrchr(szPath, '\\');
    if (slash) strcpy(slash + 1, "pak_filenames.log");
    else       strcpy(szPath, "pak_filenames.log");
    s_pPakLogFile = fopen(szPath, "w");
}

static void PakLog_Write(const char* pszFileName)
{
    if (!s_pPakLogFile) return;
    WaitForSingleObject(s_hPakLogMutex, INFINITE);
    if (s_loggedFiles.insert(pszFileName).second)   // second=true chỉ khi insert thành công (chưa có)
        fprintf(s_pPakLogFile, "%s\n", pszFileName);
    ReleaseMutex(s_hPakLogMutex);
}

static void PakLog_Flush()
{
    if (s_pPakLogFile) fflush(s_pPakLogFile);
}
#endif // PAK_LOG_FILENAMES

//---------------------------------------------------------------------------
ENGINE_API KPakList* g_pPakList = NULL;

//---------------------------------------------------------------------------
// ����:	���캯��
//---------------------------------------------------------------------------
KPakList::KPakList()
{
	g_pPakList = this;
	m_nPakNumber = 0;
#ifdef PAK_LOG_FILENAMES
	PakLog_Init();
#endif
}

//---------------------------------------------------------------------------
// ����:	���캯��
//---------------------------------------------------------------------------
KPakList::~KPakList()
{
	Close();
}

//---------------------------------------------------------------------------
// ����:	�ر������ļ�
//---------------------------------------------------------------------------
void KPakList::Close()
{
	for (int i = 0; i < m_nPakNumber; i++)
		delete m_PakFilePtrList[i];
	m_nPakNumber = 0;
#ifdef PAK_LOG_FILENAMES
	PakLog_Flush();
#endif
}

//---------------------------------------------------------------------------
// ����:	�����а���ɨ��ָ���ļ�
// ����:	uId			�ļ���ID
//			ElemRef		���ڴ�ţ��������ļ���Ϣ
// ����:	�Ƿ�ɹ��ҵ�
//---------------------------------------------------------------------------
bool KPakList::FindElemFile(unsigned long uId, XPackElemFileRef& ElemRef)
{
	bool bFounded = false;
	for (int i = 0; i < m_nPakNumber; i++)
	{
		if (m_PakFilePtrList[i]->FindElemFile(uId, ElemRef))
		{
			bFounded = true;
			break;
		}
	}
	return bFounded;
}

//---------------------------------------------------------------------------
// ����:	���ļ���ת��Ϊ���е�id
// ����:	pszFileName	�ļ���
// ����:	�ļ�����Ӧ�İ��е�id
//---------------------------------------------------------------------------
unsigned long KPakList::FileNameToId(const char* pszFileName)
{
	_ASSERT(pszFileName && pszFileName[0]);
	unsigned long id = 0;
	const char *ptr = pszFileName;
	int index = 0;
	while(*ptr)
	{
		if(*ptr >= 'A' && *ptr <= 'Z') id = (id + (++index) * (*ptr + 'a' - 'A')) % 0x8000000b * 0xffffffef;
		else id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		ptr++;
	}
	return (id ^ 0x12345678);
}

//---------------------------------------------------------------------------
// ����:	�����а���ɨ��ָ���ļ�
// ����:	pszFileName	�ļ���
//			ElemRef	���ڴ�ţ��������ļ���Ϣ
// ����:	�Ƿ�ɹ��ҵ�
//---------------------------------------------------------------------------
bool KPakList::FindElemFile(const char* pszFileName, XPackElemFileRef& ElemRef)
{
	bool bFounded = false;
	if (pszFileName && pszFileName[0])
	{
#ifdef PAK_LOG_FILENAMES
		PakLog_Write(pszFileName);
#endif
		char szPackName[128];
		#ifdef WIN32
			szPackName[0] = '\\';
		#else
			szPackName[0] = '/';
		#endif
		g_GetPackPath(szPackName + 1, (char*)pszFileName);
		unsigned long uId = FileNameToId(szPackName);
		bFounded = FindElemFile(uId, ElemRef);
	}
	return bFounded;
}

//--------------------------------------------------------------------
// ����:	Open package ini file
// ����:	char* filename
// ����:	BOOL
//---------------------------------------------------------------------------
bool KPakList::Open(const char* pPakListFile)
{
	Close();

	KIniFile IniFile;
	#define	SECTION "Package"

	bool bResult = false;
	if (IniFile.Load(pPakListFile))
	{
		char	szBuffer[32], szKey[16], szFile[MAX_PATH];

		if (IniFile.GetString(SECTION, "Path", "", szBuffer, sizeof(szBuffer)))
		{
			g_GetFullPath(szFile, szBuffer);
			int nNameStartPos = strlen(szFile);
			if (szFile[nNameStartPos - 1] != '\\' || szFile[nNameStartPos - 1] != '/')
			{
				#ifdef WIN32
					szFile[nNameStartPos++] = '\\';
				#else
					szFile[nNameStartPos++] = '/';
				#endif
				szFile[nNameStartPos] = 0;
			}

			for (int i = 0; i < MAX_PAK; i++)
			{
				itoa(i, szKey, 10);
				if (!IniFile.GetString(SECTION, szKey, "", szBuffer, sizeof(szBuffer)))
					break;
				if (szBuffer[0] == 0)
					break;
				strcpy(szFile + nNameStartPos, szBuffer);
				m_PakFilePtrList[m_nPakNumber] = new XPackFile;
				if (m_PakFilePtrList[m_nPakNumber])
				{
					if (m_PakFilePtrList[m_nPakNumber]->Open(szFile, m_nPakNumber))
					{
						m_nPakNumber++;
						//g_DebugLog("PakList Open : %s ... Ok", szFile);
					}
					else
					{
						g_DebugLog("PakList Open : %s ... Fail", szFile);
						delete (m_PakFilePtrList[m_nPakNumber]);
					}
				}
			}
			bResult = true;
		}
	}
	return bResult;
}

//��ȡ���ڵ����ļ�
int KPakList::ElemFileRead(XPackElemFileRef& ElemRef,
					void* pBuffer, unsigned uSize)
{
	if (ElemRef.nPackIndex >= 0 && ElemRef.nPackIndex < m_nPakNumber)
		return m_PakFilePtrList[ElemRef.nPackIndex]->ElemFileRead(ElemRef, pBuffer, uSize);
	return 0;
}

//��ȡspr�ļ�ͷ��������spr
SPRHEAD* KPakList::GetSprHeader(XPackElemFileRef& ElemRef, SPROFFS*& pOffsetTable)
{
	if (ElemRef.nPackIndex >= 0 && ElemRef.nPackIndex < m_nPakNumber)
		return (m_PakFilePtrList[ElemRef.nPackIndex]->GetSprHeader(ElemRef, pOffsetTable));
	return NULL;
}

//��ȡ��֡ѹ����spr��һ֡������
SPRFRAME* KPakList::GetSprFrame(int nPackIndex, SPRHEAD* pSprHeader, int nFrame)
{
	if (nPackIndex >= 0 && nPackIndex < m_nPakNumber)
		return m_PakFilePtrList[nPackIndex]->GetSprFrame(pSprHeader, nFrame);
	return NULL;
}
