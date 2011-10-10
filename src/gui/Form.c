//===========================================================================================
#include "Device.h"
#include "OS.h"
#include "GUI.h"


//===========================================================================================
#if (MODULE_GUI_SYSTEM_FORM == 0)
#else

//===========================================================================================

BOOL Form_ControlObjectHandler(FORM *form, EVENT *Event);

//===========================================================================================
//
FORM *__RootForm[MODULE_DEVICE_LCD_COUNT];
extern BYTE LCDPanelSelect;

//===========================================================================================
void Form_Init(void)
{
	DWORD i;

	for(i = 0; i < MODULE_DEVICE_LCD_COUNT; i++)
		__RootForm[i] = 0;
}

// ��������������ȡָ����ʾ�������Ĵ�������
//
DWORD Form_GetFormCountOfPanel(BYTE panel)
{
	FORM *form;
	DWORD count;

	if (panel < MODULE_DEVICE_LCD_COUNT)
	{
		for(count = 0, form = __RootForm[panel]; form; form = form->next)
			count++;
	}

	return count;
}

// ��������������ȡ���д�������
//
DWORD Form_GetFormCount()
{
	DWORD i, count;

	for(count = 0, i = 0; i < MODULE_DEVICE_LCD_COUNT; i++)
		count += Form_GetFormCountOfPanel(i);

	return count;
}

// ��������������ȡָ�����ڵ�ǰ�򴰿�
FORM *Form_GetPrevForm(FORM *form)
{
	FORM *prev_form;

	for(prev_form = __RootForm[LCDPanelSelect]; prev_form && prev_form->next; prev_form = prev_form->next)
		if (prev_form->next == form)
			return prev_form;

	return 0;
}

// ��������������ȡ���һ������
FORM *Form_GetLastForm(BYTE panel)
{
	FORM *form;

	for(form = __RootForm[panel]; form && form->next; form = form->next);

	return form;
}

// ��ָ������ʾ�����Ҵ���
FORM *Form_FindForm(BYTE panel, FORM *Find)
{
	FORM *form;

	for(form = __RootForm[panel]; form; form = form->next)
		if (Find == form)
			return form;

	return 0;
}

// ��������������ȡ��ǰ�����
FORM *Form_GetActiveForm(BYTE panel)
{
	return Form_GetLastForm(panel);
}

BOOL Form_IsAliveForm(FORM *form)
{
	BYTE panel;
	FORM *f;

	for(panel = 0; (panel < MODULE_DEVICE_LCD_COUNT); panel++)
	{
		f = Form_FindForm(panel, form);
		if (f)
			return TRUE;
	}

	return FALSE;
}

void Form_GetCaptionRect(FORM *form, RECT *captionrect)
{
	captionrect->sx = form->rect.sx;
	captionrect->sy = form->rect.sy;
	captionrect->width = form->rect.width;
	captionrect->height = 0;

	if (form->boardline) {
		captionrect->sx += 1;
		captionrect->sy += 1;
		captionrect->width -= 2;										// ȥ���߿���
	}

	if (form->title) {
		captionrect->height += Font_GetFontHeight(CS_GBK);			// ����߶�

		if (form->boardline)
			captionrect->height += 1;									// �м��

		if (form->titleline)
			captionrect->height += 1;									// �м��
	}
}

void Form_GetClientRect(FORM *form, RECT *clientrect)
{
	RECT CaptionRect;

	Form_GetCaptionRect(form, &CaptionRect);

	clientrect->sx = CaptionRect.sx;
	clientrect->sy = CaptionRect.sy + CaptionRect.height;
	clientrect->width = CaptionRect.width;
	clientrect->height = form->rect.height - CaptionRect.height;

	if (form->boardline)
		clientrect->height -= 2;

	if (form->titleline)
	{
		clientrect->sy += 1;
		clientrect->height -= 1;
	}
}

void Form_OnCreate(FORM *form)
{
	FORM *last_form;

	if (Form_FindForm(LCDPanelSelect, form))
		return;

	last_form = Form_GetLastForm(LCDPanelSelect);						// �´��ڼ��뵽��ǰ��ʾ������������
	if (last_form)
	{
		//TRACE("NEW FORM: %08x -> %08x\n", last_form, form);
		last_form->next = form;
	}
	else
	{
		//TRACE("ROOT FORM: %08x\n", form);
		__RootForm[LCDPanelSelect] = form;								// ����������
	}

	//TRACE("Form_OnCreate: LCDPanelSelect=%d, last_form:0x%x, form:0x%x, form->panel=%d\n", LCDPanelSelect, last_form, form, form->panel);
}

