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

	// ѡ���б�Ĵ�ֱ��ˮƽ�������з��
	if (style & WS_LIST_HORIZONTAL)
		list->vcobject.list_horizontal = 1;
	else if (style & WS_LIST_VERTICAL)
		list->vcobject.list_vertical = 1;
	else
		list->vcobject.list_vertical = 1;

	// ��ֱ���е��б��������ʾ��ʹ�ÿ�ݼ�����
	if ((list->vcobject.list_vertical)
			&& (style & WS_LIST_SHORTCUTKEY))
		list->vcobject.list_shortcutkey = 1;

	// ����ѡ����
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

	// ���㵱ǰ�б�ɼ���Ŀ����
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

	// ���ѡ�����Χ������Ƶ�����ʼλ��
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

	if (list->vcobject.list_vertical) {					// ��ֱ���е��б�
		sx = list->rect.sx;
		sy = list->rect.sy + item * list->item_size;
		w = list->rect.width;
		h = list->item_size;

	} else if (list->vcobject.list_horizontal) {		// ˮƽ���е��б�
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

	// ����б��������ʾ��
	Graph_ClearRectangle(
			list->rect.sx,
			list->rect.sy,
			list->rect.width,
			list->rect.height);

	// ���б����ʾ������
	if ((list->page_item_count == 0)
			|| (list->item_total == 0))
		return;

	// ���㵱ǰ�б�ɼ���Ŀ����
	list->cur_page_item_count = (BYTE)List_GetVisibleItemCount(list);

	ItemRect.sx = list->rect.sx;
	ItemRect.sy = list->rect.sy;

	for(i=0; i<list->cur_page_item_count; i++) {
		if (list->vcobject.list_vertical) {				// ��ֱ���е��б�
			ItemRect.sx = list->rect.sx;
			ItemRect.width = list->rect.width;
			ItemRect.height = list->item_size;
		} else if (list->vcobject.list_horizontal)	{	// ˮƽ���е��б�
			ItemRect.sy = list->rect.sy;
			ItemRect.width = list->item_size;
			ItemRect.height = list->rect.height;
		}

		// ��ʾ��ݼ����
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

		// �����û�������б�����ƺ���
		if (list->drawfunc) {
			if ((!list->vcobject.list_selectwrap)										// �ǻ���ģʽ��
					&& (list->top_itemindex + i > list->item_total - 1))				// �б�Χ֮�����Ŀ
				;
			else
				list->drawfunc(list, &ItemRect, List_GetItemindex(list, (DWORD)i));
		}

		// ��λ����һ���б�����ʾλ��
		if (list->vcobject.list_vertical)				// ��ֱ���е��б�
			ItemRect.sy += list->item_size;
		else if (list->vcobject.list_horizontal)		// ˮƽ���е��б�
			ItemRect.sx += list->item_size;
	}

	// ����ǰѡ����Ŀ������ʾ
	List_InvertItem(list, list->selected_item);

	list->vcobject.updated = 1;
}

BOOL List_OnKeyPress(LIST *list, DWORD KeyCode)
{
	BOOL Eat = FALSE;
	BOOL HighlightMove = FALSE;
	BOOL ReloadList = FALSE;
	BYTE PrevSelected = list->selected_item;

	//TRACE("List_OnKeyPress(), KeyCode=0x%x\n", KeyCode);				// ���Դ���

	if ((list->page_item_count == 0)
			|| (list->item_total == 0))
		return Eat;

	if ((KeyCode == KEY_ENTER)
			&& (list->selection_func)) {
		list->selection_func(list, List_GetSelect(list));
		Eat = TRUE;

	// ��ݼ���ʽ
	} else if (list->vcobject.list_shortcutkey
			&& (KeyCode >= '1')
			&& (KeyCode < '1' + list->cur_page_item_count)
			&& (list->selection_func)) {
		list->selected_item = (BYTE)(KeyCode - '1');
		List_InvertItem(list, PrevSelected);							// ��תǰһ��ѡ��
		List_InvertItem(list, list->selected_item);					// ��ת��ǰ��ѡ��
		list->selection_func(list, List_GetSelect(list));
		Eat = TRUE;

	} else if (KeyCode == KEY_UP) {
		if (list->selected_item > 0) {									// ������δ������ʾ�б��ˣ�����������
			list->selected_item--;
			HighlightMove = TRUE;
		} else {
			if (list->top_itemindex > 0) {								// ��ͷδ�����б���
				list->top_itemindex--;
				ReloadList = TRUE;
			} else if (list->vcobject.list_selectwrap) {				// ��ͷ�����б��ˣ���������
				list->top_itemindex = list->item_total - 1;
				ReloadList = TRUE;
			}
		}
		Eat = TRUE;

	} else if (KeyCode == KEY_DOWN) {
		if (list->selected_item < list->cur_page_item_count - 1) {		// ������δ������ʾ�б�ĩ�ˣ�����������
			if (list->top_itemindex + list->selected_item < list->item_total - 1) {
				list->selected_item++;
				HighlightMove = TRUE;
			}
		} else	{
			if (list->vcobject.list_selectwrap) {						// ������
				if (list->top_itemindex < list->item_total - 1) {		// ��ͷδ�����б�ĩ��
					list->top_itemindex++;
					ReloadList = TRUE;
				} else {												// ��ͷ�����б�ĩ�ˣ���������
					list->top_itemindex = 0;
					ReloadList = TRUE;
				}
			} else {													// �ǻ��Ʒ�ʽ
				if (list->top_itemindex + list->cur_page_item_count - 1 < list->item_total - 1) {	// ��βδ�����б�ĩ��
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
		List_InvertItem(list, PrevSelected);							// ��תǰһ��ѡ��
		List_InvertItem(list, list->selected_item);					// ��ת��ǰ��ѡ��
	}

	// �б����ݱ仯���ػ�
	if (ReloadList)
		List_OnDraw(list);

	// ֪ͨ�û�ѡ���ѷ����ı�
	if ((HighlightMove || ReloadList) && list->selectchange_func)
		list->selectchange_func(list, List_GetSelect(list));

	return Eat;								// TRUE: �����ѱ�����FALSE: ����δ������
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
