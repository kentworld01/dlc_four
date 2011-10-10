//===========================================================================================
#include "Device.h"
#include "OS.h"
#include "GUI.h"


//===========================================================================================
#if (MODULE_GUI_SYSTEM_GRAPHTOOLS == 0)
#else

//===========================================================================================
BOOL GUI_OutofScreen(WORD x, WORD y)
{
	if (x >= LCD_WIDTH_LIMIT)
		return 1;

	if (y >= LCD_HEIGHT_LIMIT)
		return 1;

	return 0;
}

int GUI_InvertScreen( WORD *x, WORD *y )
{
#ifdef MODULE_GUI_INVERT_SCREEN
#if (MODULE_GUI_INVERT_SCREEN == 1)
	*x = LCD_WIDTH_LIMIT - *x;
	*y = LCD_HEIGHT_LIMIT - *y - 1;
#endif
#endif

	return 0;
}

#if (0)									// 像素纵横对应的位映射方式
BYTE Graph_GetPixel(WORD x, WORD y)
{
	BYTE *p, m;

	if (GUI_OutofScreen(x, y))
		return 0;

	p = FrameBuffer + (DWORD)(LCD_FB_X_BYTES * y + (x >> 3));
	m = 1 << (7 - (x & 0x7));

	return ((*p) & m);
}

void Graph_SetPixel(WORD x, WORD y, BYTE bw)
{
	BYTE *p, m;

	if (GUI_OutofScreen(x, y))
		return;

	p = FrameBuffer + (DWORD)(LCD_FB_X_BYTES * y + (x >> 3));
	m = 1 << (7 - (x & 0x7));

//TRACE("x = %d\n", x);
//TRACE("y = %d\n", y);
//TRACE("(LCD_FB_X_BYTES * y + (x >> 3)) = %d\n", (int)(DWORD)(LCD_FB_X_BYTES * y + (x >> 3)));
//TRACE("offset = %d\n", (int)(DWORD)(p - FrameBuffer));
//TRACE("p = 0x%08x\n", (int)(DWORD)p);
//TRACE("FrameBuffer = 0x%08x\n", (int)(DWORD)FrameBuffer);

	if (bw)
		(*p) |= m;
	else
		(*p) &= ~m;
}

void Graph_InvertPixel(WORD x, WORD y)
{
	BYTE *p, m;

	if (GUI_OutofScreen(x, y))
		return;

	p = FrameBuffer + (DWORD)(LCD_FB_X_BYTES * y + (x >> 3));
	m = 1 << (7 - (x & 0x7));

	(*p) ^= m;
}
#else									// 与LCD像素位置对应的位映射方式
//===========================================================================================
//
// yp = y / 8;
// bits = (y % 8)
// offset = __LCD_WIDTH_LIMIT * yp + x
//
BYTE Graph_GetPixel(WORD x, WORD y)
{
	BYTE *p, yp, m;

	if (GUI_OutofScreen(x, y))
		return 0;

	GUI_InvertScreen( &x, &y );

	yp = (BYTE)(y >> 3);
	m = 1 << (y & 0x7);
	p = LCD_GETFBPOINTER(x, yp);

	return ((*p) & m);
}

void Graph_SetPixel(WORD x, WORD y, BYTE bw)
{
	BYTE *p, yp, m;

	if (GUI_OutofScreen(x, y))
		return;

	GUI_InvertScreen( &x, &y );

	yp = (BYTE)(y >> 3);
	m = 1 << (y & 0x7);
	p = LCD_GETFBPOINTER(x, yp);

	if (bw)
		(*p) |= m;
	else
		(*p) &= ~m;
}

void Graph_InvertPixel(WORD x, WORD y)
{
	BYTE *p, yp, m;

	if (GUI_OutofScreen(x, y))
		return;
	
	GUI_InvertScreen( &x, &y );

	yp = (BYTE)(y >> 3);
	m = 1 << (y & 0x7);
	p = LCD_GETFBPOINTER(x, yp);

	(*p) ^= m;
}
#endif