//ȷ��
const unsigned short str_ok[] = { 0x786e, 0x8ba4,  0x0000 };

//ȡ��
const unsigned short str_cancel[] = { 0x53d6, 0x6d88,  0x0000 };

void Form_OnDraw(FORM *form)
{
	RECT CaptionRect;
	RECT client;
	WORD wt, ht;
	WORD x, y, w, fh = Font_GetFontHeight(CS_GBK);

	if (form->boardline)
		Graph_Rectangle(							// ��ʾ���ڱ߿�
					form->rect.sx, form->rect.sy,
					form->rect.width, form->rect.height);

	if (form->title) {

		Form_GetCaptionRect(form, &CaptionRect);

		Graph_ClearRectangle(
				CaptionRect.sx,
				CaptionRect.sy,
				CaptionRect.width,
				CaptionRect.height);

		if (form->caption) {

			// ���ڱ��������ʾ
			wt = Text_GetDisplayWidth(form->caption);
			ht = Text_GetDisplayHeight();

			Text_DisplayWString(
					CaptionRect.sx + (CaptionRect.width - wt) / 2,
					CaptionRect.sy + ((form->boardline) ? 1 : 0),
					wt, ht,
					form->caption);
		}

		if (form->titleline)
			Graph_Line(							// ��ʾ���ڱ���߿�
					CaptionRect.sx,
					CaptionRect.sy + CaptionRect.height,
					CaptionRect.sx + CaptionRect.width - 1,
					CaptionRect.sy + CaptionRect.height);

		if (form->inverttitle)
			Graph_InvertRectangle(
					CaptionRect.sx,
					CaptionRect.sy,
					CaptionRect.width,
					CaptionRect.height);
	}

	Form_GetClientRect(form, &client);
	Graph_ClearRectangle(							// ����ͻ���ʾ��
				client.sx,
				client.sy,
				client.width,
				client.height);

	if (form->btnok) {
		w = Text_GetDisplayWidth((WCHAR *)str_ok);
		x = client.sx + 2;
		y = client.sy + (client.height - fh - 2);
		Text_DisplayWString(x, y, w, fh, (WCHAR *)str_ok);			//ȷ��
	}

	if (form->btncancel) {
		w = Text_GetDisplayWidth((WCHAR *)str_cancel);
		x = client.sx + (client.width - w - 2);
		y = client.sy + (client.height - fh - 2);
		Text_DisplayWString(x, y, w, fh, (WCHAR *)str_cancel);		//ȡ��
	}

#if (0)					// ���Դ���
	TRACE("FormRect: %d, %d, %d, %d\n",
						form->rect.sx,
						form->rect.sy,
						form->rect.width,
						form->rect.height);
	TRACE("CaptionRect: %d, %d, %d, %d\n",
						CaptionRect.sx,
						CaptionRect.sy,
						CaptionRect.width,
						CaptionRect.height);
	TRACE("client: %d, %d, %d, %d\n",
						client.sx,
						client.sy,
						client.width,
						client.height);
#endif

	form->updated = 1;								// ���ô���ˢ�±�־
}

void Form_OnIdleExit(FORM *form)
{
	if (form->timeout)
	{
		if (GetInterval(form->starttick) >= form->timeout)
			Form_Close(form);
	}
}

void Form_OnClose(FORM *form)
{
	FORM *prev_form, *f;
	EVENT ev;

	prev_form = Form_GetPrevForm(form);
	//TRACE("Form_OnClose, %08x -> %08x -> %08x\n", prev_form, form, form->next);

	if (prev_form)											// ��ǰһ�����ڴ���
	{
		prev_form->next = form->next;						// ��������ɾ��ָ������

		// ����ײ㴰����һ����ˢ������ʣ��Ĵ���
		// �˴�����ʹ����Ϣ��ʽˢ�£��������ڶ��ˢ�¶�������Ļ��˸
		for(f = __RootForm[LCDPanelSelect]; f; f = f->next)
		{
			Form_OnDraw(f);
			ev.Type = EV_FORM_DRAW;
			ev.Para = (DWORD)f;
			if (f->handler)
				f->handler(f, &ev);
			Form_ControlObjectHandler(f, &ev);
		}
		form->updated = 1;
	}
	else
	{
		__RootForm[LCDPanelSelect] = form->next;
		if (__RootForm[LCDPanelSelect] == 0)				// �޴��ڼ�����ʾ�������Ļ
		{
			Graph_ClearRectangle(0, 0, 128, 64);
			form->updated = 1;
			//TRACEs("CLEANUP");
		}
	}

	form->active = 0;										// ����ֹͣ���������Ӧ�¼����� 
}

