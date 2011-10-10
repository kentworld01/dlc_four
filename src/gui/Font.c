//===========================================================================================
#include "Device.h"
#include "OS.h"
#ifdef _d_dir_file_modules
	#include "vfs/dir_file.h"
#endif
#include "GUI.h"


//===========================================================================================
#if (MODULE_GUI_SYSTEM_FONT == 0)
#else

//===========================================================================================
#if (DEBUG_FONT == 1)
	#define DEBUG						TRACE
	#define DEBUGx(a, b)				TRACEx(a, b)
#else
	#define DEBUG(...)					
	#define DEBUGx(a, b)				
#endif

//===========================================================================================

extern const unsigned char fnt_U0612[];
extern const unsigned char fnt_fonts_vt100_5x7[];
extern const unsigned char fnt_fonts_arial_bold_8x12[];
extern const unsigned char fnt_fonts_arial_bold_12x16[];


#define SIZE_OF_GBK_FONT				(0x00063000)

unsigned ADDR_GBK_FONT = ((DWORD)(0x00004000));

static FNT Fnt[FT_END];
static BYTE EngFontSelect = FT_ENG_06X12;

// chs font
// A01AD000
// 0005BDC0
// A0208DBF

// PA_GBK_FONT: 001AD000 ~ 0020BFFF
// VA_GBK_FONT: A01AD000 ~ A020BFFF

//===========================================================================================

void Font_Init(void)
{
	Font_Load(CS_GBK, FT_GBK, &Fnt[FT_GBK]);
	Font_Load(CS_ENG, FT_ENG_VT100_5X7, &Fnt[FT_ENG_VT100_5X7]);
	Font_Load(CS_ENG, FT_ENG_06X12, &Fnt[FT_ENG_06X12]);
	Font_Load(CS_ENG, FT_ENG_ARIAL_B_8X12, &Fnt[FT_ENG_ARIAL_B_8X12]);
	Font_Load(CS_ENG, FT_ENG_ARIAL_B_12X16, &Fnt[FT_ENG_ARIAL_B_12X16]);
}

void Font_Load(WORD CodeSet, WORD FontType, FNT *Fnt)
{
	BYTE buf[sizeof(FNT)], *p = buf, *pFntData;
	void *FontsData = 0;

	if (CodeSet == CS_ENG)
	{
		switch (FontType)
		{
		case FT_ENG_VT100_5X7:
			FontsData = (void *)fnt_fonts_vt100_5x7;
			break;
		case FT_ENG_06X12:
			FontsData = (void *)fnt_U0612;
			break;
		case FT_ENG_ARIAL_B_8X12:
			FontsData = (void *)fnt_fonts_arial_bold_8x12;
			break;
		case FT_ENG_ARIAL_B_12X16:
			FontsData = (void *)fnt_fonts_arial_bold_12x16;
			break;
		}
		// Load from ROM
		memcpy((void *)buf, (void *)FontsData, sizeof(FNT));
		pFntData = (BYTE *)((BYTE *)FontsData + sizeof(FNT));
	}
	else if (CodeSet == CS_GBK)
	{
#ifndef _d_dir_file_modules
		// Load from FLASH		// the old font_load method.
		//Flash_Read(ADDR_GBK_FONT, (void *)buf, sizeof(FNT));
		//pFntData = (BYTE *)(ADDR_GBK_FONT + sizeof(FNT));
#else
		// use file location the address.
		{
			void *h;
			h = df_open( "/c/fonts.dat", 0 );
			if( h ){
				ADDR_GBK_FONT = node_flash_get_file_address( h );
				_d_hex( ADDR_GBK_FONT );
				df_close( h );
				//ADDR_GBK_FONT += 0x18c00 ;
				Flash_Read( ADDR_GBK_FONT , buf, sizeof( FNT ) );
				_d_buf( buf, sizeof( FNT ) );
				pFntData = (BYTE *)(ADDR_GBK_FONT + sizeof(FNT));
				_d_hex( pFntData );
			}
		}
#endif
	}
	else
		return;

	DEBUG("CodeSet = %d, ", CodeSet);
	DEBUG("FontType = %d, FNT:\n", FontType);
	DEBUGx(buf, sizeof(FNT));

	Fnt->Head = GetLong_LE((BYTE *)p);
	p += sizeof(DWORD);

	// 字体文件有效性校验
	if (Fnt->Head != 0xaa555032)
	{
		memset((void *)Fnt, 0x00, sizeof(FNT));
		TRACE("CodeSet: %u, Font_Load fail!\n", CodeSet);

		return;
	}

	Fnt->Width = GetShort_LE((BYTE *)p);
	p += sizeof(WORD);

	Fnt->Height = GetShort_LE((BYTE *)p);
	p += sizeof(WORD);

	Fnt->StartCode = GetShort_LE((BYTE *)p);
	p += sizeof(WORD);

	Fnt->EndCode = GetShort_LE((BYTE *)p);
	p += sizeof(WORD);

	Fnt->SizeOfFnt = GetLong_LE((BYTE *)p);
	p += sizeof(DWORD);

	Fnt->Fnts = pFntData;

	DEBUGx(Fnt, sizeof(FNT));
}

void Font_SelectEngFont(WORD FontType)
{
	EngFontSelect = FontType;
}

WORD Font_GetFontHeight(WORD CodeSet)
{
	if (CodeSet == CS_GBK)
		return Fnt[FT_GBK].Height;
	else
		return Fnt[EngFontSelect].Height;
}

WORD Font_GetFontWidth(WORD CodeSet)
{
	if (CodeSet == CS_GBK)
		return Fnt[FT_GBK].Width;
	else
		return Fnt[EngFontSelect].Width;
}

void Font_GetPixelData(WORD CodeSet, WCHAR wc, BYTE *Data)
{
	DWORD bytes, fontbytes, fontaddr;
	FNT *fnt;
	BOOL is_load_success;

	if (CodeSet == CS_GBK)
		fnt = &Fnt[FT_GBK];
	else
		fnt = &Fnt[EngFontSelect];

	bytes = (fnt->Width * fnt->Height);
	fontbytes = bytes >> 3;							// / 8
	if (bytes & 0x7)								// % 8
		fontbytes += 1;

	fontaddr = (DWORD)fnt->Fnts + fontbytes * (wc - fnt->StartCode);
	is_load_success = FALSE;

	if (CodeSet == CS_ENG)
	{
		//if (((fontaddr + 1) >= (ROM_START + 1)) && ((fontaddr + fontbytes) <= (ROM_START + ROM_SIZE)))
		{
			memcpy((void *)Data, (void *)fontaddr, fontbytes);
			is_load_success = TRUE;
		}
	}
	else if (CodeSet == CS_GBK)
	{
		if ((fontaddr >= ADDR_GBK_FONT) && ((fontaddr + fontbytes) <= (ADDR_GBK_FONT + SIZE_OF_GBK_FONT)))
		{
			//Flash_Read(fontaddr, (void *)Data, fontbytes);
			//is_load_success = TRUE;
		}
	}

	if (!is_load_success)
	{
		memset((void *)Data, 0xff, fontbytes);

		assert_param(is_load_success);
		TRACEh(fontaddr);
		TRACEh(fontbytes);
	}
}

BYTE Font_GetPixel(WORD x, WORD y, WORD width, BYTE *data)
{
	DWORD pixels, bytes;
	BYTE mask;

	pixels = y * width + x;
	bytes = pixels / 8;
	mask = 1 << (7 - (pixels % 8));

	return (data[bytes] & mask);
}

#endif
