//===========================================================================================
#include "Device.h"
#include "OS.h"
#include "GUI.h"


//===========================================================================================
#if (MODULE_GUI_SYSTEM_DIALOGBOX == 0)
#else

//===========================================================================================
#if (0)
void MsgBox_OnDraw(MSGBOX *MsgBox)
{
	WORD sx1, sx2, sy1, sy2, wt1 = 0, wt2 = 0, ht1 = 0, ht2 = 0;
	WORD sx, sy, width, height;

	if (MsgBox->title)
	{
		wt1 = Text_GetDisplayWidth(MsgBox->title);
		ht1 = Text_GetDisplayHeight();
	}

	if (MsgBox->message)
	{
		wt2 = Text_GetDisplayWidth(MsgBox->message);
		ht2 = Text_GetDisplayHeight();
	}

	width = 4 + ((wt1 > wt2) ? wt1 : wt2);
	height = ht1 + ht2 + ((ht2) ? 4 : 0);
	sx = (LCD_WIDTH_LIMIT - width) / 2;
	sy = (LCD_HEIGHT_LIMIT - height) / 2;
	sx1 = (LCD_WIDTH_LIMIT - wt1) / 2;
	sy1 = sy;
	sx2 = (LCD_WIDTH_LIMIT - wt2) / 2;
	sy2 = sy + ht1 + 2;
	Graph_ClearRectangle(sx - 3, sy - 3, width + 6, height + 6);

	if (MsgBox->title)
	{
		Text_DisplayWString(sx1, sy1, wt1, ht1, MsgBox->title);
		Graph_InvertRectangle(sx, sy, width, ht1);
	}

	if (MsgBox->message)
	{
		Text_DisplayWString(sx2, sy2, wt2, ht2, MsgBox->message);
		Graph_Rectangle(sx, sy + ht1, width, ht2 + 4);
	}
}

BOOL MsgBox_Proc(MSGBOX *MsgBox, EVENT *Event)
{
	switch (Event->Type)
	{
	case EV_FORM_CREATE:
		MsgBox->starttickcount = GetTickCount();
		break;

	case EV_FORM_DRAW:
		MsgBox_OnDraw(MsgBox);
		break;

	case EV_APP_IDLE:
		if ((MsgBox->remaintime != 0)
				&& (GetInterval(MsgBox->starttickcount) > MsgBox->remaintime))
			Form_Close((FORM *)MsgBox);
		break;

	case EV_FORM_CLOSE:
		break;
	}

	return FALSE;
}

//MSGBOX MsgBox;
//MessageBox(&MsgBox, (WCHAR *)str_064, (WCHAR *)str_070, 8000);

void MessageBox(MSGBOX *MsgBox, WCHAR *Title, WCHAR *Message, DWORD RemainTime)
{
	memset((void *)MsgBox, 0x00, sizeof(MSGBOX));

	MsgBox->title = Title;
	MsgBox->message = Message;

	Form_Create(
		(FORM *)MsgBox, 0,
		64, 32, 2, 2,
		0,
		(BOOL (*)(FORM *, EVENT *))MsgBox_Proc, RemainTime);
}
#endif

//===========================================================================================
//
#if (1)
BOOL DialogBox_Proc(DIALOGBOX *dlg, EVENT *Event)
{
	DWORD KeyCode = Event->Para;

	switch (Event->Type) {
	case EV_FORM_CREATE:
		break;

	case EV_FORM_DRAW:
		break;

	case EV_APP_IDLE:
		break;

	case EV_KEYPRESS:
		switch (KeyCode) {
		case KEY_ENTER:
			dlg->exitcode = MBI_OK;
			Form_Close((FORM *)dlg);
			return TRUE;
		case KEY_CANCEL:
			dlg->exitcode = MBI_CANCEL;
			Form_Close((FORM *)dlg);
			return TRUE;
		}
		break;

	case EV_FORM_CLOSE:
		break;
	}

	return FALSE;
}

DWORD DialogBox_DoModal(
			DIALOGBOX *dlg, WCHAR *Title,
			WORD sx, WORD sy, WORD width,
			WORD height,
			DWORD style,
			DWORD RemainTime
			)
{
	dlg->exitcode = MBI_TIMEOUT;

	Form_Create(
		(FORM *)dlg, Title,
		sx, sy, width, height,
//		WS_TITLE | WS_TITLELINE | WS_BOARDLINE,
//		WS_TITLE | WS_INVERTTITLE | WS_BOARDLINE,
//		WS_TITLE | WS_TITLELINE | WS_INVERTTITLE | WS_BOARDLINE,
//		WS_TITLE | WS_BOARDLINE,
//		WS_TITLE | WS_TITLELINE,
		style | WS_MODAL | WS_BUTTON_OK | WS_BUTTON_CANCEL,
		(F_FORMPROC)DialogBox_Proc, RemainTime);

	return dlg->exitcode;
}
#endif

//===========================================================================================
//

BOOL InputEditor_KeyFilter(DWORD KeyCode)
{
	//return (!IsDigital(KeyCode));
	return FALSE;
}

