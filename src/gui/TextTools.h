#ifndef TEXTTOOLS_H
#define TEXTTOOLS_H


//===========================================================================================
#if (MODULE_GUI_SYSTEM_TEXTTOOLS == 0)
#else

WORD Text_GetDisplayWidth(WCHAR *string);

WORD Text_GetDisplayHeight(void);

WORD Text_DisplayWChar(WORD x, WORD y, WORD width, WORD height, WCHAR wc);

void Text_DisplayWString(WORD x, WORD y, WORD width, WORD height, WCHAR *string);

void Text_DisplayPassword(WORD x, WORD y, WORD width, WORD height, WCHAR *string);

#endif


#endif
