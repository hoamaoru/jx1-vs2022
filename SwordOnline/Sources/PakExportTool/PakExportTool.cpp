//---------------------------------------------------------------------------
// PakExportTool
//
// Doc danh sach duong dan (moi loai file, khong rieng .spr) va export nguyen
// ven tung file ra dia, dung class KPakFile - lop doc file generic cua engine,
// tu dong tim trong .pak hoac tren dia roi (khong can biet truoc loai nen).
//
// Cach dung:
//   PakExportTool.exe <path_list.log> [game_root_dir] [output_dir]
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include "KPakList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

KPakList g_PakList;

static void EnsureParentDirExists(const char* pszFullFilePath)
{
	char szDir[MAX_PATH];
	strcpy(szDir, pszFullFilePath);

	int nLen = (int)strlen(szDir);
	int i;
	for (i = nLen - 1; i >= 0; i--)
	{
		if (szDir[i] == '\\' || szDir[i] == '/')
		{
			szDir[i] = 0;
			break;
		}
	}

	for (i = 1; szDir[i]; i++)
	{
		if (szDir[i] == '\\' || szDir[i] == '/')
		{
			char c = szDir[i];
			szDir[i] = 0;
			CreateDirectory(szDir, NULL);
			szDir[i] = c;
		}
	}
	CreateDirectory(szDir, NULL);
}

static bool ExportOneFile(const char* pszPath, const char* pszOutRoot)
{
	KPakFile file;
	if (!file.Open(pszPath))
	{
		printf("[NOTFOUND] %s\n", pszPath);
		return false;
	}

	DWORD uSize = file.Size();
	if (uSize == 0)
	{
		file.Close();
		printf("[EMPTY] %s\n", pszPath);
		return false;
	}

	void* pBuffer = malloc(uSize);
	if (!pBuffer)
	{
		file.Close();
		printf("[WRITEFAIL] %s (khong cap phat duoc bo nho)\n", pszPath);
		return false;
	}

	DWORD uRead = file.Read(pBuffer, uSize);
	file.Close();

	if (uRead != uSize)
	{
		free(pBuffer);
		printf("[READFAIL] %s (doc %u/%u byte)\n", pszPath, uRead, uSize);
		return false;
	}

	char szOutPath[MAX_PATH];
	sprintf(szOutPath, "%s%s", pszOutRoot, pszPath);
	EnsureParentDirExists(szOutPath);

	FILE* fOut = fopen(szOutPath, "wb");
	if (!fOut)
	{
		free(pBuffer);
		printf("[WRITEFAIL] %s -> %s (khong mo duoc file dich)\n", pszPath, szOutPath);
		return false;
	}

	bool bOk = (fwrite(pBuffer, 1, uSize, fOut) == uSize);
	fclose(fOut);
	free(pBuffer);

	if (bOk)
		printf("[OK] (%u byte) %s -> %s\n", uSize, pszPath, szOutPath);
	else
		printf("[WRITEFAIL] %s -> %s\n", pszPath, szOutPath);

	return bOk;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Cach dung: PakExportTool.exe <path_list.log> [game_root_dir] [output_dir]\n");
		printf("  path_list.log : file danh sach duong dan (moi dong 1 duong dan, cot dau neu co tab)\n");
		printf("  game_root_dir : thu muc chua package.ini + cac file .pak (mac dinh = thu muc hien tai)\n");
		printf("  output_dir    : thu muc ghi cac file da export (mac dinh = .\\exported_all)\n");
		return 1;
	}

	const char* pszLogFile = argv[1];
	const char* pszGameRoot = (argc >= 3) ? argv[2] : NULL;
	const char* pszOutDir = (argc >= 4) ? argv[3] : ".\\exported_all";

	g_SetRootPath(pszGameRoot ? (char*)pszGameRoot : NULL);
	g_SetFilePath("\\");

	if (!g_PakList.Open("\\package.ini"))
		printf("Canh bao: khong mo duoc package.ini trong thu muc goc - chi doc duoc file roi tren dia.\n");

	CreateDirectory(pszOutDir, NULL);

	FILE* fLog = fopen(pszLogFile, "r");
	if (!fLog)
	{
		printf("Loi: khong mo duoc file danh sach: %s\n", pszLogFile);
		return 1;
	}

	char szLine[1024];
	int nTotal = 0, nOk = 0, nFail = 0;

	while (fgets(szLine, sizeof(szLine), fLog))
	{
		char* pTab = strchr(szLine, '\t');
		if (pTab)
			*pTab = 0;

		int nLen = (int)strlen(szLine);
		while (nLen > 0 && (szLine[nLen - 1] == '\n' || szLine[nLen - 1] == '\r'))
			szLine[--nLen] = 0;

		if (nLen == 0)
			continue;

		nTotal++;
		if (ExportOneFile(szLine, pszOutDir))
			nOk++;
		else
			nFail++;
	}
	fclose(fLog);

	printf("\nTong: %d, thanh cong: %d, loi: %d\n", nTotal, nOk, nFail);
	return (nFail > 0) ? 1 : 0;
}
