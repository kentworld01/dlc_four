#ifndef GUI_H
#define GUI_H

#include "os/typedef.h"

//===========================================================================================
#if (MODULE_GUI_SYSTEM == 0)

#define GUI_Init()					

#undef MODULE_GUI_SYSTEM_FONT
#undef MODULE_GUI_SYSTEM_BITMAP
#undef MODULE_GUI_SYSTEM_TEXTTOOLS
#undef MODULE_GUI_SYSTEM_GRAPHTOOLS
#undef MODULE_GUI_SYSTEM_FORM
#undef MODULE_GUI_SYSTEM_DIALOGBOX
#undef MODULE_GUI_SYSTEM_LABEL
#undef MODULE_GUI_SYSTEM_LIST
#undef MODULE_GUI_SYSTEM_EDITOR
#undef MODULE_GUI_SYSTEM_MENU

#define MODULE_GUI_SYSTEM_FONT		(0)
#define MODULE_GUI_SYSTEM_BITMAP		(0)
#define MODULE_GUI_SYSTEM_TEXTTOOLS	(0)
#define MODULE_GUI_SYSTEM_GRAPHTOOLS	(0)
#define MODULE_GUI_SYSTEM_FORM		(0)
#define MODULE_GUI_SYSTEM_DIALOGBOX	(0)
#define MODULE_GUI_SYSTEM_LABEL		(0)
#define MODULE_GUI_SYSTEM_LIST		(0)
#define MODULE_GUI_SYSTEM_EDITOR		(0)
#define MODULE_GUI_SYSTEM_MENU		(0)

#else

//===========================================================================================
void GUI_Init(void);

#endif


//===========================================================================================
#include "Typedef.h"


//===========================================================================================

#define WS_TITLE					((DWORD)(1 << 0))
#define WS_TITLELINE				((DWORD)(1 << 1))
#define WS_INVERTTITLE				((DWORD)(1 << 2))
#define WS_BOARDLINE				((DWORD)(1 << 3))
#define WS_UNDERLINE				((DWORD)(1 << 4))
#define WS_ALIGN_HMIDDLE			((DWORD)(1 << 5))
#define WS_ALIGN_LEFT				((DWORD)(1 << 6))
#define WS_ALIGN_RIGHT				((DWORD)(1 << 7))
#define WS_ALIGN_VMIDDLE			((DWORD)(1 << 8))
#define WS_ALIGN_TOP				((DWORD)(1 << 9))
#define WS_ALIGN_BOTTOM			((DWORD)(1 << 10))
#define WS_INVERT					((DWORD)(1 << 11))
#define WS_LIST_SHORTCUTKEY		((DWORD)(1 << 12))
#define WS_LIST_HORIZONTAL		((DWORD)(1 << 13))
#define WS_LIST_VERTICAL			((DWORD)(1 << 14))
#define WS_LIST_SELECTWRAP		((DWORD)(1 << 15))
#define WS_MODAL					((DWORD)(1 << 16))
#define WS_BUTTON_OK				((DWORD)(1 << 17))
#define WS_BUTTON_CANCEL			((DWORD)(1 << 18))
#define WS_PASSWORD				((DWORD)(1 << 19))
#define WS_IME_LETTER				((DWORD)(1 << 20))
#define WS_IME_DIGITAL				((DWORD)(1 << 21))
#define WS_NO_EDITOR				((DWORD)(1 << 22))


//===========================================================================================
typedef bool (*KEYFILTERPROC)(DWORD KeyCode);

typedef struct
{
	BYTE sx;
	BYTE sy;
	BYTE width;
	BYTE height;
} RECT;

typedef struct VCOBJECT
{
	struct VCOBJECT *next;

	bool (*handler)(struct VCOBJECT *, void *);

	bit enable:					1;
	bit updated:				1;
	bit boardline:				1;
	bit underline:				1;
	bit password:				1;
	bit align_hmiddle:			1;
	bit align_left:				1;
	bit align_right:			1;
	bit align_vmiddle:			1;
	bit align_top:				1;
	bit align_bottom:			1;
	bit invert:					1;
	bit list_shortcutkey:		1;
	bit list_horizontal:		1;
	bit list_vertical:			1;
	bit list_selectwrap:		1;
	bit ime_letter_enable:	1;
	bit ime_digital_enable:	1;
} VCOBJECT;

typedef struct FORM
{
	RECT rect;

	WCHAR *caption;
	DWORD starttick;
	DWORD timeout;

	bool (*handler)(struct FORM *, void *);

	struct FORM *next;
	VCOBJECT *firstobj;
	
	bit enable:					1;
	bit updated:				1;
	bit modal:					1;
	bit title:					1;
	bit titleline:				1;
	bit inverttitle:			1;
	bit boardline:				1;
	bit btnok:					1;
	bit btncancel:				1;
	bit active:					1;
//	BIT dummy:30;

	BYTE panel;
} FORM;

typedef struct LIST
{
	VCOBJECT vcobject;
	RECT rect;

	void (*drawfunc)(struct LIST *list, RECT *itemrect, DWORD itemindex);
	void (*selection_func)(struct LIST *list, DWORD itemindex);
	void (*selectchange_func)(struct LIST *list, DWORD itemindex);

	DWORD item_total;
	DWORD top_itemindex;

	BYTE item_size;
	BYTE page_item_count;
	BYTE selected_item;
	BYTE cur_page_item_count;
} LIST;

typedef struct EDITOR
{
	VCOBJECT vcobject;
	RECT rect;
	WORD cursortogglecnt;
	WORD maxinputchar;
	WCHAR *input_buffer;
	KEYFILTERPROC keyfilter;
	BYTE ime_mode;
	BYTE ime_input_timeout;
	BYTE ime_prev_key;
} EDITOR;


//===========================================================================================
// GUI
#include "Bitmap.h"
#include "Font.h"
#include "GraphTools.h"
#include "TextTools.h"
#include "Form.h"
#include "Menu.h"
#include "Editor.h"
#include "Label.h"
#include "List.h"
#include "DialogBox.h"

#include "resource\gui_resource.h"


#endif
