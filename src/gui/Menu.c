//===========================================================================================
#include "Device.h"
#include "OS.h"
#include "GUI.h"


//===========================================================================================
#if (MODULE_GUI_SYSTEM_MENU == 0)
#else

void Menu_OnSelection(LIST *list, DWORD itemindex)
{
	MENU *ParentForm = (MENU *)Object_GetParentForm((VCOBJECT *)list);

	ParentForm->selection = itemindex;

	if (ParentForm->selection_func)
		ParentForm->selection_func(ParentForm, itemindex);

	else
		Form_Close((FORM *)ParentForm);
}

void Menu_OnDrawItem(LIST *list, RECT *itemrect, DWORD itemindex)
{
	MENU *ParentForm = (MENU *)Object_GetParentForm((VCOBJECT *)list);

	if (ParentForm->ondrawitem_func)
		ParentForm->ondrawitem_func(ParentForm, itemrect, itemindex);

	else {
		Font_SelectEngFont(FT_ENG_VT100_5X7);
		Font_SelectEngFont(FT_ENG_06X12);
		Text_DisplayWString(itemrect->sx, itemrect->sy, itemrect->width, itemrect->height, (WCHAR *)ParentForm->menuitemlist[itemindex].Caption);
	}
}

DWORD Menu_GetSelect(MENU *menu)
{
	return List_GetSelect(&menu->menulist);
}

void Menu_SetSelect(MENU *menu, DWORD Selection)
{
	if (Selection < List_GetTotal(&menu->menulist)) {
		List_SetSelect(&menu->menulist, Selection);
		Form_ReDraw((FORM *)menu);
	}
}

DWORD Menu_SetModal(MENU *menu)
{
	Form_DoModal((FORM *)menu);

	return menu->selection;
}

BOOL menu_default_proc(FORM *form, EVENT *Event)
{
	MENU *menu = (MENU *)form;

	switch (Event->Type)
	{
	case EV_FORM_CREATE:
		break;

	case EV_FORM_DRAW:
		break;

	case EV_FORM_CLOSE:
		if (menu->on_close_func)
			menu->on_close_func(menu);
		break;

	case EV_KEYPRESS:
		break;

	case EV_APP_IDLE:
		break;
	}

	return FALSE;
}

void menu_set_close_handler(MENU *menu, F_MENU_ONCLOSE on_close_func)
{
	menu->on_close_func = on_close_func;
}

DWORD Menu_Create(MENU *menu, WCHAR *Title,
				WORD sx, WORD sy, WORD width, WORD height,
				DWORD style, DWORD RemainTime,
				MENUITEM *MenuItemList, DWORD MenuCount,
				int Selection,
				F_MENU_ONDRAWITEM ondrawitem_func,
				F_MENU_ONSELECTION selection_func
				)
{
	RECT client;
	WORD fh = Font_GetFontHeight(CS_GBK);

	menu->selection = -1;
	menu->menuitemlist = MenuItemList;
	menu->ondrawitem_func = ondrawitem_func;
	menu->selection_func = selection_func;
	menu->on_close_func = 0;

	Form_Create(
		(FORM *)menu, Title,
		sx, sy, width, height,
		style & ~WS_MODAL,
		(F_FORMPROC)menu_default_proc, RemainTime);

	Form_GetClientRect((FORM *)menu, &client);
	List_Create(
		(FORM *)menu, &menu->menulist,
		client.sx + 1, client.sy + 1,
		client.width - 2, client.height - 2,
		WS_LIST_VERTICAL,
		fh + 2, MenuCount,
		Menu_OnDrawItem,
		Menu_OnSelection,
		0);

	//TRACE("MenuList = 0x%x\n", &MenuList);
	//TRACE("ParentForm = 0x%x\n", ParentForm);
	//TRACEx((void *)ParentForm, sizeof(FORM));

	if ((Selection >= 0) && (Selection < MenuCount))
		Menu_SetSelect(menu, Selection);

	if (style & WS_MODAL)
		Form_DoModal((FORM *)menu);

	return menu->selection;
}

#endif
