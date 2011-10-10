//===========================================================================================
#include "Device.h"
#include "OS.h"
#include "GUI.h"


//===========================================================================================
#if (MODULE_GUI_SYSTEM_BITMAP == 0)
#else

//===========================================================================================

bool MonoBmp_Load(MONOBMP *bmp, BYTE *BmpData)
{
	BYTE *p = BmpData;

	bmp->width = GetLong_LE((BYTE *)p);
	p += sizeof(DWORD);

	bmp->height = GetLong_LE((BYTE *)p);
	p += sizeof(DWORD);

	bmp->SizeImage = GetLong_LE((BYTE *)p);
	p += sizeof(DWORD);

	bmp->Bitmap = p;

	if (bmp->width * bmp->height / 8 == bmp->SizeImage)
		return TRUE;

	return FALSE;
}

BYTE MonoBmp_GetPixel(MONOBMP *bmp, WORD x, WORD y)
{
	DWORD pixels, bytes;
	BYTE mask;

	if (x >= bmp->width)
		return 0;
	if (y >= bmp->height)
		return 0;

	pixels = (DWORD)y * bmp->width + (DWORD)x;
	bytes = pixels / 8;
	mask = 1 << (7 - (pixels % 8));

	return (bmp->Bitmap[bytes] & mask);
}

void MonoBmp_Inverse(MONOBMP *bmp)
{
	DWORD i;

	for(i=0; i<bmp->SizeImage; i++)
		bmp->Bitmap[i] = ~bmp->Bitmap[i];
}

#endif