BOOL _Form_Handler(FORM *form, EVENT *Event)
{
	BOOL b = FALSE;

	if (!form->active)
	{
		//TRACE("_Form_Handler(), form: %08x, event: %08x, EVENT LEAK!\n", form, Event->Type);
		return b;
	}

	if (Event->Type == EV_FORM_CREATE)
		Form_OnCreate(form);									// ���ڴ����¼�����

	else if (Event->Type == EV_FORM_DRAW)
		Form_OnDraw(form);										// ����Ĭ��ˢ������

	else if (Event->Type == EV_APP_IDLE)
		Form_OnIdleExit(form);								// �����Զ��˳�

	if (form->handler)
		b = form->handler(form, Event);						// �����¼��������

	if (Event->Type == EV_KEYPRESS)
	{
		Form_ResetIdleExitTime(form);						// �����¼������Զ��˳�������

		// ÿ�ΰ�����������һ�������߽��н��ա�����
		// ��Formδ��������Ϣ������Ϣ�ַ��������ؼ�
		if (!b)
		{
			b |= Form_ControlObjectHandler(form, Event);	// �������ڿؼ������¼�

			if ((WORD)Event->Para == KEY_CANCEL)				// ����Ĭ�ϵ��˳�����
				Form_Close(form);
		}
	}
	else
		b |= Form_ControlObjectHandler(form, Event);		// �������ڿؼ������¼�

	if (Event->Type == EV_FORM_CLOSE)
		Form_OnClose(form);									// ���ڹرպ�����

	if (form->updated) 											// ������ˢ�µ�FRAMEBUFFER
	{
		form->updated = 0;
		LCD_Refresh();
	}

	return b;
}

// ��ÿһ����ʾ���ֱ����Ӧ�Ĵ����¼�
// �������ʾ����Ϣ�ַ���������ʾ���л�
//
BOOL Form_Handler(EVENT *Event)
{
	FORM *form;
	BOOL b = FALSE;
	BYTE OldPanel, panel;

	OldPanel = LCD_GetPanelSelection();

	switch (Event->Type)
	{
	case EV_APP_IDLE:
		for(panel = 0; (panel < MODULE_DEVICE_LCD_COUNT); panel++)
		{
			form = Form_GetActiveForm(panel);							// ��Ϣ�ַ���ÿ����ʾ���Ļ����
			if (form)
			{
				LCD_SetPanelSelection(panel);
				b |= _Form_Handler(form, Event);
			}
		}
		break;

	case EV_KEYPRESS:
		panel = 0;														// ��������ʾ���ϴ��������¼�
		form = Form_GetActiveForm(panel);								// ���㴰��ӵ�����뽹��
		if (form)
		{
			//TRACE("EV_KEYPRESS: Form_GetLastForm:0x%x, form->panel=%d\n", form, form->panel);
			LCD_SetPanelSelection(panel);
			b = _Form_Handler(form, Event);
		}
		break;

	default:
		if ((Event->Type > EV_FORM) && (Event->Type < EV_FORM_END))
		{
			form = (FORM *)Event->Para;
			panel = form->panel;
			if (form)
			{
				LCD_SetPanelSelection(panel);
				b = _Form_Handler(form, Event);
			}
		}
	}

	LCD_SetPanelSelection(OldPanel);

	return b;
}

//===========================================================================================

BOOL Form_Create(
			FORM *form,
			WCHAR *caption,
			WORD sx, WORD sy,
			WORD width, WORD height,
			DWORD style,
			F_FORMPROC formproc,
			DWORD IdleExitTime
			)
{
	EVENT ev;

	if (Form_GetFormCount() >= MAX_FORMCOUNT)
		return FALSE;

	memset((void *)form, 0x00, sizeof(FORM));
	form->caption = caption;
	form->rect.sx = sx;
	form->rect.sy = sy;
	form->rect.width = width;
	form->rect.height = height;
	form->handler = formproc;
	form->panel = LCDPanelSelect;

	form->enable = 1;

	if (style & WS_BUTTON_OK)
		form->btnok = 1;

	if (style & WS_BUTTON_CANCEL)
		form->btncancel = 1;

	if (style & WS_BOARDLINE)
		form->boardline = 1;

	if (style & WS_TITLE)
	{
		form->title = 1;
		if (style & WS_TITLELINE)
			form->titleline = 1;
		if (style & WS_INVERTTITLE)
			form->inverttitle = 1;
	}

	ev.Type = EV_FORM_CREATE;							// ���ʹ�����Ϣ���½�����
	ev.Para = (DWORD)form;
	AddEvent(&ev);

	ev.Type = EV_FORM_DRAW;								// ��������Ϣ���½�����
	ev.Para = (DWORD)form;
	AddEvent(&ev);

	form->active = 1;
	Form_SetIdleExitTime(form, IdleExitTime);

	if (style & WS_MODAL)
		Form_DoModal(form);

	return TRUE;
}

