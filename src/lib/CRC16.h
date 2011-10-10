
#ifndef CRC16_H
#define CRC16_H


#ifdef __cplusplus
extern "C" {
#endif


void CRC16_Clear(WORD *CRC16);
void CRC16_Calc(WORD *CRC16, BYTE uc);

WORD CRC16_Reset(void);
WORD CRC16(WORD CRC16, BYTE c);

WORD CRC16_Get(BYTE *Data, DWORD Count);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

