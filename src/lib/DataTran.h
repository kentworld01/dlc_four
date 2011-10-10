#ifndef DATATRAN_H
#define DATATRAN_H


#define WCHARTOCHAR(x)					((char)(BYTE)x)

void strtowstr(WCHAR *dest, char *src);
void wstrtostr(char *dest, WCHAR *src);
int wstrlen(WCHAR *p);
void wstrcpy(WCHAR *pD,WCHAR *pS);
void wstrcat(WCHAR *pD, WCHAR *pS);
int wstrcmp(WCHAR *p1, WCHAR *p2);
WCHAR *wstrstr(WCHAR *p1, WCHAR *p2);


void GetString(char *tag, char *src, int count);
void SetString(char *tag, char *src, int count);

DWORD GetLong_BE(BYTE *src);
void SetLong_BE(BYTE *tag, DWORD val);
DWORD Get3B_BE(BYTE *src);
void Set3B_BE(BYTE *tag, DWORD val);
WORD GetShort_BE(BYTE *src);
void SetShort_BE(BYTE *tag, WORD val);

DWORD GetLong_LE(BYTE *src);
void SetLong_LE(BYTE *tag, DWORD val);
WORD GetShort_LE(BYTE *src);
void SetShort_LE(BYTE *tag, WORD val);

DWORD GetUint24(char *src);

// ===========================================================================================
// 数据包管理
//
#define STREAM_GetByte(val, p)						do { val = (BYTE)*p++; } while (0)
#define STREAM_GetWord(val, p)						do { val = (WORD)GetShort_LE(p); p += sizeof(WORD); } while (0)
#define STREAM_GetDWord(val, p)						do { val = (DWORD)GetLong_LE(p); p += sizeof(DWORD); } while (0)
#define STREAM_GetBinary(buffer, p, Length)			do { memcpy((void *)(buffer), (void *)(p), (Length)); (p) += (size_t)(Length); } while (0)

#define STREAM_GetWord_BE(val, p)						do { val = (WORD)GetShort_BE(p); p += sizeof(WORD); } while (0)
#define STREAM_Get3B_BE(val, p)						do { val = (DWORD)Get3B_BE(p); p += 3; } while (0)
#define STREAM_GetDWord_BE(val, p)					do { val = (DWORD)GetLong_BE(p); p += sizeof(DWORD); } while (0)

#define STREAM_GetWord_LE(val, p)						do { val = (WORD)GetShort_LE(p); p += sizeof(WORD); } while (0)
#define STREAM_GetDWord_LE(val, p)					do { val = (DWORD)GetLong_LE(p); p += sizeof(DWORD); } while (0)

#define STREAM_AddByte(p, val)						do { *p++ = (BYTE)val; } while (0)
#define STREAM_AddWord(p, val)						do { SetShort_LE(p, (WORD)val); p += sizeof(WORD); } while (0)
#define STREAM_AddDWord(p, val)						do { SetLong_LE(p, (DWORD)val); p += sizeof(DWORD); } while (0)
#define STREAM_AddBinary(p, val, Length)				do { memcpy((void *)(p), (void *)(val), (Length)); (p) += (size_t)(Length); } while (0)
#define STREAM_AddRepeat(p, val, Length)				do { memset((void *)(p), (BYTE)(val), (Length)); (p) += (size_t)(Length); } while (0)

#define STREAM_AddWord_BE(p, val)						do { SetShort_BE(p, (WORD)val); p += sizeof(WORD); } while (0)
#define STREAM_Add3B_BE(p, val)						do { Set3B_BE(p, (DWORD)val); p += 3; } while (0)
#define STREAM_AddDWord_BE(p, val)					do { SetLong_BE(p, (DWORD)val); p += sizeof(DWORD); } while (0)

#define STREAM_AddWord_LE(p, val)						do { SetShort_LE(p, (WORD)val); p += sizeof(WORD); } while (0)
#define STREAM_AddDWord_LE(p, val)					do { SetLong_LE(p, (DWORD)val); p += sizeof(DWORD); } while (0)

#define STREAM_GetLength(start, end)					((DWORD)end - (DWORD)start)

#define HiByte(x)										((BYTE)(((WORD)(x) >> 8) & 0xff))
#define LoByte(x)										((BYTE)(((WORD)(x) >> 0) & 0xff))

#define HI_BYTE(x)										((BYTE)(((WORD)(x) >> 8) & 0xff))
#define LO_BYTE(x)										((BYTE)(((WORD)(x) >> 0) & 0xff))

#define DWTOB_L1(x)									((BYTE)(((x) >> 24) & 0xff))
#define DWTOB_L2(x)									((BYTE)(((x) >> 16) & 0xff))
#define DWTOB_L3(x)									((BYTE)(((x) >> 8) & 0xff))
#define DWTOB_L4(x)									((BYTE)(((x) >> 0) & 0xff))

#define BTODW_L1(dw, b)								((dw & (~((DWORD)0xff << 24))) | (((DWORD)(b)) << 24))

char NibbleToHex(BYTE hex);
BYTE HexToNibble(char c);
int HexToMem(BYTE *mem, char *s);
void hextowstr(WCHAR *wcs, char *s, WORD WLength);

void ByteToHex(char *s, BYTE b);

BYTE BCDToByte(BYTE c);
BYTE ByteToBCD(BYTE c);
void GetBCD(char *tag, char *src, int count);
void SetBCD(char *tag, char *src, int count);

DWORD htol(DWORD h);

#define IsDigital(c)		((c >= '0') && (c <= '9'))
#define IsHex(c)			(((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'B')))

#define max(a, b)			((a > b) ? a : b)
#define min(a, b)			((a < b) ? a : b)

DWORD s_atou(char *s);
// 
// 十六进制，[-]0x...
// 二进制，[-]0b...
// 八进制，[-]0...
// 十进制，[-]...
//
long s_atoi (char *s);

void s_itoa (char *buf, long val, int radix, int len);
void s_sprintf (char *buf, const char* str, ...);

char s_lowcase(char c);
char s_upcase(char c);

void strlwr(char *s);
void struwr(char *s);

void IPToString(char *s, DWORD IP);

void StrReverse(char *s);

int s_strcmp(char *p1, char *p2);
void s_memcpy(void *mt, void *ms, DWORD count);
int s_memmem(void *mt, DWORD tc, void *ms, DWORD sc);
int s_memcmp(void *mt, void *ms, DWORD count);


void IntToString(char *s, int n);
void FDigitalToString(char *s, BYTE Sign, DWORD Val, BYTE RadixPoint);

void IntToWString(WCHAR *wcs, DWORD n);
DWORD WStringToInt(WCHAR *wcs);

DWORD MoneyStringToInt(char *Money);

DWORD MoneyWStringToInt(WCHAR *Money);
void IntToMoneyString(WCHAR *Money, DWORD val);

DWORD DateTimeToRelatvie(void *DateTime);
void RelatvieToDateTime(void *DateTime, DWORD RELATIVETIME);

BYTE date_to_week_day(DATETIME *dt);

WORD Cmdline_GetParamCount(char *CmdLine);
void Cmdline_GetParam(char *CmdLine, WORD Index, char *Param, WORD ParamBufLength);


int s_strncmp(char *p1, char *p2, int size);

int s_isdigit( char c );



#endif
