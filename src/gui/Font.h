#ifndef FONT_H
#define FONT_H

//===========================================================================================
#if (MODULE_GUI_SYSTEM_FONT == 0)
#else

//===========================================================================================

typedef struct
{
	DWORD Head;
	WORD Width;
	WORD Height;
	WORD StartCode;
	WORD EndCode;
	DWORD SizeOfFnt;
	BYTE *Fnts;
} FNT;

typedef enum
{
	CS_GBK = 0x00,
	CS_ENG = 0x01
} CODE_SET;

typedef enum
{
	FT_GBK						= 0x00,
	FT_ENG_VT100_5X7			= 0x01,
	FT_ENG_ARIAL_B_8X12		= 0x02,
	FT_ENG_ARIAL_B_12X16		= 0x03,
	FT_ENG_06X12				= 0x04,
	FT_END
} FONT_TYPE;

//===========================================================================================

void Font_Init(void);
void Font_Load(WORD CodeSet, WORD FontType, FNT *Fnt);
void Font_SelectEngFont(WORD FontType);
WORD Font_GetFontHeight(WORD CodeSet);
WORD Font_GetFontWidth(WORD CodeSet);
void Font_GetPixelData(WORD CodeSet, WCHAR wc, BYTE *Data);
BYTE Font_GetPixel(WORD x, WORD y, WORD width, BYTE *data);


#endif


#endif
