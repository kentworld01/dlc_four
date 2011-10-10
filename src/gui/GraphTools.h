#ifndef GRAPHTOOLS_H
#define GRAPHTOOLS_H


//===========================================================================================
#if (MODULE_GUI_SYSTEM_GRAPHTOOLS == 0)
#else

//===========================================================================================

BYTE Graph_GetPixel(WORD x, WORD y);
void Graph_SetPixel(WORD x, WORD y, BYTE bw);
void Graph_InvertPixel(WORD x, WORD y);
void Graph_Line(WORD sx, WORD sy, WORD ex, WORD ey);
void Graph_Rectangle(WORD left, WORD top, WORD width, WORD height);
void Graph_ClearRectangle(WORD left, WORD top, WORD width, WORD height);
void Graph_FillRectangle(WORD left, WORD top, WORD width, WORD height);
void Graph_InvertRectangle(WORD left, WORD top, WORD width, WORD height);
void Graph_PutBitmap(WORD x, WORD y, WORD width, WORD height, MONOBMP *bmp);


#endif


#endif
