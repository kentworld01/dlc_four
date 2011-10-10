//===========================================================================================
#include "Device.h"
#include "OS.h"
#include "GUI.h"


//===========================================================================================
#if (MODULE_GUI_SYSTEM_EDITOR == 0)
#else

//===========================================================================================
BOOL Editor_Handler(EDITOR *editor, EVENT *Event);

//===========================================================================================
typedef enum
{
	ITP_DIGITAL					= 0,
	ITP_LETTER_UP,
	ITP_LETTER_LOW,
	ITP_END
} IME_TYPE;

/*
����123  
Ӣ��ABC  
Ӣ��abc  
����,./  
*/

//����123  
static const unsigned short str_123[] = { 0x6570, 0x5b57, 0x0031, 0x0032, 0x0033, 0x0020, 0x0020,  0x0000 };

//Ӣ��ABC  
static const unsigned short str_ABC[] = { 0x82f1, 0x6587, 0x0041, 0x0042, 0x0043, 0x0020, 0x0020,  0x0000 };

//Ӣ��abc  
static const unsigned short str_abc[] = { 0x82f1, 0x6587, 0x0061, 0x0062, 0x0063, 0x0020, 0x0020,  0x0000 };

//����: ,./
static const unsigned short str_alpha[] = { 0x7b26, 0x53f7, 0x002c, 0x002e, 0x002f, 0x0020, 0x0020,  0x0000 };

//===========================================================================================

BOOL Editor_Create(
			FORM *form,
			EDITOR *editor,
			WORD sx, WORD sy,
			WORD width, WORD height,
			DWORD style,
			WCHAR *inputbuffer, WORD maxinputchar,
			KEYFILTERPROC keyfilter
			)
{
	memset((void *)editor, 0x00, sizeof(EDITOR));

	editor->rect.sx = sx;
	editor->rect.sy = sy;
	editor->rect.width = width;
	editor->rect.height = height;

	editor->vcobject.handler =
		(BOOL (*)(VCOBJECT *, EVENT *))
			Editor_Handler;

	editor->keyfilter = keyfilter;
	editor->input_buffer = inputbuffer;
	editor->maxinputchar = ((width / 6) > maxinputchar) ? maxinputchar : (width / 6);
	editor->cursortogglecnt = 0;

	editor->vcobject.enable = 1;

	if (style & WS_PASSWORD)
		editor->vcobject.password = 1;

	// ���������ʽ��Ĭ���ޱ߿�
	if (style & WS_BOARDLINE)
		editor->vcobject.boardline = 1;
	else if (style & WS_UNDERLINE)
		editor->vcobject.underline = 1;

	// ����ˮƽ���룬Ĭ���������
	if (style & WS_ALIGN_HMIDDLE)
		editor->vcobject.align_hmiddle = 1;
	else if (style & WS_ALIGN_LEFT)
		editor->vcobject.align_left = 1;
	else if (style & WS_ALIGN_RIGHT)
		editor->vcobject.align_right = 1;
	else
		editor->vcobject.align_left = 1;

	// ���ô�ֱ���룬Ĭ�����϶���
	if (style & WS_ALIGN_VMIDDLE)
		editor->vcobject.align_vmiddle = 1;
	else if (style & WS_ALIGN_TOP)
		editor->vcobject.align_top = 1;
	else if (style & WS_ALIGN_BOTTOM)
		editor->vcobject.align_bottom = 1;
	else
		editor->vcobject.align_top = 1;

	if (WS_IME_LETTER & style)
	{
		editor->vcobject.ime_letter_enable = 1;
		editor->ime_mode = ITP_LETTER_UP;
	}

	if (WS_IME_DIGITAL & style)
	{
		editor->vcobject.ime_digital_enable = 1;
		editor->ime_mode = ITP_DIGITAL;
	}

	if ((WS_IME_DIGITAL & style) || (WS_IME_LETTER & style))
		editor->rect.height += 13;

	// �����ı�������ʾ
	if (style & WS_INVERT)
		editor->vcobject.invert = 1;

	Form_AddObject(form, (VCOBJECT *)editor);

	return TRUE;
}

