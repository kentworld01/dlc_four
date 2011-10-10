//===========================================================================================
#include "Device.h"
#include "OS.h"
#include "GUI.h"


//===========================================================================================
#if (MODULE_GUI_SYSTEM_LIST == 0)
#else

//===========================================================================================
DWORD List_GetVisibleItemCount(LIST *list);
DWORD List_GetItemindex(LIST *list, DWORD offset_form_top);
void List_OnDraw(LIST *list);
BOOL List_Handler(LIST *list, EVENT *Event);

//===========================================================================================

BOOL List_Create(
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
			)
{
	memset((void *)list, 0x00, sizeof(LIST));

	list->rect.sx = sx;
	list->rect.sy = sy;
	list->rect.width = width;
	list->rect.height = height;

	list->vcobject.enable = 1;

	list->vcobject.handler =
		(BOOL (*)(VCOBJECT *, EVENT *))
			List_Handler;

	// 选择列表的垂直或水平方向排列风格
	if (style & WS_LIST_HORIZONTAL)
		list->vcobject.list_horizontal = 1;
	else if (style & WS_LIST_VERTICAL)
		list->vcobject.list_vertical = 1;
	else
		list->vcobject.list_vertical = 1;

	// 垂直排列的列表才允许显示及使用快捷键功能
	if ((list->vcobject.list_vertical)
			&& (style & WS_LIST_SHORTCUTKEY))
		list->vcobject.list_shortcutkey = 1;

	// 环绕选择风格
	if (style & WS_LIST_SELECTWRAP)
		list->vcobject.list_selectwrap = 1;

	list->drawfunc = draw_func;
	list->selection_func = selection_func;
	list->selectchange_func = selectchange_func;

	list->item_total = item_total;
	list->item_size = item_size;

	if (list->vcobject.list_horizontal)
		list->page_item_count = width / item_size;
	else if (list->vcobject.list_vertical)
		list->page_item_count = height / item_size;

	list->top_itemindex = 0;
	list->selected_item = 0;

	// 计算当前列表可见项目数量
	list->cur_page_item_count = (BYTE)List_GetVisibleItemCount(list);

	Form_AddObject(form, (VCOBJECT *)list);

	return TRUE;
}

DWORD List_GetVisibleItemCount(LIST *list)
{
	DWORD count;

	count = list->item_total - list->top_itemindex;
	if ((count > list->page_item_count)
			|| (list->item_total > list->page_item_count))
		count = list->page_item_count;

	return count;
}

DWORD List_GetItemindex(LIST *list, DWORD offset_form_top)
{
	DWORD itemindex;

	itemindex = list->top_itemindex + offset_form_top;

	// 如果选项超出范围，则回绕到最起始位置
	if (itemindex >= list->item_total)
		itemindex -= list->item_total;

	return itemindex;
}

void List_SetSelect(LIST *list, DWORD itemindex)
{
	list->top_itemindex = itemindex;
	list->selected_item = 0;
}

DWORD List_GetSelect(LIST *list)
{
	return List_GetItemindex(list, (DWORD)list->selected_item);
}

DWORD List_GetTop(LIST *list)
{
	return list->top_itemindex;
}

DWORD List_GetTotal(LIST *list)
{
	return list->item_total;
}

void List_InvertItem(LIST *list, BYTE item)
{
	WORD sx, sy, w, h;

	if (list->vcobject.list_vertical) {					// 垂直排列的列表
		sx = list->rect.sx;
		sy = list->rect.sy + item * list->item_size;
		w = list->rect.width;
		h = list->item_size;

	} else if (list->vcobject.list_horizontal) {		// 水平排列的列表
		sx = list->rect.sx + item * list->item_size;
		sy = list->rect.sy;
		w = list->item_size;
		h = list->rect.height;
	}

	Graph_InvertRectangle(sx, sy, w, h);

	list->vcobject.updated = 1;
}

void List_OnDraw(LIST *list)
{
	RECT ItemRect;
	BYTE i;

	//TRACE("List_OnDraw: vcobject = 0x%x\n", list);

	// 清除列表框内容显示区
	Graph_ClearRectangle(
			list->rect.sx,
			list->rect.sy,
			list->rect.width,
			list->rect.height);

	// 无列表可显示，返回
	if ((list->page_item_count == 0)
			|| (list->item_total == 0))
		return;

	// 计算当前列表可见项目数量
	list->cur_page_item_count = (BYTE)List_GetVisibleItemCount(list);

	ItemRect.sx = list->rect.sx;
	ItemRect.sy = list->rect.sy;

	for(i=0; i<list->cur_page_item_count; i++) {
		if (list->vcobject.list_vertical) {				// 垂直排列的列表
			ItemRect.sx = list->rect.sx;
			ItemRect.width = list->rect.width;
			ItemRect.height = list->item_size;
		} else if (list->vcobject.list_horizontal)	{	// 水平排列的列表
			ItemRect.sy = list->rect.sy;
			ItemRect.width = list->item_size;
			ItemRect.height = list->rect.height;
		}

		// 显示快捷键编号
		if (list->vcobject.list_shortcutkey)
		{
			char buf[4];
			WCHAR wcs[4];
			WORD wt, ht;

			s_sprintf(buf, "%1d.", (int)(i + 1));
			strtowstr(wcs, buf);

			wt = Text_GetDisplayWidth(wcs);
			ht = Text_GetDisplayHeight();

			Text_DisplayWString(
					ItemRect.sx,
					ItemRect.sy,
					wt, ht,
					wcs);

			ItemRect.sx += wt;
			ItemRect.width -= wt;
		}

		// 处理用户定义的列表项绘制函数
		if (list->drawfunc) {
			if ((!list->vcobject.list_selectwrap)										// 非环绕模式下
					&& (list->top_itemindex + i > list->item_total - 1))				// 列表范围之外的项目
				;
			else
				list->drawfunc(list, &ItemRect, List_GetItemindex(list, (DWORD)i));
		}

		// 定位到下一个列表项显示位置
		if (list->vcobject.list_vertical)				// 垂直排列的列表
			ItemRect.sy += list->item_size;
		else if (list->vcobject.list_horizontal)		// 水平排列的列表
			ItemRect.sx += list->item_size;
	}

	// 将当前选择项目反白显示
	List_InvertItem(list, list->selected_item);

	list->vcobject.updated = 1;
}

