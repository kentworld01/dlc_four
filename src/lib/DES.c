//===========================================================================================
//
#include "Config.h"
#include "DES.h"


#if (MODULE_APP_DES == 1)

void DESCalc(BYTE *In, BYTE *pParity, BYTE *Out);
void InitPermutation(BYTE *In);
void ConversePermutation(BYTE *In);
void Expand(BYTE *In, BYTE *Out);
void Permutation(BYTE *In);
void PermutationChoose1(BYTE *In, BYTE *Out);
void PermutationChoose2(BYTE *In, BYTE *pParity);
void CycleMove(BYTE *In, BYTE bitMove);


//////////////////////////////////////////////////////////////////////////////
// DES特征数据
//////////////////////////////////////////////////////////////////////////////

static const BYTE BitIP[64] =
{
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7,
	56, 48, 40, 32, 24, 16,  8,  0,
	58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6
};

static const BYTE BitCP[64] =
{
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25,
	32,  0, 40,  8, 48, 16, 56, 24
};

static const BYTE BitExp[48] =
{
	31, 0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8, 7, 8, 9,10,
	11,12,11,12,13,14,15,16,15,16,17,18,19,20,19,20,
	21,22,23,24,23,24,25,26,27,28,27,28,29,30,31,0
};

static const BYTE BitPM[32] =
{
	15, 6,19,20,28,11,27,16, 0,14,22,25, 4,17,30, 9,
	 1, 7,23,13,31,26, 2, 8,18,12,29, 5,21,10, 3, 24
};

static const BYTE sBox[8][64] =
{
	{
		14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
		 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
	},

	{
		15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
		 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
	},

	{
		10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
		13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
	},

	{
		 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
	},

    {
		 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
		14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
	},

	{
		12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
	},


	{
		 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
	},

	{
		13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
	}
};

static const BYTE BitPMC1[56] =
{
	56, 48, 40, 32, 24, 16,  8,
  	 0, 57, 49, 41, 33, 25, 17,
	 9,  1, 58, 50, 42, 34, 26,
	18, 10,  2, 59, 51, 43, 35,
	62, 54, 46, 38, 30, 22, 14,
	 6, 61, 53, 45, 37, 29, 21,
	13,  5, 60, 52, 44, 36, 28,
	20, 12,  4, 27, 19, 11,  3
};

static const BYTE BitPMC2[48] =
{
	13, 16, 10, 23,  0,  4,
 	 2, 27, 14,  5, 20,  9,
	22, 18, 11,  3, 25,  7,
	15,  6, 26, 19, 12,  1,
	40, 51, 30, 36, 46, 54,
	29, 39, 50, 44, 32, 47,
	43, 48, 38, 55, 33, 52,
	45, 41, 49, 35, 28, 31
};

static const BYTE bitDisplace[16] = {1,1,2,2, 2,2,2,2, 1,2,2,2, 2,2,2,1};

static unsigned char SubKey[16][6];


//////////////////////////////////////////////////////////////////////////////
// DES算法实现
//////////////////////////////////////////////////////////////////////////////

void DESCalc(BYTE *In, BYTE *pParity, BYTE *Out)
{
	BYTE outBuf[6], buf[8], c;
	DWORD i;

	Expand(In, outBuf);

	for(i=0; i<6; i++)
		outBuf[i] = outBuf[i] ^ pParity[i];
												// outBuf    xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
	buf[0] = outBuf[0] >> 2;                               //xxxxxx -> 2
	buf[1] = ((outBuf[0] & 0x03) << 4) | (outBuf[1] >> 4);    // 4 <- xx xxxx -> 4
	buf[2] = ((outBuf[1] & 0x0f) << 2) | (outBuf[2] >> 6); //        2 <- xxxx xx -> 6
	buf[3] = outBuf[2] & 0x3f;                             //                    xxxxxx
	buf[4] = outBuf[3] >> 2;                               //                           xxxxxx
	buf[5] = ((outBuf[3] & 0x03) << 4) | (outBuf[4] >> 4); //                                 xx xxxx
	buf[6] = ((outBuf[4] & 0x0f) << 2) | (outBuf[5] >> 6); //                                        xxxx xx
	buf[7] = outBuf[5] & 0x3f;                             //                                               xxxxxx

	for(i=0; i<8; i++)
	{
		c = (buf[i] & 0x20) | ((buf[i] & 0x1e) >> 1) | ((buf[i] & 0x01) << 4);
		buf[i] = sBox[i][c] & 0x0f;
	}

	for(i=0; i<4; i++)
		outBuf[i] = (buf[i << 1] << 4) | buf[(i << 1) | 1];

	Permutation(outBuf);

	for(i=0; i<4; i++)
		Out[i] = outBuf[i];
}

void InitPermutation(BYTE *In)
{
	BYTE newData[8];
	DWORD i;

	for(i=0; i<8; i++)
		newData[i] = 0;

	for(i=0; i<64; i++)
		if((In[BitIP[i] >> 3] & (1 << (7- (BitIP[i] & 0x07)))) != 0)
			newData[i >> 3] = newData[i >> 3] | (1 << (7 - (i & 0x07)));

	for(i=0; i<8; i++)
		In[i] = newData[i];
}

void ConversePermutation(BYTE *In)
{
	BYTE newData[8];
	DWORD i;

	for(i=0; i<8; i++)
		newData[i] = 0;

	for(i=0; i<64; i++)
		if((In[BitCP[i] >> 3] & (1 << (7 - (BitCP[i] & 0x07))))!=0)
			newData[i >> 3] = newData[i >> 3] | (1 << (7 - (i & 0x07)));

	for(i=0; i<8; i++)
		In[i] = newData[i];
}

