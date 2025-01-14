//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerChat.h
// Date:	2002.10.05
// Code:	�߳�����
// Desc:	PlayerChat Class
//---------------------------------------------------------------------------

#ifndef KPLAYERCHAT_H
#define KPLAYERCHAT_H

#include	"GameDataDef.h"
#ifndef _SERVER
#include	"KNode.h"
#include	"KList.h"
#endif

#ifdef _SERVER

// ÿ��������ѵ�����
class CChatFriend : public KNode
{
public:
	DWORD			m_dwID;					// ����ID
	int				m_nPlayerIdx;			// ������ player �����е�λ��
	char			m_szName[32];			// ��������

public:
	CChatFriend() {Release();};
	void			Release() {m_dwID = 0; memset(m_szName, 0, 32);};
};
#endif

#ifdef _SERVER
#include	<list>
#endif

class KPlayerChat
{
public:
	void			Release();
public:
#ifdef _SERVER
	static	void	SendSystemInfo(int nType, int nTargetIdx, char *lpszSendName, char *lpszSentence, int nSentenceLength, int nChannedID = -1);
	static	void	SendGlobalSystemInfo(char *lpszSendName, char *lpszSentence, int nSentenceLength);
	typedef std::list<std::string>	STRINGLIST;
	static	void	MakeBrother(const STRINGLIST& brothers);
	static	void	MakeEnemy(char* szPlayer, char* szEnemy);
	static	void	MakeMate(char* szPlayer, char* szMate);
	static	void	SendInfoToGM(char *lpszAccName, char *lpszRoleName, char *lpszSentence, int nSentenceLength);
	static	void	SendInfoToIP(DWORD nIP, DWORD nID, char *lpszAccName, char *lpszRoleName, char *lpszSentence, int nSentenceLength);
#endif
};

#endif