BOOL List_OnKeyPress(LIST *list, DWORD KeyCode)
{
	BOOL Eat = FALSE;
	BOOL HighlightMove = FALSE;
	BOOL ReloadList = FALSE;
	BYTE PrevSelected = list->selected_item;

	//TRACE("List_OnKeyPress(), KeyCode=0x%x\n", KeyCode);				// 调试代码

	if ((list->page_item_count == 0)
			|| (list->item_total == 0))
		return Eat;

	if ((KeyCode == KEY_ENTER)
			&& (list->selection_func)) {
		list->selection_func(list, List_GetSelect(list));
		Eat = TRUE;

	// 快捷键方式
	} else if (list->vcobject.list_shortcutkey
			&& (KeyCode >= '1')
			&& (KeyCode < '1' + list->cur_page_item_count)
			&& (list->selection_func)) {
		list->selected_item = (BYTE)(KeyCode - '1');
		List_InvertItem(list, PrevSelected);							// 翻转前一被选列
		List_InvertItem(list, list->selected_item);					// 翻转当前被选列
		list->selection_func(list, List_GetSelect(list));
		Eat = TRUE;

	} else if (KeyCode == KEY_UP) {
		if (list->selected_item > 0) {									// 反白项未到达显示列表顶端，反白项上移
			list->selected_item--;
			HighlightMove = TRUE;
		} else {
			if (list->top_itemindex > 0) {								// 表头未到达列表顶端
				list->top_itemindex--;
				ReloadList = TRUE;
			} else if (list->vcobject.list_selectwrap) {				// 表头到达列表顶端，且允许环绕
				list->top_itemindex = list->item_total - 1;
				ReloadList = TRUE;
			}
		}
		Eat = TRUE;

	} else if (KeyCode == KEY_DOWN) {
		if (list->selected_item < list->cur_page_item_count - 1) {		// 反白项未到达显示列表末端，反白项下移
			if (list->top_itemindex + list->selected_item < list->item_total - 1) {
				list->selected_item++;
				HighlightMove = TRUE;
			}
		} else	{
			if (list->vcobject.list_selectwrap) {						// 允许环绕
				if (list->top_itemindex < list->item_total - 1) {		// 表头未到达列表末端
					list->top_itemindex++;
					ReloadList = TRUE;
				} else {												// 表头到达列表末端，且允许环绕
					list->top_itemindex = 0;
					ReloadList = TRUE;
				}
			} else {													// 非环绕方式
				if (list->top_itemindex + list->cur_page_item_count - 1 < list->item_total - 1) {	// 表尾未到达列表末端
					list->top_itemindex++;
					ReloadList = TRUE;
				}
			}
		}
		Eat = TRUE;

	}  else if (KeyCode == KEY_PAGEUP) {
		if (list->selected_item > 0) {
			list->selected_item = 0;
			HighlightMove = TRUE;
		} else {
			list->top_itemindex -=
				(list->top_itemindex >= list->cur_page_item_count)
					? list->cur_page_item_count
						: list->top_itemindex;
			ReloadList = TRUE;
		}

	} else if (KeyCode == KEY_PAGEDOWN) {
		DWORD LastSelectedItem =
			(list->top_itemindex + list->cur_page_item_count - 1 <= list->item_total - 1)
				? (list->cur_page_item_count - 1)
					: (list->item_total - 1 - list->top_itemindex);
		if (list->selected_item != LastSelectedItem) {
			list->selected_item = LastSelectedItem;
			HighlightMove = TRUE;
		} else {
			list->top_itemindex +=
				(list->top_itemindex + list->selected_item + list->cur_page_item_count <= list->item_total - 1)
					? list->cur_page_item_count
						: (list->item_total - 1 - (list->top_itemindex + list->selected_item));
			ReloadList = TRUE;
		}
	}

	if (HighlightMove) {
		List_InvertItem(list, PrevSelected);							// 翻转前一被选列
		List_InvertItem(list, list->selected_item);					// 翻转当前被选列
	}

	// 列表内容变化，重绘
	if (ReloadList)
		List_OnDraw(list);

	// 通知用户选项已发生改变
	if ((HighlightMove || ReloadList) && list->selectchange_func)
		list->selectchange_func(list, List_GetSelect(list));

	return Eat;								// TRUE: 按键已被处理，FALSE: 按键未被处理
}

BOOL List_Handler(LIST *list, EVENT *Event)
{
	switch (Event->Type)
	{
	case EV_FORM_DRAW:
		List_OnDraw(list);
		break;

	case EV_KEYPRESS:
		return List_OnKeyPress(list, (WORD)Event->Para);

	case EV_APP_IDLE:
		break;
	}
	return FALSE;
}

#endif
