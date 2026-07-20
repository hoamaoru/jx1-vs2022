//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KJpegLib.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KJpegLib_H
#define KJpegLib_H
//---------------------------------------------------------------------------
#pragma pack (push)	// packđẩy giá trị
#pragma pack (1)	// pack = 1

// Jpeg file info
typedef struct {
	int mode;		// chế độ màu
	int width;		// chiều rộng
	int height;		// cao
} JPEG_INFO;

// Huffman code struct
typedef struct {
	WORD code;		// huffman mã hóa
	BYTE len;		// chiều dài mặt nạ
	BYTE num;		// byte đại diện
} JPEG_HCODE;

// Huffman table
typedef struct {
	JPEG_HCODE *htb;
	int num;
} JPEG_HTABLE;

// Jpeg compoents
typedef struct {
	BYTE v;
	BYTE h;
	BYTE qtb;
	BYTE act;
	BYTE dct;
} JPEG_COMPONENT;

// Jpeg start of frame 0
typedef struct {
	BYTE colordepth;
	WORD height;
	WORD width;
	BYTE components;
	JPEG_COMPONENT component[3];
} JPEG_SOF0;

#pragma pack (pop)	// packgiá trị xuất hiện
//---------------------------------------------------------------------------
#define READ_BYTE(stream) (*stream++)
// Reads a big-endian 16-bit value from the byte stream and advances it by 2
// (portable equivalent of the original x86 `lodsw` + `xchg al,ah` asm).
#define READ_WORD(a,stream) \
	do { \
		(a) = (WORD)(((stream)[0] << 8) | (stream)[1]); \
		(stream) += 2; \
	} while (0)
//---------------------------------------------------------------------------
// decode vars
extern PBYTE		jpeg_stream;
extern short		jpeg_ybuf[256];
extern short		jpeg_cbbuf[64];
extern short		jpeg_crbuf[64];
extern short		jpeg_DC[3];
extern short*		jpeg_qtable[4];
extern JPEG_HTABLE	jpeg_htable[8];
extern BYTE			jpeg_bit;
extern JPEG_SOF0	jpeg_head;

// interface
extern BOOL			jpeg_decode_init(BOOL bRGB555, BOOL bMMXCPU);
extern BOOL			jpeg_decode_info(PBYTE pJpgBuf, JPEG_INFO* pInfo);
extern BOOL			jpeg_decode_data(PWORD pBmpBuf, JPEG_INFO* pInfo);
extern BOOL			jpeg_decode_dataEx(PWORD pBmpBuf, int pitch, JPEG_INFO* pInfo);

// decode functions
extern PBYTE		jpeg_read_DQT(PBYTE stream);
extern PBYTE		jpeg_read_DHT(PBYTE stream);
extern PBYTE		jpeg_read_SOF(PBYTE stream);
extern PBYTE		jpeg_read_SOS(PBYTE stream);
extern PBYTE		jpeg_skip_SEG(PBYTE stream);
extern void			jpeg_preprocess(LPBYTE stream);
extern void			jpeg_decode_DU(short* buf, int com);
extern void			jpeg_init_table();
extern void			jpeg_free_table();
extern void			jpeg_idct_mmx(short* buf);
extern void			jpeg_idct_2d(short* buf);

// function pointers
extern void			(*jpeg_IDCT)(short* buf);
extern void			(*jpeg_Y2RGB)(WORD *bmppixel, int pitch);
extern void			(*jpeg_YCbCr411)(WORD *bmppixel, int pitch);
extern void			(*jpeg_YCbCr111)(WORD *bmppixel, int pitch);
//---------------------------------------------------------------------------
#endif