BOOL InputDialog_Proc(INPUTDIALOG *dlg, EVENT *Event)
{
	DWORD KeyCode = Event->Para;

	if ((dlg->dialogproc)
			&& (dlg->dialogproc(dlg, Event)))
		return TRUE;

	switch (Event->Type)
	{
	case EV_FORM_CREATE:
		break;

	case EV_FORM_DRAW:
		break;

	case EV_APP_IDLE:
		break;

	case EV_KEYPRESS:
		switch (KeyCode)
		{
		case KEY_ENTER:
			if (dlg->inputer.vcobject.enable
					&& wstrlen(dlg->inputer.input_buffer))
			{
				dlg->exitcode = MBI_OK;
				Form_Close((FORM *)dlg);
			}
			break;

		case KEY_CANCEL:
			if (dlg->inputer.vcobject.enable
					&& wstrlen(dlg->inputer.input_buffer))
			{
				dlg->inputer.input_buffer[0] = 0;
				Form_ReDraw((FORM *)dlg);
			}
			else
			{
				dlg->exitcode = MBI_CANCEL;
				Form_Close((FORM *)dlg);
			}
			break;

		default:
			return FALSE;
		}
		return TRUE;

	case EV_FORM_CLOSE:
		break;
	}

	return FALSE;
}

DWORD InputDialog_DoModal(
			INPUTDIALOG *dlg, WCHAR *Title,
			WORD sx, WORD sy, WORD width, WORD height,
			DWORD style, DWORD RemainTime,
			WCHAR *inputbuffer, WORD maxinputchar, DWORD editorstyle,
			F_INPUT_DIALOG_PROC dialogproc
			)
{
	RECT client;
	WORD w;

	dlg->exitcode = MBI_TIMEOUT;
	dlg->dialogproc = dialogproc;

	Font_SelectEngFont(FT_ENG_ARIAL_B_8X12);

	Form_Create(
		(FORM *)dlg, Title,
		sx, sy, width, height,
		style,
		(F_FORMPROC)InputDialog_Proc, RemainTime);

	if (!(WS_NO_EDITOR & editorstyle))
	{
	    Font_SelectEngFont(FT_ENG_06X12);
		w = maxinputchar * Font_GetFontWidth(FT_ENG_06X12);

		Form_GetClientRect((FORM *)dlg, &client);
		Editor_Create(
				(FORM *)dlg, &dlg->inputer,
				client.sx + (client.width - w) / 2, client.sy + 2,
				w, 4 + 11,
				editorstyle,
				inputbuffer, maxinputchar,
				InputEditor_KeyFilter);
	}

	Form_DoModal((FORM *)dlg);

	return dlg->exitcode;
}

//===========================================================================================
//
void IPDialog_RuleCheck(IPDIALOG *dlg, BYTE Col)
{
	DWORD v;

	if (Col == 2 || Col == 6 || Col == 10 || Col == 14) {	
		v = s_atou(dlg->EditorBuffer_1);
		if (v > 255)
			strcpy(dlg->EditorBuffer_1, "255");

		v = s_atou(dlg->EditorBuffer_2);
		if (v > 255)
			strcpy(dlg->EditorBuffer_2, "255");

		v = s_atou(dlg->EditorBuffer_3);
		if (v > 255)
			strcpy(dlg->EditorBuffer_3, "255");

		v = s_atou(dlg->EditorBuffer_4);
		if (v > 255)
			strcpy(dlg->EditorBuffer_4, "255");
	}
}

void IPDialog_GetBufferPointer(IPDIALOG *dlg, BYTE Col, BYTE Keycode)
{
	char *p = 0;

	if (Col < 3) {
		p = &dlg->EditorBuffer_1[Col - 0];

	} else if ((Col >= 4) && (Col <= 6)) {
		p = &dlg->EditorBuffer_2[Col - 4];

	} else if ((Col >= 8) && (Col <= 10)) {
		p = &dlg->EditorBuffer_3[Col - 8];

	} else if ((Col >= 12) && (Col <= 14)) {
		p = &dlg->EditorBuffer_4[Col - 12];
	}

	if (Keycode == '+')		
		(*p) = ((*p) == '9') ? '0' : (*p) + 1;
	else if(Keycode == '-')		
		(*p) = ((*p) == '0') ? '9' : (*p) - 1;
	else if (IsDigital(Keycode))
		(*p) = Keycode;
	
	if (Col == 0 || Col == 4 || Col == 8 || Col == 12) {			//µÚÒ»Î»
		if ((*p) > '2')
			*p = '0'; 	
	}

	IPDialog_RuleCheck(dlg, Col);
}

