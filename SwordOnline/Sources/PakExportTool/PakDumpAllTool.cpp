//---------------------------------------------------------------------------
// PakDumpAllTool
//
// Xuat TOAN BO entry trong 1 file .pak theo VI TRI (khong can biet ten),
// dung KPakList::FindElemFile(unsigned long uId, ...) + ElemFileRead - ham
// tra cuu generic theo ID hash, khong phu thuoc ten file. Danh sach ID doc
// truc tiep tu bang index cua pak (xem extract_maps_ids.ps1). Vi khong biet
// ten goc, file duoc dat ten theo hash + doan duoi qua magic byte.
//
// Cach dung:
//   PakDumpAllTool.exe <id_list.txt> [game_root_dir] [output_dir]
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include "KPakList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

KPakList g_PakList;

static const char* SniffExtension(const unsigned char* pData, int nSize)
{
	if (nSize >= 4 && memcmp(pData, "SPR\0", 4) == 0)
		return "spr";
	if (nSize >= 4 && memcmp(pData, "RIFF", 4) == 0)
		return "wav";
	if (nSize >= 3 && pData[0] == 0xFF && pData[1] == 0xD8 && pData[2] == 0xFF)
		return "jpg";
	if (nSize >= 2 && pData[0] == 'B' && pData[1] == 'M')
		return "bmp";
	if (nSize >= 4 && memcmp(pData, "GIF8", 4) == 0)
		return "gif";
	if (nSize >= 8 && memcmp(pData, "\x89PNG\r\n\x1a\n", 8) == 0)
		return "png";
	if (nSize >= 2 && (unsigned char)pData[0] == 0x0A && (unsigned char)pData[1] <= 5)
		return "pcx";
	if (nSize >= 4 && pData[0] == 0x00 && pData[1] == 0x01 && pData[2] == 0x00 && pData[3] == 0x00)
		return "ttf";
	if (nSize >= 4 && memcmp(pData, "OTTO", 4) == 0)
		return "otf";
	if (nSize >= 4 && (unsigned char)pData[0] == 0x1B && memcmp(pData + 1, "Lua", 3) == 0)
		return "luac";

	int nCheckLen = (nSize < 64) ? nSize : 64;
	bool bLooksText = true;
	for (int i = 0; i < nCheckLen; i++)
	{
		unsigned char c = pData[i];
		if (c == 0 || (c < 9))
		{
			bLooksText = false;
			break;
		}
	}
	if (bLooksText)
		return "txt";

	return "bin";
}

static bool DumpOneEntry(unsigned long uId, const char* pszOutRoot, int* pnFail)
{
	XPackElemFileRef ref;
	if (!g_PakList.FindElemFile(uId, ref))
	{
		printf("[NOTFOUND] id=0x%08lX\n", uId);
		(*pnFail)++;
		return false;
	}

	if (ref.nSize <= 0)
	{
		printf("[EMPTY] id=0x%08lX\n", uId);
		(*pnFail)++;
		return false;
	}

	void* pBuffer = malloc(ref.nSize);
	if (!pBuffer)
	{
		printf("[WRITEFAIL] id=0x%08lX (khong cap phat duoc bo nho)\n", uId);
		(*pnFail)++;
		return false;
	}

	int nRead = g_PakList.ElemFileRead(ref, pBuffer, (unsigned)ref.nSize);
	if (nRead != ref.nSize)
	{
		free(pBuffer);
		printf("[READFAIL] id=0x%08lX (doc %d/%d byte)\n", uId, nRead, ref.nSize);
		(*pnFail)++;
		return false;
	}

	const char* pszExt = SniffExtension((unsigned char*)pBuffer, ref.nSize);

	char szOutPath[MAX_PATH];
	sprintf(szOutPath, "%s\\unknown_%08lX.%s", pszOutRoot, uId, pszExt);

	FILE* fOut = fopen(szOutPath, "wb");
	if (!fOut)
	{
		free(pBuffer);
		printf("[WRITEFAIL] id=0x%08lX -> %s (khong mo duoc file dich)\n", uId, szOutPath);
		(*pnFail)++;
		return false;
	}

	bool bOk = (fwrite(pBuffer, 1, ref.nSize, fOut) == (size_t)ref.nSize);
	fclose(fOut);
	free(pBuffer);

	if (!bOk)
	{
		printf("[WRITEFAIL] id=0x%08lX -> %s\n", uId, szOutPath);
		(*pnFail)++;
		return false;
	}

	printf("[OK] (%d byte, .%s) id=0x%08lX -> %s\n", ref.nSize, pszExt, uId, szOutPath);
	return true;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Cach dung: PakDumpAllTool.exe <id_list.txt> [game_root_dir] [output_dir]\n");
		printf("  id_list.txt   : file danh sach ID (moi dong 1 so, dang thap phan, doc tu bang index pak)\n");
		printf("  game_root_dir : thu muc chua package.ini + cac file .pak (mac dinh = thu muc hien tai)\n");
		printf("  output_dir    : thu muc ghi cac file da xuat (mac dinh = .\\dumped_all)\n");
		return 1;
	}

	const char* pszIdListFile = argv[1];
	const char* pszGameRoot = (argc >= 3) ? argv[2] : NULL;
	const char* pszOutDir = (argc >= 4) ? argv[3] : ".\\dumped_all";

	g_SetRootPath(pszGameRoot ? (char*)pszGameRoot : NULL);
	g_SetFilePath("\\");

	if (!g_PakList.Open("\\package.ini"))
	{
		printf("Loi: khong mo duoc package.ini trong thu muc goc.\n");
		return 1;
	}

	CreateDirectory(pszOutDir, NULL);

	FILE* fIdList = fopen(pszIdListFile, "r");
	if (!fIdList)
	{
		printf("Loi: khong mo duoc file danh sach ID: %s\n", pszIdListFile);
		return 1;
	}

	char szLine[64];
	int nTotal = 0, nOk = 0, nFail = 0;

	while (fgets(szLine, sizeof(szLine), fIdList))
	{
		unsigned long uId = strtoul(szLine, NULL, 10);
		if (uId == 0)
			continue;

		nTotal++;
		if (DumpOneEntry(uId, pszOutDir, &nFail))
			nOk++;

		if (nTotal % 2000 == 0)
			printf("... da xu ly %d / tong so dang doc\n", nTotal);
	}
	fclose(fIdList);

	printf("\nTong: %d, thanh cong: %d, loi: %d\n", nTotal, nOk, nFail);
	return (nFail > 0) ? 1 : 0;
}