void Expand(BYTE *In, BYTE *Out)
{
	DWORD i;

	for(i=0; i<6; i++)
		Out[i] = 0;
	for(i=0; i<48; i++)
		if((In[BitExp[i] >> 3] & (1 << (7 - (BitExp[i] & 0x07))))!=0)
			Out[i >> 3] = Out[i >> 3] | (1 << (7 - (i & 0x07)));
}

void Permutation(BYTE *In)
{
	BYTE newData[4];
	DWORD i;

	for(i=0; i<4; i++)
		newData[i] = 0;

	for(i=0; i<32; i++)
		if((In[BitPM[i] >> 3] & (1 << (7 - (BitPM[i] & 0x07)))) != 0)
			newData[i >> 3] = newData[i >> 3] | (1 << (7 - (i & 0x07)));

	for(i=0; i<4; i++)
		In[i] = newData[i];
}

void PermutationChoose1(BYTE *In, BYTE *Out)
{
	DWORD i;

	for(i=0; i<7; i++)
		Out[i] = 0;

	for(i=0; i<56; i++)
		if((In[BitPMC1[i] >> 3] & (1 << (7 - (BitPMC1[i] & 0x07)))) != 0)
			Out[i >> 3] = Out[i >> 3] | (1 << (7 - (i & 0x07)));
}

void PermutationChoose2(BYTE *In, BYTE *pParity)
{
	DWORD i;

	for(i=0; i<6; i++)
		pParity[i] = 0;

	for(i=0; i<48; i++)
	    if((In[BitPMC2[i] >> 3] & (1 << (7 - (BitPMC2[i] & 0x07))))!=0)
			pParity[i >> 3] = pParity[i >> 3] | (1 << (7 - (i & 0x07)));
}

void CycleMove(BYTE *In, BYTE bitMove)
{
	DWORD i;

	for(i=0; i<bitMove; i++)
	{
		In[0] = (In[0] << 1) | (In[1] >> 7);
		In[1] = (In[1] << 1) | (In[2] >> 7);
		In[2] = (In[2] << 1) | (In[3] >> 7);
		In[3] = (In[3] << 1) | ((In[0] & 0x10) >> 4);
		In[0] = (In[0] & 0x0f);
	}
}

//////////////////////////////////////////////////////////////////////////////
// DES调用接口
//////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------
void DES_SetKey(BYTE *Key)
{
	BYTE outData56[7], key28l[4], key28r[4], key56o[7];
	DWORD i;

	memset((void *)key28l, 0x00, 4);
	memset((void *)key28r, 0x00, 4);

	PermutationChoose1(Key, outData56);

	key28l[0] = outData56[0] >> 4;
	key28l[1] = (outData56[0] << 4) | (outData56[1] >> 4);
	key28l[2] = (outData56[1] << 4) | (outData56[2] >> 4);
	key28l[3] = (outData56[2] << 4) | (outData56[3] >> 4);
	key28r[0] = outData56[3] & 0x0f;
	key28r[1] = outData56[4];
	key28r[2] = outData56[5];
	key28r[3] = outData56[6];

	for(i=0; i<16; i++)
	{
		CycleMove(key28l, bitDisplace[i]);
		CycleMove(key28r, bitDisplace[i]);
		key56o[0] = (key28l[0] << 4) | (key28l[1] >> 4);
		key56o[1] = (key28l[1] << 4) | (key28l[2] >> 4);
		key56o[2] = (key28l[2] << 4) | (key28l[3] >> 4);
		key56o[3] = (key28l[3] << 4) | (key28r[0]);
		key56o[4] = key28r[1];
		key56o[5] = key28r[2];
		key56o[6] = key28r[3];
		PermutationChoose2(key56o, &SubKey[i][0]);
	}
}

//-----------------------------------------------------------------------------------
// inData, outData 都为8Bytes，否则出错
//
void DES_Encrypt(BYTE *In, BYTE *Out)
{
	BYTE temp[4], buf[4];
	DWORD i, j;

	memset((void *)temp, 0x00, 4);

	for(i=0; i<8; i++)
		Out[i] = In[i];

	InitPermutation(Out);

	for(i=0; i<16; i++)
	{
		for(j=0; j<4; j++)
			temp[j] = Out[j];					//temp = Ln
		for(j=0; j<4; j++)
			Out[j] = Out[j + 4];				//Ln+1 = Rn
		DESCalc(Out, &SubKey[i][0], buf);	//Rn ==Kn==> buf
		for(j=0; j<4; j++)
			Out[j + 4] = temp[j] ^ buf[j];		//Rn+1 = Ln^buf
	}

	for(j=0; j<4; j++)
		temp[j] = Out[j + 4];

	for(j=0; j<4; j++)
		Out[j + 4] = Out[j];

	for(j=0; j<4; j++)
		Out[j] = temp[j];

	ConversePermutation(Out);
}

//-----------------------------------------------------------------------------------
// inData, outData 都为8Bytes，否则出错
//
void DES_Decrypt(BYTE *In, BYTE *Out)
{
	BYTE temp[4], buf[4];
	int i, j;

	memset((void *)temp, 0x00, 4);

	for(i=0; i<8; i++)
		Out[i] = In[i];

	InitPermutation(Out);

	for(i=15; i>=0; i--)
	{
		for(j=0; j<4; j++)
			temp[j] = Out[j];
		for(j=0; j<4; j++)
			Out[j] = Out[j + 4];
		DESCalc(Out, &SubKey[i][0], buf);
		for(j=0; j<4; j++)
			Out[j + 4] = temp[j] ^ buf[j];
	}

	for(j=0; j<4; j++)
		temp[j] = Out[j + 4];

	for(j=0; j<4; j++)
		Out[j + 4] = Out[j];

	for(j=0; j<4; j++)
		Out[j] = temp[j];

	ConversePermutation(Out);
}

#endif

