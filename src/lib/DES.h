#ifndef DES_H
#define DES_H


//-----------------------------------------------------------------------------------
// ���ýӿ�

void DES_SetKey(BYTE *Key);						// ��ʼ���ܳ�
void DES_Encrypt(BYTE *In, BYTE *Out);				// ���ݼ���
void DES_Decrypt(BYTE *In, BYTE *Out);				// ���ݽ���


#endif