void Form_DoModal(FORM *form)
{
	form->modal = 1;

	app_event_loop(EV_FORM_CLOSE);
}

void Form_Close(FORM *form)
{
	EVENT ev;

	if ((Form_IsAliveForm(form)) && (form->active))
	{
		ev.Type = EV_FORM_CLOSE;
		ev.Para = (DWORD)form;
		AddEvent(&ev);
	}
}

void Form_SetIdleExitTime(FORM *form, DWORD Timeout)
{
	form->timeout = Timeout;
	Form_ResetIdleExitTime(form);
}

void Form_ResetIdleExitTime(FORM *form)
{
	if (form->timeout)
		form->starttick = GetTickCount();
}

void Form_Update(FORM *form)
{
	form->updated = 1;						// updated��־����֪ͨFRAMEBUFFER����
}

void Form_ReDraw(FORM *form)
{
	EVENT ev;
	BYTE i;

	for(i = 0; (i < MODULE_DEVICE_LCD_COUNT); i++)
	{
		if (Form_GetActiveForm(i) == form)
		{
			ev.Type = EV_FORM_DRAW;				// ��������Ϣ���½�����
			ev.Para = (DWORD)form;
			AddEvent(&ev);
			break;
		}
	}
}

//===========================================================================================

VCOBJECT *Form_GetLastObject(FORM *form)
{
	VCOBJECT *vcobject;

	for(vcobject = form->firstobj; vcobject && vcobject->next; vcobject = vcobject->next);

	return vcobject;
}

void Form_AddObject(FORM *form, VCOBJECT *vcobject)
{
	VCOBJECT *lastobject = Form_GetLastObject(form);

	if (lastobject)
		lastobject->next = vcobject;
	else
		form->firstobj = vcobject;

	//TRACE("form->firstobj = 0x%x\n", vcobject);

	vcobject->next = 0;
	vcobject->updated = 0;
}

void Form_DelObject(FORM *form, VCOBJECT *vcobject)
{
	VCOBJECT *prevobj, *vobj;

	for(prevobj = 0, vobj = form->firstobj; vobj; prevobj = vobj, vobj = vobj->next) {
		if (vobj == vcobject) {
			if (prevobj)
				prevobj->next = vcobject->next;
			else
				form->firstobj = vcobject->next;
			break;
		}
	}
}

VCOBJECT *Form_FindObject(FORM *form, VCOBJECT *vcobject)
{
	VCOBJECT *vco;

	//TRACE("Form_FindObject: form = 0x%x, vcobject = 0x%x\n", form, vcobject);
	//TRACEx((void *)form, sizeof(FORM));

	if (form)
		for(vco = form->firstobj; vco; vco = vco->next) {
			//TRACE("vco = 0x%x\n", vco);
			if (vcobject == vco)
				return vco;
		}

	return (VCOBJECT *)0;
}

FORM *Object_GetParentForm(VCOBJECT *vcobject)
{
	BYTE panel = 0;
	FORM *form = 0;

	for(panel = 0; (panel < MODULE_DEVICE_LCD_COUNT); panel++)
		for(form = __RootForm[panel]; form; form = form->next)
			if (Form_FindObject(form, vcobject))
				return form;

	return (FORM *)0;
}

BOOL Form_ControlObjectHandler(FORM *form, EVENT *Event)
{
	VCOBJECT *vcobject;
	BOOL b = FALSE;

	for(vcobject = form->firstobj; vcobject; vcobject = vcobject->next)
	{
		switch (Event->Type)
		{
		case EV_KEYPRESS:
		case EV_FORM_DRAW:
		case EV_APP_IDLE:
			if (vcobject->handler)
			{
				//TRACE("Form_ControlObjectHandler: form = 0x%x, firstobj = 0x%x, vcobject = 0x%x\n", form, form->firstobj, vcobject);
				b = vcobject->handler(vcobject, Event);
			}

			if (vcobject->updated)
			{
				form->updated = 1;							// ���������
				vcobject->updated = 0;						// �����ѽ��ܣ����ԭ״̬
			}

			// ÿ�ΰ�����ֻ����һ���ؼ����н��ա�����
			if ((Event->Type == EV_KEYPRESS)
					&& b)
				return b;
			break;
		}
	}

	return b;
}

#endif
