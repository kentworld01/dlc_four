#ifndef EDITOR_H
#define EDITOR_H


//===========================================================================================
#if (MODULE_GUI_SYSTEM_EDITOR == 0)

#else

//===========================================================================================
// 若keyfilter返回TRUE，表示按键被忽略
bool Editor_Create(
			FORM *form,
			EDITOR *editor,
			WORD sx, WORD sy,
			WORD width, WORD height,
			DWORD style,
			WCHAR *inputbuffer, WORD maxinputchar,
			KEYFILTERPROC keyfilter
			);


#endif


#endif
