#ifndef DES_H
#define DES_H


//-----------------------------------------------------------------------------------
// 调用接口

void DES_SetKey(BYTE *Key);						// 初始化密匙
void DES_Encrypt(BYTE *In, BYTE *Out);				// 数据加密
void DES_Decrypt(BYTE *In, BYTE *Out);				// 数据解密


#endif
