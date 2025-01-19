// -------------------------------------------------------------------------
//	文件名		：	inoutmac.h
//	创建者		：	谢茂培 (Hsie)
//	创建时间	：	2003-07-20 14:47:18
//	功能描述	：	
//
// -------------------------------------------------------------------------
#ifndef __INOUTMAC_H__
#define __INOUTMAC_H__


#include "malloc.h"

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.


typedef struct {
    char String[4 * 4];
} K_IP_ADDRESS_STRING, *K_PIP_ADDRESS_STRING, K_IP_MASK_STRING, *K_PIP_MASK_STRING;

typedef struct _K_IP_ADDR_STRING {
    struct _K_IP_ADDR_STRING* Next;
    K_IP_ADDRESS_STRING IpAddress;
    K_IP_MASK_STRING IpMask;
    DWORD Context;
} K_IP_ADDR_STRING, *K_PIP_ADDR_STRING;

typedef struct _K_IP_ADAPTER_INFO {
    struct _K_IP_ADAPTER_INFO* Next;
    DWORD ComboIndex;
    char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
    char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
    UINT AddressLength;
    BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
    DWORD Index;
    UINT Type;
    UINT DhcpEnabled;
    K_PIP_ADDR_STRING CurrentIpAddress;
    K_IP_ADDR_STRING IpAddressList;
    K_IP_ADDR_STRING GatewayList;
    K_IP_ADDR_STRING DhcpServer;
    BOOL HaveWins;
    K_IP_ADDR_STRING PrimaryWinsServer;
    K_IP_ADDR_STRING SecondaryWinsServer;
    time_t LeaseObtained;
    time_t LeaseExpires;
} K_IP_ADAPTER_INFO, *K_PIP_ADAPTER_INFO;

inline void ConvertMacAddressStringIntoByte(const char* pszMACAddress, unsigned char* pbyAddress, const char cSep = '-')
{
	for (int iConunter = 0; iConunter < 6; ++iConunter)
	{
		unsigned int iNumber = 0;
		char ch;
		//Convert letter into lower case.
		ch = tolower(*pszMACAddress++);
		if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
		{
			return;
		}
		//Convert into number. 
		//       a. If character is digit then ch - '0'
		//	b. else (ch - 'a' + 10) it is done 
		//	because addition of 10 takes correct value.
		iNumber = isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);
		ch = tolower(*pszMACAddress);
		if ((iConunter < 5 && ch != cSep) ||
			(iConunter == 5 && ch != '\0' && !isspace(ch)))
		{
			++pszMACAddress;
			if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
			{
				return;
			}
			iNumber <<= 4;
			iNumber += isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);
			ch = *pszMACAddress;
			if (iConunter < 5 && ch != cSep)
			{
				return;
			}
		}
		/* Store result.  */
		pbyAddress[iConunter] = (unsigned char)iNumber;
		/* Skip cSep.  */
		++pszMACAddress;
	}
}

//此函数会确保两个地址均有
//返回值为网卡数量
inline int gGetMacAndIPAddress(
	BYTE* pMacAddressIn, 
	DWORD* pIPAddressIn, 
	BYTE* pMacAddressOut, 
	DWORD* pIPAddressOut, 
	DWORD nMask = 0x0000a8c0, 
	DWORD nMacLength = 6,
	char* cIntranetIP = "127.0.0.1",
	char* cInternetIP = "127.0.0.1",
	char* cMac = "00-00-00-00-00-00"
)
{
	typedef DWORD(CALLBACK * PGAINFO)(K_PIP_ADAPTER_INFO, PULONG);//GetAdaptersInfo

    //加载IP Helper API 所需的库文件
    HINSTANCE hInst = LoadLibrary("iphlpapi.dll");
    if(!hInst)
		return -1;
    PGAINFO pGAInfo = (PGAINFO)GetProcAddress(hInst,"GetAdaptersInfo");
	if (!pGAInfo)
		return -1;
	K_PIP_ADAPTER_INFO pInfo = NULL;

	ULONG ulSize = 0;

	pGAInfo(pInfo, &ulSize);//第一次调用，获取缓冲区大小

	pInfo = (K_PIP_ADAPTER_INFO)alloca(ulSize);

	pGAInfo(pInfo, &ulSize);

	unsigned char MAC[6] = { '\0' };
	ConvertMacAddressStringIntoByte(cMac, MAC);
	if (pMacAddressIn) {
		memcpy(pMacAddressIn, MAC, nMacLength);
	}

	if (pMacAddressOut) {
		memcpy(pMacAddressOut, MAC, nMacLength);
	}

	if (pIPAddressIn) {
		*pIPAddressIn = inet_addr(cIntranetIP);
	}

	if (pIPAddressOut) {
		*pIPAddressOut = inet_addr(cInternetIP);
	}

	DWORD nMAcNum = 0;

	while (pInfo) {
		pInfo = pInfo->Next;
		nMAcNum++;
	}

	FreeLibrary(hInst);

	return nMAcNum;
}


#endif // __INOUTMAC_H__