BOOL IPDialog_Proc(IPDIALOG *dlg, EVENT *Event)
{
	MONOBMP bmp;
	RECT client;
	DWORD KeyCode = (DWORD)Event->Para;
	BYTE buf[8];

	Form_GetClientRect((FORM *)dlg, &client);

	switch (Event->Type) {
	case EV_FORM_CREATE:
		SetLong_BE(buf, dlg->IP);
		s_sprintf(dlg->EditorBuffer_1, "%03u", buf[0]);
		s_sprintf(dlg->EditorBuffer_2, "%03u", buf[1]);
		s_sprintf(dlg->EditorBuffer_3, "%03u", buf[2]);
		s_sprintf(dlg->EditorBuffer_4, "%03u", buf[3]);
		dlg->Col = 0;
		break;

	case EV_FORM_DRAW: {
		char buf[16];
		WCHAR wcs[16];
		WORD w;

		Font_SelectEngFont(FT_ENG_VT100_5X7);

		strtowstr(wcs, "***.***.***.***");
		w = Text_GetDisplayWidth(wcs);
		Text_DisplayWString(client.sx + 10, client.sy + 7, w, 7, (WCHAR *)wcs);

		MonoBmp_Load(&bmp, (BYTE *)((dlg->Col == 0) ? bmp_arrow_lf_w : bmp_arrow_lf_b));
		Graph_PutBitmap(client.sx + 3, client.sy + 22, 5, 9, &bmp);
		MonoBmp_Load(&bmp, (BYTE *)((dlg->Col == 14) ? bmp_arrow_rt_w : bmp_arrow_rt_b));
		Graph_PutBitmap(client.sx + 85, client.sy + 22, 5, 9, &bmp);

		s_sprintf(buf, "%s.%s.%s.%s", dlg->EditorBuffer_1, dlg->EditorBuffer_2, 
							dlg->EditorBuffer_3, dlg->EditorBuffer_4);
		strtowstr(wcs, buf);
		w = Text_GetDisplayWidth(wcs);
		Text_DisplayWString(client.sx + 10, client.sy + 23, w, 7, (WCHAR *)wcs);

		MonoBmp_Load(&bmp, (BYTE *)bmp_arrow_up_1_b);
		Graph_PutBitmap(client.sx + 8 + dlg->Col * 5, client.sy + 17, 7, 4, &bmp);
		MonoBmp_Load(&bmp, (BYTE *)bmp_arrow_dn_1_b);
		Graph_PutBitmap(client.sx + 8 + dlg->Col * 5, client.sy + 32, 7, 4, &bmp);

		Graph_InvertRectangle(client.sx + 9 + dlg->Col * 5, client.sy + 22, 5, 9);
		break;
	}

	case EV_FORM_CLOSE:
		break;

	case EV_KEYPRESS:
		if (KeyCode == KEY_CANCEL) {
			dlg->exitcode = MBI_CANCEL;
			Form_Close((FORM *)dlg);

		 } else if (KeyCode == KEY_ENTER) {
		 	buf[0] = s_atou(dlg->EditorBuffer_1);
		 	buf[1] = s_atou(dlg->EditorBuffer_2);
		 	buf[2] = s_atou(dlg->EditorBuffer_3);
		 	buf[3] = s_atou(dlg->EditorBuffer_4);
			dlg->IP = GetLong_BE(buf);
			dlg->exitcode = MBI_OK;
			Form_Close((FORM *)dlg);

		} else if (KeyCode == KEY_UP) {
			if (dlg->Col > 0) {
				dlg->Col--;
				if ((dlg->Col == 3) || (dlg->Col == 7)|| (dlg->Col == 11))
					dlg->Col--;
				Form_ReDraw((FORM *)dlg);
			}

		} else if (KeyCode == KEY_DOWN) {
			if (dlg->Col < 14) {
				dlg->Col++;
				if ((dlg->Col == 3) || (dlg->Col == 7)|| (dlg->Col == 11))
					dlg->Col++;
				Form_ReDraw((FORM *)dlg);
			}

		} else if (KeyCode == KEY_ADD) {
			IPDialog_GetBufferPointer(dlg, dlg->Col, '+');
			Form_ReDraw((FORM *)dlg);

		} else if (KeyCode == KEY_DEC ) {
			IPDialog_GetBufferPointer(dlg, dlg->Col, '-');
			Form_ReDraw((FORM *)dlg);

		} else if (IsDigital(KeyCode)) {
			IPDialog_GetBufferPointer(dlg, dlg->Col, KeyCode);
			if (dlg->Col < 14) {
				dlg->Col++;
				if ((dlg->Col == 3) || (dlg->Col == 7)|| (dlg->Col == 11))
					dlg->Col++;
			}
			Form_ReDraw((FORM *)dlg);

		} else
			return FALSE;

		return TRUE;

	case EV_APP_IDLE:
		break;
	}

	return FALSE;
}

DWORD IPDialog_DoModal(
			IPDIALOG *dlg, WCHAR *Title,
			WORD sx, WORD sy, WORD width, WORD height,
			DWORD style, DWORD RemainTime,
			DWORD *IP
			)
{
	dlg->exitcode = MBI_TIMEOUT;
	dlg->IP = *IP;

	Form_Create(
		(FORM *)dlg, Title,
		sx, sy, width, height,
		style | WS_MODAL,
		(F_FORMPROC)IPDialog_Proc, RemainTime);

	*IP = dlg->IP;

	return dlg->exitcode;
}

#endif
