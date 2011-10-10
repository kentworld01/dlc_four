//-----------------------------------------------------------------------------------
#include "Device.h"
#include "OS.h"
#include "GUI.h"


//===========================================================================================
#if (MODULE_GUI_SYSTEM_LABEL == 0)
#else

//===========================================================================================
BOOL Label_Handler(LABEL *label, EVENT *Event);

void Static_Draw(
			WORD sx, WORD sy,
			WORD width, WORD height,
			DWORD style,
			WCHAR *Title,
			WCHAR *String
			)
{
	WORD w, fh;

	fh = Font_GetFontHeight(FT_GBK);

	Graph_ClearRectangle(sx, sy, width, height);

	if (WS_BOARDLINE & style)
		Graph_Rectangle(sx, sy, width, height);

	if ((WS_ALIGN_VMIDDLE & style) && (fh < height))
		sy += ((height - fh) >> 1);

	if (Title) {
		w = Text_GetDisplayWidth((WCHAR *)Title);
		Text_DisplayWString(sx, sy, w, height, (WCHAR *)Title);
		sx += w; 
		width -= w;
	}

	if (String) {

		w = Text_GetDisplayWidth((WCHAR *)String);

		if ((WS_ALIGN_RIGHT & style) && (w < width))
			sx += width - w;

		else if (WS_ALIGN_VMIDDLE & style)
			sx += ((width - w) >> 1);

		Text_DisplayWString(sx, sy, width, height, (WCHAR *)String);
	}
}

//===========================================================================================

BOOL Label_Create(
			FORM *form,
			LABEL *label,
			WORD sx, WORD sy,
			WORD width, WORD height,
			DWORD style,
			WCHAR *output_buffer
			)
{
	memset((void *)label, 0x00, sizeof(LABEL));
	label->rect.sx = sx;
	label->rect.sy = sy;
	label->rect.width = width;
	label->rect.height = height;

	label->vcobject.handler =
		(BOOL (*)(VCOBJECT *, EVENT *))
			Label_Handler;

	label->output_buffer = output_buffer;

	label->vcobject.enable = 1;

	// 设置外框形式，默认无边框
	if (style & WS_BOARDLINE)
		label->vcobject.boardline = 1;
	else if (style & WS_UNDERLINE)
		label->vcobject.underline = 1;

	// 设置水平对齐，默认向左对齐
	if (style & WS_ALIGN_HMIDDLE)
		label->vcobject.align_hmiddle = 1;
	else if (style & WS_ALIGN_LEFT)
		label->vcobject.align_left = 1;
	else if (style & WS_ALIGN_RIGHT)
		label->vcobject.align_right = 1;
	else
		label->vcobject.align_left = 1;

	// 设置垂直对齐，默认向上对齐
	if (style & WS_ALIGN_VMIDDLE)
		label->vcobject.align_vmiddle = 1;
	else if (style & WS_ALIGN_TOP)
		label->vcobject.align_top = 1;
	else if (style & WS_ALIGN_BOTTOM)
		label->vcobject.align_bottom = 1;
	else
		label->vcobject.align_top = 1;

	// 设置文本反白显示
	if (style & WS_INVERT)
		label->vcobject.invert = 1;

	Form_AddObject(form, (VCOBJECT *)label);

	return TRUE;
}

void Label_GetOutputRect(LABEL *label, RECT *outputrect, RECT *textrect)
{
	outputrect->sx = label->rect.sx;
	outputrect->sy = label->rect.sy;
	outputrect->width = label->rect.width;
	outputrect->height = label->rect.height;

	if (label->vcobject.boardline)
	{
		outputrect->sx += 1;
		outputrect->sy += 1;
		outputrect->width -= 2;
		outputrect->height -= 2;
	}
	else if (label->vcobject.underline)
		outputrect->height -= 1;

	textrect->sx = outputrect->sx;
	textrect->sy = outputrect->sy;
	textrect->width = Text_GetDisplayWidth(label->output_buffer);
	textrect->height = Text_GetDisplayHeight();

	// 计算输出文本水平方向对齐位置
	if (label->vcobject.align_hmiddle)
		textrect->sx += (outputrect->width - textrect->width) / 2;
	else if (label->vcobject.align_right)
		textrect->sx += (outputrect->width - textrect->width);

	// 计算输出文本垂直方向对齐位置
	if (label->vcobject.align_vmiddle)
		textrect->sy += (outputrect->height - textrect->height) / 2;
	else if (label->vcobject.align_bottom)
		textrect->sy += (outputrect->height - textrect->height);
}

void Label_OnDraw(LABEL *label)
{
	RECT client, TextRect;

	// 清除显示区
	Graph_ClearRectangle(
				label->rect.sx,
				label->rect.sy,
				label->rect.width,
				label->rect.height);

	// 根据LABEL属性，计算输出区域及文本区域位置
	Label_GetOutputRect(label, &client, &TextRect);

	// 绘制外边框
	if (label->vcobject.boardline)
	{
		Graph_Rectangle(label->rect.sx,
					label->rect.sy,
					label->rect.width,
					label->rect.height);
	}

	// 绘制下划线
	else if (label->vcobject.underline)
	{
		Graph_Line(label->rect.sx,
					label->rect.sy + label->rect.height - 1,
					label->rect.sx + label->rect.width - 1,
					label->rect.sy + label->rect.height - 1);
	}

	// 输出显示文本
	Text_DisplayWString(TextRect.sx,
				TextRect.sy,
				TextRect.width,
				TextRect.height,
				label->output_buffer);

	// 设置文本反白显示
	if (label->vcobject.invert)
	{
		Graph_InvertRectangle(
				TextRect.sx,
				TextRect.sy,
				TextRect.width,
				TextRect.height);
	}

	label->vcobject.updated = 1;
}

void Label_Update(LABEL *label)
{
	Label_OnDraw(label);
}

BOOL Label_Handler(LABEL *label, EVENT *Event)
{
	switch (Event->Type)
	{
	case EV_FORM_DRAW:
		Label_OnDraw(label);
		break;

	case EV_KEYPRESS:
		break;

	case EV_APP_IDLE:
		break;
	}
	return FALSE;
}

#endif