void Graph_Line(WORD sx, WORD sy, WORD ex, WORD ey)
{
	WORD tmp, dx, dy, x, y;

	if (sx > ex) {
		tmp = sx;
		sx = ex;
		ex = tmp;
	}
	if (sy > ey) {
		tmp = sy;
		sy = ey;
		ey = tmp;
	}
	dx = (ex - sx);
	dy = (ey - sy);

	if (dx == 0) {
		// 垂直线
		for(y=sy; y<=ey; y++)
			Graph_SetPixel(sx, y, 1);

	} else if (dy == 0) {
		// 水平线
		for(x=sx; x<=ex; x++)
			Graph_SetPixel(x, sy, 1);

	 }
#if (0)
	 else if (((ex > sx) && (ey > sy))
			|| ((ex < sx) && (ey < sy))) {
		// 左上-右下，斜线
		if (dx > dy) {
			for(x=0; x<=dx; x++) {
				y = x * dy / dx;
				Graph_SetPixel(sx + x, sy + y, 1);
			}
		} else {
			for(y=0; y<=dy; y++) {
				x = y * dx / dy;
				Graph_SetPixel(sx + x, sy + y, 1);
			}
		}

	} else {
		// 右上-左下，斜线
		if (dx > dy) {
			for(x=0; x<=dx; x++) {
				y = x * dy / dx;
				Graph_SetPixel(sx + x, ey - y, 1);
			}
		} else {
			for(y=0; y<=dy; y++) {
				x = y * dx / dy;
				Graph_SetPixel(sx + x, ey - y, 1);
			}
		}
	}
#endif
}

void Graph_Rectangle(WORD left, WORD top, WORD width, WORD height)
{
	WORD right, bottom;

	right = left + width - 1;
	right = (right < LCD_WIDTH_LIMIT) ? right : LCD_WIDTH_LIMIT - 1;
	bottom = top + height - 1;
	bottom = (bottom < LCD_HEIGHT_LIMIT) ? bottom : LCD_HEIGHT_LIMIT - 1;

	Graph_Line(left, top, right, top);
	Graph_Line(left, bottom, right, bottom);
	Graph_Line(left, top, left, bottom);
	Graph_Line(right, top, right, bottom);
}

void Graph_ClearRectangle(WORD left, WORD top, WORD width, WORD height)
{
	WORD ex, ey, vx, vy, i, j;

	ex = left + width - 1;
	ex = (ex < LCD_WIDTH_LIMIT) ? ex : LCD_WIDTH_LIMIT - 1;
	ey = top + height - 1;
	ey = (ey < LCD_HEIGHT_LIMIT) ? ey : LCD_HEIGHT_LIMIT - 1;

	for(vy=top,j=0; vy<=ey; vy++,j++)
		for(vx=left,i=0; vx<=ex; vx++,i++)
			Graph_SetPixel(vx, vy, 0);
}

void Graph_FillRectangle(WORD left, WORD top, WORD width, WORD height)
{
	WORD ex, ey, vx, vy, i, j;

	ex = left + width - 1;
	ex = (ex < LCD_WIDTH_LIMIT) ? ex : LCD_WIDTH_LIMIT - 1;
	ey = top + height - 1;
	ey = (ey < LCD_HEIGHT_LIMIT) ? ey : LCD_HEIGHT_LIMIT - 1;

	for(vy=top,j=0; vy<=ey; vy++,j++)
		for(vx=left,i=0; vx<=ex; vx++,i++)
			Graph_SetPixel(vx, vy, 1);
}

void Graph_InvertRectangle(WORD left, WORD top, WORD width, WORD height)
{
	WORD ex, ey, vx, vy, i, j;

	ex = left + width - 1;
	ex = (ex < LCD_WIDTH_LIMIT) ? ex : LCD_WIDTH_LIMIT - 1;
	ey = top + height - 1;
	ey = (ey < LCD_HEIGHT_LIMIT) ? ey : LCD_HEIGHT_LIMIT - 1;

	for(vy=top,j=0; vy<=ey; vy++,j++)
		for(vx=left,i=0; vx<=ex; vx++,i++)
			Graph_InvertPixel(vx, vy);
}

void Graph_PutBitmap(WORD x, WORD y, WORD width, WORD height, MONOBMP *bmp)
{
	WORD ex, ey, vx, vy, i, j;

	ex = x + ((width < bmp->width) ? width : bmp->width) - 1;
	ex = (ex < LCD_WIDTH_LIMIT) ? ex : LCD_WIDTH_LIMIT - 1;
	ey = y + ((height < bmp->height) ? height : bmp->height) - 1;
	ey = (ey < LCD_HEIGHT_LIMIT) ? ey : LCD_HEIGHT_LIMIT - 1;

	for(vy=y,j=0; vy<=ey; vy++,j++)
		for(vx=x,i=0; vx<=ex; vx++,i++)
			Graph_SetPixel(vx, vy, MonoBmp_GetPixel(bmp, i, j));
}

#endif