void Editor_GetOutputRect(EDITOR *editor, RECT *outputrect, RECT *textrect)
{
	outputrect->sx = editor->rect.sx;
	outputrect->sy = editor->rect.sy;
	outputrect->width = editor->rect.width;
	outputrect->height = editor->rect.height;

	if (editor->vcobject.boardline)
	{
		outputrect->sx += 1;
		outputrect->sy += 1;
		outputrect->width -= 2;
		outputrect->height -= 2;
	}
	else if (editor->vcobject.underline)
		outputrect->height -= 1;

	textrect->sx = outputrect->sx;
	textrect->sy = outputrect->sy;
	textrect->width = Text_GetDisplayWidth(editor->input_buffer);
	textrect->height = Text_GetDisplayHeight();

	if (editor->vcobject.ime_letter_enable || editor->vcobject.ime_digital_enable)
		textrect->sy += 13;

	// ��������ı�ˮƽ�������λ��
	if (editor->vcobject.align_hmiddle) {
		textrect->sx += (outputrect->width - textrect->width) / 2;
		if (textrect->width + 6 <= outputrect->width)
			textrect->sx -= 6 / 2;

	} else if (editor->vcobject.align_right) {
		textrect->sx += (outputrect->width - textrect->width);
		if (textrect->width + 6 <= outputrect->width)
			textrect->sx -= 6;
	}

	// ��������ı���ֱ�������λ��
	if (editor->vcobject.align_vmiddle)
		textrect->sy += (outputrect->height - textrect->height) / 2;

	else if (editor->vcobject.align_bottom)
		textrect->sy += (outputrect->height - textrect->height);
}

void Editor_ShowCursor(EDITOR *editor)
{
	int len;
	RECT client, TextRect, CursorRect;

	if (!editor->input_buffer)
		return;
	len = wstrlen(editor->input_buffer);
	if (len >= editor->maxinputchar)
		return;

	if (++editor->cursortogglecnt >= 10)
		editor->cursortogglecnt = 0;						// ���ù����˸״̬

	if ((editor->cursortogglecnt != 1)
			&& (editor->cursortogglecnt != 6))
		return;

	// ����EDITOR���ԣ�������������ı�����λ��
	Editor_GetOutputRect(editor, &client, &TextRect);
	CursorRect.sx = TextRect.sx + TextRect.width + 1;
	CursorRect.sy = TextRect.sy + 7;
	CursorRect.width = 4;
	CursorRect.height = 4;

	
	if (editor->cursortogglecnt == 1) {					// �����ʾ��500ms
		Graph_FillRectangle(CursorRect.sx,
					CursorRect.sy,
					CursorRect.width,
					CursorRect.height);

	} else if (editor->cursortogglecnt == 6) {				// ���������500ms
		Graph_ClearRectangle(CursorRect.sx,
					CursorRect.sy,
					CursorRect.width,
					CursorRect.height);
	}

	editor->vcobject.updated = 1;
}

void Editor_OnDraw(EDITOR *editor)
{
	RECT client, TextRect;

	Font_SelectEngFont(FT_ENG_06X12);

	// ����EDITOR���ԣ�������������ı�����λ��
	Editor_GetOutputRect(editor, &client, &TextRect);

 	if (editor->vcobject.boardline)
 	{
		// ������߿�
		Graph_Rectangle(editor->rect.sx,
					editor->rect.sy,
					editor->rect.width,
					editor->rect.height);
	}
	else if (editor->vcobject.underline)
	{
		// �����»���
		Graph_Line(editor->rect.sx,
					editor->rect.sy + editor->rect.height - 1,
					editor->rect.sx + editor->rect.width - 1,
					editor->rect.sy + editor->rect.height - 1);
	}

	// ����ı���ʾ����
	Graph_ClearRectangle(client.sx,
				client.sy,
				client.width,
				client.height);

	if (editor->vcobject.password)
	{
		// ����ģʽ���'*'�ַ�
		Text_DisplayPassword(TextRect.sx,
					TextRect.sy,
					TextRect.width,
					TextRect.height,
					editor->input_buffer);
	}
	else
	{
		// �����ʾ�ı�
		Text_DisplayWString(TextRect.sx,
					TextRect.sy,
					TextRect.width,
					TextRect.height,
					editor->input_buffer);
	}

	if (editor->vcobject.ime_letter_enable || editor->vcobject.ime_digital_enable)
	{
		WCHAR *wcs;

		switch (editor->ime_mode)
		{
		case ITP_LETTER_UP:		wcs = (WCHAR *)str_ABC; break;
		case ITP_LETTER_LOW:	wcs = (WCHAR *)str_abc; break;
		default:
		case ITP_DIGITAL:		wcs = (WCHAR *)str_123; break;
		}

		// ���뷨��ʾ
		Text_DisplayWString(editor->rect.sx,
					editor->rect.sy,
					editor->rect.width,
					13,
					wcs);
		Graph_InvertRectangle(
				editor->rect.sx,
				editor->rect.sy,
				editor->rect.width,
				13);
	}

	if (editor->vcobject.invert)
	{
		// �����ı�������ʾ
		Graph_InvertRectangle(
				TextRect.sx,
				TextRect.sy,
				TextRect.width,
				TextRect.height);
	}

	// ���ƹ��
	Editor_ShowCursor(editor);

	editor->vcobject.updated = 1;
}

