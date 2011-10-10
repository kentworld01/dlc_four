#ifndef LIST_H
#define LIST_H


//===========================================================================================
#if (MODULE_GUI_SYSTEM_LIST == 0)
#else

//===========================================================================================
typedef void (*F_LIST_ONDRAWITEM)(LIST *list, RECT *itemrect, DWORD itemindex);
typedef void (*F_LIST_ONSELECTION)(struct LIST *list, DWORD itemindex);
typedef void (*F_LIST_ONSELECTCHANGE)(struct LIST *list, DWORD itemindex);

//===========================================================================================
// style: WS_LIST_SHORTCUTKEY, WS_LIST_HORIZONTAL, WS_LIST_VERTICAL
//
bool List_Create(
			FORM *form,
			LIST *list,
			WORD sx, WORD sy,
			WORD width, WORD height,
			DWORD style,
			DWORD item_size,
			DWORD item_total,
			F_LIST_ONDRAWITEM draw_func,
			F_LIST_ONSELECTION selection_func,
			F_LIST_ONSELECTCHANGE selectchange_func
			);

void List_SetSelect(LIST *list, DWORD itemindex);

DWORD List_GetSelect(LIST *list);

DWORD List_GetTop(LIST *list);
DWORD List_GetTotal(LIST *list);

#endif

#endif
