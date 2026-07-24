//---------------------------------------------------------------------------
// SprExportTool
//
// Doc file spr_read.log (ghi boi hook trong Engine\Src\KPakFile.cpp) va
// export lai tung file .spr duoc liet ke trong do, thanh file .spr chuan
// tren dia, doc duoc lai bang chinh engine (SprGetHeader tu file roi).
//
// Cach dung:
//   SprExportTool.exe <spr_read.log> [game_root_dir] [output_dir]
//
//   spr_read.log : file log tao ra boi client (mac dinh <client_dir>\logs\spr_read.log)
//   game_root_dir: thu muc chua package.ini + cac file .pak (mac dinh = thu muc hien tai)
//   output_dir   : thu muc se ghi cac file .spr da export (mac dinh = .\exported_spr)
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include "KPakList.h"
#include "KSprite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

KPakList g_PakList;

//---------------------------------------------------------------------------
// Tao toan bo thu muc cha (neu chua co) cho 1 duong dan file day du.
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
// Export 1 file .spr (duong dan logic pszSprPath, vd "\spr\ui3\...\x.spr")
// ra thanh file that tren dia, duoi pszOutRoot, giu nguyen cau truc thu muc.
//---------------------------------------------------------------------------
static bool ExportOneSpr(const char* pszSprPath, const char* pszOutRoot)
{
	SPROFFS* pOffsetTable = NULL;
	SPRHEAD* pSpr = SprGetHeader(pszSprPath, pOffsetTable);
	if (!pSpr)
	{
		printf("[NOTFOUND] %s\n", pszSprPath);
		return false;
	}

	char szOutPath[MAX_PATH];
	sprintf(szOutPath, "%s%s", pszOutRoot, pszSprPath);
	EnsureParentDirExists(szOutPath);

	FILE* fOut = fopen(szOutPath, "wb");
	if (!fOut)
	{
		printf("[WRITEFAIL] %s -> %s (khong mo duoc file dich)\n", pszSprPath, szOutPath);
		SprReleaseHeader(pSpr);
		return false;
	}

	bool bOk = false;

	if (pOffsetTable != NULL)
	{
		// Sprite nen gop 1 khoi (hoac file roi tren dia): buffer tra ve da
		// lien mach dung layout file .spr chuan, chi can ghi thang ra dia.
		DWORD dwLastOffset = pOffsetTable[pSpr->Frames - 1].Offset;
		DWORD dwLastLength = pOffsetTable[pSpr->Frames - 1].Length;
		size_t nTotal = sizeof(SPRHEAD) + (size_t)pSpr->Colors * 3
			+ (size_t)pSpr->Frames * sizeof(SPROFFS) + dwLastOffset + dwLastLength;

		// Reserved[] chi la vung nho scratch luc runtime (pak index / elem index),
		// khong co y nghia tren dia - xoa cho file xuat ra sach se. Phai xoa o day
		// (sau khi da doc xong du lieu), khong duoc xoa som hon vi cac ham doc frame
		// (nhanh else ben duoi) can nguyen Reserved[] de biet doc tu pak/elem nao.
		memset(pSpr->Reserved, 0, sizeof(pSpr->Reserved));

		bOk = (fwrite(pSpr, 1, nTotal, fOut) == nTotal);
	}
	else
	{
		// Sprite TYPE_FRAME: tung frame duoc nen rieng trong pak. Phai doc
		// tung frame qua SprGetFrame() (engine tu giai nen UCL), roi tu dung
		// lai bang SPROFFS va noi cac frame lai theo dung layout file chuan.
		int nFrames = pSpr->Frames;
		void** ppFrameBuf = (void**)malloc(sizeof(void*) * nFrames);
		unsigned int* puFrameSize = (unsigned int*)malloc(sizeof(unsigned int) * nFrames);
		bool bAllFramesOk = (nFrames > 0);

		int i;
		for (i = 0; i < nFrames; i++)
		{
			unsigned int uSize = 0;
			SPRFRAME* pFrame = SprGetFrame(pSpr, i, &uSize);
			ppFrameBuf[i] = pFrame;
			puFrameSize[i] = uSize;
			if (!pFrame || uSize == 0)
				bAllFramesOk = false;
		}

		if (bAllFramesOk)
		{
			SPROFFS* pNewOffsets = (SPROFFS*)malloc(sizeof(SPROFFS) * nFrames);
			DWORD dwRunning = 0;
			for (i = 0; i < nFrames; i++)
			{
				pNewOffsets[i].Offset = dwRunning;
				pNewOffsets[i].Length = puFrameSize[i];
				dwRunning += puFrameSize[i];
			}

			size_t nPaletteSize = (size_t)pSpr->Colors * 3;
			size_t nOffsTableSize = sizeof(SPROFFS) * (size_t)nFrames;

			// Tat ca frame da doc xong, tu day khong con dung Reserved[] nua nen
			// co the xoa sach truoc khi ghi (xem giai thich o nhanh direct ben tren).
			memset(pSpr->Reserved, 0, sizeof(pSpr->Reserved));

			bOk = (fwrite(pSpr, 1, sizeof(SPRHEAD), fOut) == sizeof(SPRHEAD));
			bOk = bOk && (fwrite((char*)pSpr + sizeof(SPRHEAD), 1, nPaletteSize, fOut) == nPaletteSize);
			bOk = bOk && (fwrite(pNewOffsets, 1, nOffsTableSize, fOut) == nOffsTableSize);
			for (i = 0; i < nFrames && bOk; i++)
				bOk = bOk && (fwrite(ppFrameBuf[i], 1, puFrameSize[i], fOut) == puFrameSize[i]);

			free(pNewOffsets);
		}

		for (i = 0; i < nFrames; i++)
		{
			if (ppFrameBuf[i])
				SprReleaseFrame((SPRFRAME*)ppFrameBuf[i]);
		}
		free(ppFrameBuf);
		free(puFrameSize);
	}

	const char* pszMode = (pOffsetTable != NULL) ? "direct" : "frame";
	int nFramesForLog = pSpr->Frames;

	fclose(fOut);
	SprReleaseHeader(pSpr);

	if (bOk)
		printf("[OK] (%s, %d frames) %s -> %s\n", pszMode, nFramesForLog, pszSprPath, szOutPath);
	else
		printf("[WRITEFAIL] (%s) %s -> %s\n", pszMode, pszSprPath, szOutPath);

	return bOk;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Cach dung: SprExportTool.exe <spr_read.log> [game_root_dir] [output_dir]\n");
		printf("  spr_read.log  : file log tao boi client (logs\\spr_read.log)\n");
		printf("  game_root_dir : thu muc chua package.ini + cac file .pak (mac dinh = thu muc hien tai)\n");
		printf("  output_dir    : thu muc ghi cac file .spr da export (mac dinh = .\\exported_spr)\n");
		return 1;
	}

	const char* pszLogFile = argv[1];
	const char* pszGameRoot = (argc >= 3) ? argv[2] : NULL;
	const char* pszOutDir = (argc >= 4) ? argv[3] : ".\\exported_spr";

	g_SetRootPath(pszGameRoot ? (char*)pszGameRoot : NULL);
	g_SetFilePath("\\");

	if (!g_PakList.Open("\\package.ini"))
		printf("Canh bao: khong mo duoc package.ini trong thu muc goc - chi doc duoc file .spr roi tren dia.\n");

	CreateDirectory(pszOutDir, NULL);

	FILE* fLog = fopen(pszLogFile, "r");
	if (!fLog)
	{
		printf("Loi: khong mo duoc file log: %s\n", pszLogFile);
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
		if (ExportOneSpr(szLine, pszOutDir))
			nOk++;
		else
			nFail++;
	}
	fclose(fLog);

	printf("\nTong: %d, thanh cong: %d, loi: %d\n", nTotal, nOk, nFail);
	return (nFail > 0) ? 1 : 0;
}