WCHAR editor_key_to_letter(EDITOR *editor, WCHAR current_letter, DWORD key)
{
	static const char digitalkey_to_letter_table[10][5] = 
	{
		"",
		"",
		"abc",
		"def",
		"ghi",
		"jkl",
		"mno",
		"pqrs",
		"tuv",
		"wxyz"
	};
	BYTE letter;
	char *group;

	switch (editor->ime_mode)
	{
	case ITP_LETTER_UP:
	case ITP_LETTER_LOW:
		group = (char *)digitalkey_to_letter_table[(BYTE)key - (BYTE)'0'];
		letter = group[0];

		if (current_letter && (editor->ime_prev_key == key))
		{
			current_letter = s_lowcase(current_letter);
			if ((current_letter - letter) < (strlen(group) - 1))
				letter = current_letter + 1;
		}

		if (letter)
		{
			if (ITP_LETTER_UP == editor->ime_mode)
				letter = letter - 'a' + 'A';
		}
		break;

	case ITP_DIGITAL:
	default:
		return (WCHAR)key;
	}

	return (WCHAR)letter;
}

void editor_reset_ime_roll(EDITOR *editor)
{
	editor->ime_input_timeout = 0;
	editor->ime_prev_key = 0;
}

BOOL Editor_OnKeyPress(EDITOR *editor, DWORD KeyCode)
{
	int len;
	WCHAR input, wcspoint[4] = { '.', 0 };

	//TRACE("Editor_OnKeyPress, Key: '%c', 0x%02x\n", (char)KeyCode, KeyCode);

	if (editor->vcobject.ime_letter_enable)
	{
	#ifdef KEY_IME
		if (KEY_IME == KeyCode)
		{
			editor_reset_ime_roll(editor);

			if (editor->vcobject.ime_letter_enable && editor->vcobject.ime_digital_enable)
			{
				if (++editor->ime_mode > ITP_LETTER_LOW)
					editor->ime_mode = ITP_DIGITAL;
			}
			else if (editor->vcobject.ime_letter_enable)
			{
				editor->ime_mode = (ITP_LETTER_UP == editor->ime_mode) ? ITP_LETTER_LOW : ITP_LETTER_UP;
			}
			goto process_key;
		}
	#endif
	}

	if (editor->keyfilter && editor->keyfilter(KeyCode))		// ���������˺���
		return FALSE;

	if (!editor->input_buffer)
		return FALSE;

	len = wstrlen(editor->input_buffer);
	if (len && (KEY_CANCEL == KeyCode))
	{
		editor->input_buffer[0] = 0;
		editor_reset_ime_roll(editor);
	}
	else if (len && (KEY_BACKSPACE == KeyCode))
	{
		editor->input_buffer[--len] = 0;
		editor_reset_ime_roll(editor);
	}
	else if ((len < editor->maxinputchar)
					&& (IsDigital(KeyCode) || ('.' == KeyCode)))
	{
		input = (WCHAR)KeyCode;

		if ('.' == KeyCode)
		{
			editor_reset_ime_roll(editor);

			if (wstrstr(editor->input_buffer, wcspoint))
				return FALSE;
		}
		else
		{
			WCHAR current_letter = len ? editor->input_buffer[len - 1] : 0;
			input = editor_key_to_letter(editor, current_letter, KeyCode);
		}

		if (input)
		{
			if (len && (editor->ime_prev_key == (BYTE)KeyCode) && (ITP_DIGITAL != editor->ime_mode))
				len--;

			editor->input_buffer[len++] = input;
			editor->input_buffer[len++] = (WCHAR)0;

			editor->ime_input_timeout = 0;
			editor->ime_prev_key = (BYTE)KeyCode;
		}
	}
	else
		return FALSE;					// ����FALSE��ʾ����δ����

#ifdef KEY_IME
process_key:
#endif
	editor->cursortogglecnt = 0;		// ���ù����˸״̬
	Editor_OnDraw(editor);

	return TRUE;						// ����TRUE��ʾ�����Ѵ���
}

BOOL Editor_Handler(EDITOR *editor, EVENT *Event)
{
	switch (Event->Type) {
	case EV_FORM_DRAW:
		Editor_OnDraw(editor);
		break;

	case EV_KEYPRESS:
		return Editor_OnKeyPress(editor, (WORD)Event->Para);

	case EV_APP_IDLE:
		Editor_ShowCursor(editor);

		if (editor->ime_input_timeout < 15)
		{
			if (++editor->ime_input_timeout >= 15)
				editor->ime_prev_key = 0;
		}
		break;
	}

	return FALSE;
}

#endif
