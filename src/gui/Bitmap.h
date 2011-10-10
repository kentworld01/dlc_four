#ifndef BITMAP_H
#define BITMAP_H


//===========================================================================================
#if (MODULE_GUI_SYSTEM_BITMAP == 0)
#else

typedef struct
{
	DWORD width;
	DWORD height;
	DWORD SizeImage;
	BYTE *Bitmap;
} MONOBMP;

//===========================================================================================

bool MonoBmp_Load(MONOBMP *bmp, BYTE *BmpData);
BYTE MonoBmp_GetPixel(MONOBMP *bmp, WORD x, WORD y);
void MonoBmp_Inverse(MONOBMP *bmp);


#endif


#endif
