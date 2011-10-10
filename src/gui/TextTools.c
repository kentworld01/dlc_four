//===========================================================================================
#include "Device.h"
#include "GUI.h"
#include "OS.h"


//===========================================================================================
#if (MODULE_GUI_SYSTEM_TEXTTOOLS == 0)
#else

//===========================================================================================

WORD Text_GetDisplayWidth(WCHAR *string)
{
	WORD cs, w;

	if (!string)
		return 0;

	for(w=0; *string; string++) {
		cs = (*string >= 0x4e00) ? CS_GBK : CS_ENG;
		w += Font_GetFontWidth(cs);
	}

	return w;
}

WORD Text_GetDisplayHeight(void)
{
	return Font_GetFontHeight(CS_ENG);
}

WORD Text_DisplayWChar(WORD x, WORD y, WORD width, WORD height, WCHAR wc)
{
	BYTE FontData[16 * 16 / 8];
	WORD cs, fw, fh;
	WORD lx, ly, vx, vy, i, j;

	cs = (wc >= 0x4e00) ? CS_GBK : CS_ENG;
	fw = Font_GetFontWidth(cs);
	fh = Font_GetFontHeight(cs);
	Font_GetPixelData(cs, wc, FontData);

	lx = x + ((width < fw) ? width : fw);
	lx = (lx < LCD_WIDTH_LIMIT) ? lx : LCD_WIDTH_LIMIT;
	ly = y + ((height < fh) ? height : fh);
	ly = (ly < LCD_HEIGHT_LIMIT) ? ly : LCD_HEIGHT_LIMIT;
	for(vx=x,i=0; vx<lx; vx++,i++)
		for(vy=y,j=0; vy<ly; vy++,j++)
			Graph_SetPixel(vx, vy, Font_GetPixel(i, j, fw, FontData));
	return fw;
}

void Text_DisplayWString(WORD x, WORD y, WORD width, WORD height, WCHAR *string)
{
	WORD CharWidth;

	for(; *string && width; string++) {
		CharWidth = Text_DisplayWChar(x, y, width, height, *string);
		x += CharWidth;
		width = (width > CharWidth) ? (width - CharWidth) : 0;
	}
}

void Text_DisplayPassword(WORD x, WORD y, WORD width, WORD height, WCHAR *string)
{
	WORD CharWidth;

	for(; *string && width; string++) {
		CharWidth = Text_DisplayWChar(x, y, width, height, (WCHAR)'*');
		x += CharWidth;
		width = (width > CharWidth) ? (width - CharWidth) : 0;
	}
}

#endif
