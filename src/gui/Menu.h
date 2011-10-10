#ifndef MENU_H
#define MENU_H


//===========================================================================================
#if (MODULE_GUI_SYSTEM_MENU == 0)

#else

//===========================================================================================
//

typedef struct
{
	WORD MenuID;
	const WCHAR *Caption;
} MENUITEM;

typedef struct MENU_t
{
	FORM form;
	LIST menulist;
	DWORD selection;
	MENUITEM *menuitemlist;
	void (*ondrawitem_func)(struct MENU_t *menu, RECT *itemrect, DWORD itemindex);
	void (*selection_func)(struct MENU_t *menu, DWORD itemindex);
	void (*on_close_func)(struct MENU_t *menu);
} MENU;

typedef void (*F_MENU_ONDRAWITEM)(MENU *menu, RECT *itemrect, DWORD itemindex);
typedef void (*F_MENU_ONSELECTION)(MENU *menu, DWORD itemindex);
typedef void (*F_MENU_ONCLOSE)(MENU *menu);

DWORD Menu_GetSelect(MENU *menu);
void Menu_SetSelect(MENU *menu, DWORD Selection);
DWORD Menu_SetModal(MENU *menu);
void menu_set_close_handler(MENU *menu, F_MENU_ONCLOSE on_close_func);

DWORD Menu_Create(MENU *menu, WCHAR *Title,
				WORD sx, WORD sy, WORD width, WORD height,
				DWORD style, DWORD RemainTime,
				MENUITEM *MenuItemList, DWORD MenuCount,
				int Selection,
				F_MENU_ONDRAWITEM ondrawitem_func,
				F_MENU_ONSELECTION selection_func
				);


#endif


#endif
