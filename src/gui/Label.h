#ifndef LABED_H
#define LABED_H

//===========================================================================================
#if (MODULE_GUI_SYSTEM_LABEL == 0)
#else

//===========================================================================================

typedef struct LABEL
{
	VCOBJECT vcobject;
	RECT rect;
	WCHAR *output_buffer;
} LABEL;

void Static_Draw(
			WORD sx, WORD sy,
			WORD width, WORD height,
			DWORD style,
			WCHAR *Title,
			WCHAR *String
			);

bool Label_Create(
			FORM *form,
			LABEL *label,
			WORD sx, WORD sy,
			WORD width, WORD height,
			DWORD style,
			WCHAR *output_buffer
			);

void Label_Update(LABEL *label);


#endif


#endif
