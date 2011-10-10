//===========================================================================================
//
#include "Config.h"
#include "DataTran.h"
#include "Trace.h"
#include <stdarg.h>

#if 0
static const char WeekDayName[7][4] = 
{
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
#endif

#if (0)
static const char MonthName[12][4] = 
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

// ASCDate=Mar 28 2005
void ASCDateToDate(void *Date, char *ASCDate)
{
//	DATETIME *dt;

//	dt = (DATETIME *)Date;
}

//===========================================================================================

void strtowstr(WCHAR *s1, char *s2)
{
	while (*s2)
		*s1++ = (WCHAR)*s2++;
	*s1 = 0;
}

void wstrtostr(char *s1, WCHAR *s2)
{
	while (*s2)
		*s1++ = (char)(BYTE)*s2++;
	*s1 = 0;
}

int wstrlen(WCHAR *p)
{
	int i = 0;

	while (*p++)
		i++;

	return i;
}

void wstrcpy(WCHAR *p1, WCHAR *p2)
{
	*p1 = 0;

	if (!p2)
		return;

	while (*p2)
		*p1++ = *p2++;
	*p1 = 0;
}

void wstrcat(WCHAR *p1, WCHAR *p2)
{
	WCHAR *p = &p1[wstrlen(p1)];
	wstrcpy(p, p2);
}

int wstrcmp(WCHAR *p1, WCHAR *p2)
{
	int r;

	if (p1 == p2)
		return 0;

	while ((r = *p1 - *p2++) == 0)
		if (*p1++ == 0)
			break;

	return r;
}

WCHAR *wstrstr(WCHAR *p1, WCHAR *p2)
{
	return 0;
}

//===========================================================================================

void GetString(char *tag, char *src, int count)
{
	int i;

	for(i=0; i<count; i++)
		*tag++ = *src++;
	*tag = 0;
}

void SetString(char *tag, char *src, int count)
{
	int i;

	for(i=0; i<count; i++)
		*tag++ = *src++;
	*tag = 0;
}

//===========================================================================================
DWORD GetLong_BE(BYTE *src)
{
	DWORD val = 0;

	val |= ((WORD)(*src++) << 24);
	val |= ((WORD)(*src++) << 16);
	val |= ((WORD)(*src++) << 8);
	val |= ((WORD)(*src++) << 0);

	return val;
}

DWORD Get3B_BE(BYTE *src)
{
	DWORD val = 0;

	val |= ((WORD)(*src++) << 16);
	val |= ((WORD)(*src++) << 8);
	val |= ((WORD)(*src++) << 0);

	return val;
}

void SetLong_BE(BYTE *tag, DWORD val)
{
	*tag++ = (BYTE)((val >> 24) & 0xff);
	*tag++ = (BYTE)((val >> 16) & 0xff);
	*tag++ = (BYTE)((val >> 8) & 0xff);
	*tag++ = (BYTE)((val >> 0) & 0xff);
}

void Set3B_BE(BYTE *tag, DWORD val)
{
	*tag++ = (BYTE)((val >> 16) & 0xff);
	*tag++ = (BYTE)((val >> 8) & 0xff);
	*tag++ = (BYTE)((val >> 0) & 0xff);
}

WORD GetShort_BE(BYTE *src)
{
	WORD val = 0;

	val |= ((WORD)(*src++) << 8);
	val |= ((WORD)(*src++) << 0);

	return val;
}

void SetShort_BE(BYTE *tag, WORD val)
{
	*tag++ = (BYTE)((val >> 8) & 0xff);
	*tag++ = (BYTE)((val >> 0) & 0xff);
}

//===========================================================================================
DWORD GetLong_LE(BYTE *src)
{
	DWORD val = 0;

	val |= ((WORD)(*src++) << 0);
	val |= ((WORD)(*src++) << 8);
	val |= ((WORD)(*src++) << 16);
	val |= ((WORD)(*src++) << 24);

	return val;
}

void SetLong_LE(BYTE *tag, DWORD val)
{
	*tag++ = (BYTE)((val >> 0) & 0xff);
	*tag++ = (BYTE)((val >> 8) & 0xff);
	*tag++ = (BYTE)((val >> 16) & 0xff);
	*tag++ = (BYTE)((val >> 24) & 0xff);
}

WORD GetShort_LE(BYTE *src)
{
	WORD val = 0;

	val |= ((WORD)(*src++) << 0);
	val |= ((WORD)(*src++) << 8);

	return val;
}

void SetShort_LE(BYTE *tag, WORD val)
{
	*tag++ = (BYTE)((val >> 0) & 0xff);
	*tag++ = (BYTE)((val >> 8) & 0xff);
}

DWORD GetUint24(char *src)
{
	DWORD val;
	char *tag;

	tag = (char *)&val;
	val = 0;
	tag[0] = src[0];
	tag[1] = src[1];
	tag[2] = src[2];

	return val;
}

//===============================================================================
char NibbleToHex(BYTE n)
{
	const char HexChar[] = "0123456789abcdef";

	return HexChar[n];
}

BYTE HexToNibble(char c)
{
	BYTE val = 0;

	if (c < '0') 
		return 0xff;

	if (c >= 'a')							// 小写转为大写
		c -= 0x20;
	c -= '0';

	if (c >= 17)
		c -= 7;
	if (c < 16)
		val = c;
	else
		return 0xff;

	return val;
}

int HexToMem(BYTE *mem, char *s)
{
	BYTE nb1, nb2;
	int n;

	for(n = 0; *s; s++) {
		nb1 = HexToNibble(*s++);
		nb2 = HexToNibble(*s++);
		if ((0xff != nb1) && (0xff != nb2)) {
			*mem++ = (nb1 << 4) + (nb2 << 0);
			n++;
		} else
			break;
	}

	return n;
}

void hextowstr(WCHAR *wcs, char *s, WORD WLength)
{
	WORD i, j, l1, wc;
	BYTE nb;

	for(l1 = strlen(s) / 4, j = 1; (j < l1) && (j < WLength); j++, wcs++) {
		for(wc = 0, i = 0; i < 4; i++) {
			nb = HexToNibble(*s++);
			if (0xff == nb)
				goto ret;
			wc = (wc << 4) + nb;
		}
		*wcs = wc;
	}

ret:
	*wcs = 0;
}

//===============================================================================
void ByteToHex(char *s, BYTE b)
{
	BYTE a = ByteToBCD(b);

	s[0] = NibbleToHex((a >> 4) & 0xf);
	s[1] = NibbleToHex((a >> 0) & 0xf);
	s[2] = 0;
}

BYTE BCDToByte(BYTE c)
{
	return ((((c & 0xf0) >> 4) * 10)
			+ (c & 0x0f) >> 0);
}

BYTE ByteToBCD(BYTE b)
{
	BYTE a;

	for(a=b; b>=10; b-=10,a+=6);

	return a;
}

void GetBCD(char *tag, char *src, int count)
{
	int i;
	unsigned char c;

	for(i=0; i<count; i++)
	{
		c = (unsigned char)(*src++);
		*tag++ = '0' + (c >> 4);
		*tag++ = '0' + (c & 0xf);
	}
	*tag = 0;
}

void SetBCD(char *tag, char *src, int count)
{
	char buf[50], *p;
	int i, l;

	l = strlen(src);
	strcpy(buf, "");
	for(i=0; i<count*2-l; i++)
		strcat(buf, "0");
	strcat(buf, src);

	l = strlen(buf);
	for(p=buf,i=0; i<count; i++)
	{
		*tag++ = ((p[0] - '0') << 4) | (p[1] - '0');
		p += 2;
	}
	*tag = 0;
}

DWORD htol(DWORD h)
{
	DWORD l;
	BYTE *pl, *ph;

	ph = (BYTE *)&h;
	pl = (BYTE *)&l;

	pl[0] = ph[3];
	pl[1] = ph[2];
	pl[2] = ph[1];
	pl[3] = ph[0];

	return l;
}

//===============================================================================
// 
// 十进制，[-]...
//
DWORD s_atou(char *s)
{
	DWORD val = 0;
	BYTE c, radix = 10, sign = 0;

	while (' ' == (c = *s))					// 过滤空格
		s++;

	if (c == '-') {								// 负数
		sign = 1;
		c = *(++s);
	}

	while (IsDigital(c)) {
		c -= '0';
		val = val * radix + c;
		c = *(++s);
	}

	if ((sign) && (val < 0x80000000))
		val = 0 - val;

	return val;
}

//===============================================================================
// 
// 十六进制，[-]0x...
// 二进制，[-]0b...
// 八进制，[-]0...
// 十进制，[-]...
//
long s_atoi (char *s)
{
	long val = 0;
	BYTE c, radix, sign = 0;

	while ((c = *s) == ' ')						// 过滤空格
		s++;

	if (c == '-') {								// 负数
		sign = 1;
		c = *(++s);
	}

	if (c == '0') {
		c = *(++s);

		if (c <= ' ')							// !!! 包含非法字符，终止转换!!! 
			goto RET;

		if (c == 'x') {							// 十六进制，0x...
			radix = 16;
			c = *(++s);
		} else {
			if (c == 'b') {						// 二进制，0b...
				radix = 2;
				c = *(++s);
			} else {
				if (IsDigital(c))				// 八进制，0...
					radix = 8;
				else							// !!! 包含非法字符，终止转换!!! 
					goto RET;
			}
		}

	} else {					// 十进制，...
		if ((c < '1')||(c > '9'))				// !!! 包含非法字符，终止转换!!! 
			goto RET;
		radix = 10;
	}

	while (c > ' ') {

		if (c >= 'a')							// 小写转为大写
			c -= 0x20;
		c -= '0';

		if (c >= 17)
			c -= 7;
		if (c >= radix)							// !!! 包含非法字符，终止转换!!! 
			goto RET;

		val = val * radix + c;
		c = *(++s);
	}

RET:
	if ((sign) && (val > 0))
		val = -val;

	return val;
}

void s_itoa (char *buf, long val, int radix, int len)
{
	BYTE i, c, r, sgn = 0, pad = ' ';
	DWORD v;

	buf[0] = 0;

	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}

	v = val;
	r = radix;
	if (len < 0) {
		len = -len;
		pad = '0';
	}

	if (len > 20)
		return;

	i = 0;
	do {
		c = (BYTE)(v % r);
		if (c < 10)
			c += '0';
		else
			c += 'a' - 10;
		buf[i++] = c;
		v /= r;
	} while (v);

	while (i < len)
		buf[i++] = pad;

	if (sgn)
		buf[i++] = sgn;

	buf[i] = 0;

	StrReverse(buf);
}

void s_sprintf (char *buf, const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l;

	if (buf == 0)
		return;

	va_start(arp, str);

	while ((d = *str++) != 0) {

		if (d != '%') {
			*(buf++) = d;
			continue;
		}

		d = *str++; w = r = s = l = 0;

		if (d == '0') {
			d = *str++; s = 1;
		}

		while (IsDigital(d)) {
			w = w * 10 + (d - '0');
			d = *str++;
		}

		if (s)
			w = -w;

		if (d == 'l') {
			l = 1;
			d = *str++;
		}

		if (!d)
			break;

		if (d == 's') {
			strcpy(buf, va_arg(arp, char *));
			buf += strlen(buf);
			continue;
		}

		if (d == 'c') {
			*(buf++) = (char)va_arg(arp, int);
			continue;
		}

		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'x') r = 16;
		if (d == 'b') r = 2;

		if (!r)
			break;

		if (l) {
			s_itoa(buf, (long)va_arg(arp, long), r, w);
		} else {
			if (r > 0)
				s_itoa(buf, (unsigned long)va_arg(arp, int), r, w);
			else
				s_itoa(buf, (long)va_arg(arp, int), r, w);
		}
		buf += strlen(buf);
	}

	*buf = 0;

	va_end(arp);
}

//===============================================================================
//"1"
//"12"
//"123"
//"1234"
//"12345"
//
void StrReverse(char *s)
{
	char *pr, c;

	pr = s + strlen(s) - 1;
	while (*s) {
		c = *pr;
		*pr = *s;
		*s = c;
		s++;
		pr--;
		if (pr <= s)
			break;
	}
}

char s_lowcase(char c)
{
	if ((c >= 'A') && (c <= 'Z'))
		c = c - 'A' + 'a';

	return c;
}

char s_upcase(char c)
{
	if ((c >= 'a') && (c <= 'z'))
		c = c - 'a' + 'A';

	return c;
}

void strlwr(char *s)
{
	while (*s)
	{
		*s = s_lowcase(*s);
		s++;
	}
}

void struwr(char *s)
{
	while (*s)
	{
		*s = s_upcase(*s);
		s++;
	}
}

int s_strcmp(char *p1, char *p2)
{
	int r;

	if (p1 == p2)
		return 0;

	while ((r = *p1 - *p2++) == 0)
		if (*p1++ == 0)
			break;

	return r;
}
int s_chop( char *str )
{
	int len;
	char tc;
	len = strlen( str );
	while( 1 ){
		len = strlen( str );
		if( len <= 0 )
			break;
		tc = str[ len-1 ];
		if( tc == 0xd || tc == 0xa )
			str[ len-1] = 0;
		else break;
	}
	return 0;
}

void s_memcpy(void *mt, void *ms, DWORD count)
{
	BYTE *pt, *ps;

	ps = (BYTE *)ms + count - 1;
	pt = (BYTE *)mt + count - 1;

	while (count--)
		*pt-- = *ps--;
}
int s_memcmp(void *mt, void *ms, DWORD count)
{
	BYTE *pt, *ps;
	int i;

	ps = (BYTE *)ms;
	pt = (BYTE *)mt;

	for( i=0; i<count; i++ ){
		if(  *pt - *ps  != 0 )
			return (int)( (int)(unsigned)*pt - (int)(unsigned)*ps );
		pt ++;
		ps ++;
	}
	return 0;
}
int s_memmem(void *mt, DWORD tc, void *ms, DWORD sc)
{
	BYTE *pt, *ps;
	int i,j;
	pt = (BYTE*)mt;
	ps = (BYTE*)ms;
	for( i=0; i<tc; i++ ){
		for( j=0; j<sc; j++ ){
			if( pt[i+j] != ps[j] )
				break;
			}
		if( j==sc )
			return i;
		}
	return -1;
}

//===============================================================================
void IntToString(char *s, int n)
{
	BYTE sign;
	char *p;

	if (n == 0) {
		s[0] = '0';
		s[1] = 0;
		return;

	} else if (n < 0) {
		sign = 1;
		n *= -1;

	} else
		sign = 0;

	for(p=s; n; ) {
		*p++ = (n % 10) + '0';
		n /= 10;
	}

	if (sign)
		*p++ = '-';
	*p = 0;

	StrReverse(s);
}

void IPToString(char *s, DWORD IP)
{
	s_sprintf(s, "%03d.%03d.%03d.%03d", ((IP >> 24) & 0xff),((IP >> 16) & 0xff),
						((IP >> 8) & 0xff),(IP & 0xff));
}

void IntToWString(WCHAR *wcs, DWORD n)
{
	char buf[32];

	s_sprintf(buf, "%u", n);
	strtowstr(wcs, buf);
}

DWORD WStringToInt(WCHAR *wcs)
{
	char buf[32];

	wstrtostr(buf, wcs);

	return atoi(buf);
}

DWORD MoneyStringToInt(char *Money)
{
	char *p, buf[32];
	BYTE point, radix, i;
	DWORD val;

	for(p = buf, radix = 0, point = 0; *Money; Money++) {
		if (point)
			radix++;
		if ('.' == *Money)
			point = 1;
		else
			*p++ = *Money;
	}
	*p = 0;

	if (0 == strlen(buf))
		strcpy(buf, "0");

	for(i = radix; i < 2; i++)
		strcat(buf, "0");

	val = (DWORD)atoi(buf);
	//val = (DWORD)s_atou(buf);
	//TRACE("buf:%s, point=%d, radix=%d, val=%u\n", buf, point, radix, val);

	return val;
}

DWORD MoneyWStringToInt(WCHAR *Money)
{
	char buf[32];

	wstrtostr(buf, Money);

	return MoneyStringToInt(buf);
}

#if (PLATFORM_POS_GS05 == 1)
void IntToMoneyString(WCHAR *Money, DWORD val)
{
	char buf[32];
	extern const unsigned short str_063[];

	FDigitalToString(buf, 0, val, 2);
	strtowstr(Money, buf);
	#if(PLATFORM_POS_GS05_ENG == 0)
	wstrcat(Money, (WCHAR *)str_063);									//元
	#endif
	#if(PLATFORM_POS_GS05_ENG ==1)
	//wstrcat(Money, (WCHAR *)str_063);									//元
	#endif
}

void IntToMoneyAndTimerString(WCHAR *Money, DWORD val, DWORD timer)
{
	char buf[32];
	char buf2[32];
	WCHAR tmp[50];
	int len;
	extern const unsigned short str_063[];
	extern const unsigned short str_259[];

	FDigitalToString(buf2, 0, timer, 1);
	buf2[ strlen( buf2 )-2 ] = 0;
	if( 5 > strlen( buf2 ) ){
		len = 5 - strlen( buf2 );
		buf[0] = 0;
		while( len-- > 0 ){
			strcat( buf, " " );
		}
		strcat( buf, buf2 );
		strcpy( buf2, buf );
	}
	FDigitalToString(buf, 0, val, 2);
	strtowstr(Money, buf);
	strtowstr(tmp, buf2);
	#if(PLATFORM_POS_GS05_ENG == 0)
	wstrcat(Money, (WCHAR *)str_063);									//元
	#endif
	#if(PLATFORM_POS_GS05_ENG ==1)
	//wstrcat(Money, (WCHAR *)str_063);									//元
	#endif
	wstrcat(Money, tmp);
	wstrcat(Money, (WCHAR *)str_259);									//次
}
#endif

void FDigitalToString(char *s, BYTE Sign, DWORD Val, BYTE RadixPoint)
{
	BYTE i, l;
	char tmp[16];

	if (Val == 0) {
		strcpy(s, "0");
		return;
	}

	IntToString(tmp, Val);

	if (Sign)
		*s++ = '-';

	l = strlen(tmp);

	if (l > RadixPoint) {
		for(i=0; i<l-RadixPoint; i++)
			*s++ = tmp[i];
		*s++ = '.';
		for(; i<l; i++)
			*s++ = tmp[i];

	} else {
		*s++ = '0';
		*s++ = '.';
		for(i=0; i<RadixPoint-l; i++)
			*s++ = '0';
		for(i=0; i<l; i++)
			*s++ = tmp[i];
	}

	*s = 0;
}

// ===========================================================================================
// 时间原点为公元前1年12月31日
// D = (Y-1)*365 + [(Y-1)/4] - [(Y-1)/100] + [(Y-1)/400] + D

//
static const WORD DaysOnMonthNY[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
static const WORD DaysOnMonthLY[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };
static const WORD DaysOnYear[] = 
{
	0,
//  1    2    3    4     5     6     7    8      9    0
	366, 731, 1096, 1461, 1827, 2192, 2557, 2922, 3288, 3653,
	4018, 4383, 4749, 5114, 5479, 5844, 6210, 6575, 6940, 7305,
	7671, 8036, 8401, 8766, 9132, 9497, 9862, 10227, 10593, 10958,
	11323, 11688, 12054, 12419, 12784, 13149, 13515, 13880, 14245, 14610,
	14976, 15341, 15706, 16071, 16437, 16802, 17167, 17532, 17898, 18263,
	18628, 18993, 19359, 19724, 20089, 20454, 20820, 21185, 21550, 21915,
	22281, 22646, 23011, 23376, 23742, 24107, 24472, 24837, 25203, 25568,
	25933, 26298, 26664, 27029, 27394, 27759, 28125, 28490, 28855, 29220,
	29586, 29951, 30316, 30681, 31047, 31412, 31777, 32142, 32508, 32873,
	33238, 33603, 33969, 34334, 34699, 35064, 35430, 35795, 36160, 36525
};

bool IsLeapYear(DWORD y)
{
	return ((!(y % 4) && (y % 100)) || !(y % 400));
}

// 计算相对于2000年1月1日0时0分0秒的时间
// 此程序仅适用于2000年到2099年
//
DWORD DateTimeToRelatvie(void *DateTime)
{
	DATETIME *dt;
	DWORD D, S;

	dt = (DATETIME *)DateTime;

//printf("DateTimeToRelatvie(), D = %u\n", D);
	D = DaysOnYear[dt->year - 2000];
	if (IsLeapYear(dt->year))
		D += DaysOnMonthLY[dt->month - 1];
	else
		D += DaysOnMonthNY[dt->month - 1];
	D += dt->day - 1;

	S = D * 86400;
	S += dt->hour * 3600 + dt->minute * 60 + dt->second;

	return S;
}

// 根据相对于2000年1月1日0时0分0秒的时间计算当前绝对时间
// 此程序仅适用于2000年到2099年
//
void RelatvieToDateTime(void *DateTime, DWORD RELATIVETIME)
{
	DATETIME *dt;
	DWORD D, S;
	WORD *dom;

	dt = (DATETIME *)DateTime;
	dt->weekday = 0;

	S = RELATIVETIME % 86400;
	dt->hour = (BYTE)(S / 3600);
	S %= 3600;
	dt->minute = (BYTE)(S / 60);
	dt->second = (BYTE)(S % 60);

//printf("RelatvieToDateTime(), D = %u\n", D);
	D = RELATIVETIME / 86400;
	dt->year = (WORD)(D / 365);
	if (DaysOnYear[dt->year] > D)
		dt->year--;
	D -= DaysOnYear[dt->year];
	dt->year += 2000;
	if (IsLeapYear(dt->year))
		dom = (WORD *)DaysOnMonthLY;
	else
		dom = (WORD *)DaysOnMonthNY;
	for(dt->month=1; dt->month<=12; dt->month++)
		if (D < dom[dt->month])
			break;
	dt->day = (BYTE)(D - dom[dt->month - 1] + 1);
}

void test_relatvie_time(void)
{
	DATETIME dt;
	DWORD rt;

	dt.year = 2010;
	dt.month = 11;
	dt.day = 6;
	dt.hour = 23;
	dt.minute = 50;
	dt.second = 0;
	rt = DateTimeToRelatvie(&dt);

	TRACE("Time: %04u-%02u-%02u %02u:%02u:%02u\n",
		dt.year, dt.month, dt.day,
		dt.hour, dt.minute, dt.second);
	TRACEd(rt);

	RelatvieToDateTime(&dt, rt);
	TRACE("Time: %04u-%02u-%02u %02u:%02u:%02u\n",
		dt.year, dt.month, dt.day,
		dt.hour, dt.minute, dt.second);
	TRACEd(rt);
}

/*
计算星期可用   蔡勒（Zeller）公式（只适合于1582年10月15日之后的情形）： 
          W   =   Y   +   [Y/4]   +   [C/4]   -   2C   +   [13(M+1)/5]   +   D   -   1 

  公式中的符号含义如下： 
          C：世纪数减一（年的高两位数）； 
          Y：年（年的低两位数）； 
          M：月（M大于等于3，小于等于14，即在蔡勒公式中，某年的1、2月要看作上一年的 
                13、14月来计算，比如2005年1月1日要看作2004年的13月1日来计算）； 
          D：日； 
          []代表取整，即只要整数部分。 
          W：星期；W对7取模得：0-星期日，1-星期一，2-星期二，3-星期三，4-星期四， 
                5-星期五，6-星期六 
          注意负数不能按习惯的余数的概念求余数，只能按数论中的余数的定义求余。为了 
          方便计算，我们可以给它加上一个7的整数倍，使它变为一个正数。 

  以2005年2月14日为例：C=20，Y=4，M=14，D=14 
          W   =   4   +   [4/4]   +   [20/4]   -   2*20   +   [26*(14+1)/10]   +   14   -   1 
              =   4   +   1           +   5             -   40       +   39                           +   14   -   1 
              =   22   (除以7余1) 
  所以2005年2月14日是星期一。 
*/
BYTE date_to_week_day(DATETIME *dt)
{
	DWORD c, y, m, d, p1, p2, w;

	y = dt->year;
	m = dt->month;
	d = dt->day;

	if (m < 3)
	{
		m += 12;
		y -= 1;
	}

	c = y / 100;
	y -= c * 100;

	p1 = y + (y / 4) + c / 4 + (13 * (m + 1)) / 5 + d;
	p2 = c * 2 + 1;

	if (p1 >= p2)
		w = ((p1 - p2) % 7);
	else
		w = 7 - ((p2 - p1) % 7);

	return (BYTE)w;
}

WORD Cmdline_GetParamCount(char *CmdLine)
{
	WORD i;
	char *s;

	for(s = CmdLine, i = 0; *s; ) {
		for(; *s && (' ' == *s); s++);
		if (*s) {
			for(; *s && (',' != *s); s++);
			if (*s)
				s++;
			i++;
		}
	}

	return i;
}

void Cmdline_GetParam(char *CmdLine, WORD Index, char *Param, WORD ParamBufLength)
{
	WORD i, j;
	char *s, *p;

	for(s = CmdLine, p = Param, j = 1, i = 0; *s; ) {
		for(; *s && (' ' == *s); s++);
		if (*s) {
			for(; *s && (',' != *s); s++)
				if ((i == Index) && (j < ParamBufLength)) {
					*p++ = *s;
					j++;
				}
			if (*s)
				s++;
			i++;
		}
	}
	*p = 0;
}


#if 0
	if (p1 == p2)
		return 0;

	while (( (r = *p1 - *p2++) && (size > 0) ) == 0){
		_d_int( *p1 );
		_d_int( *p2 );
		_d_int( r );
		_d_int( size );
		if (*p1++ == 0)
			break;
		size -= 1;
	}

#else
int s_strncmp(char *p1, char *p2, int size)
{
	int r = 0;

	for(; *p1 && *p2 && size; p1++, p2++, size--){
		r = *p1 - *p2;
		//_d_int( r );
		//s_printf( "r:%d, *p1:%c, *p2:%c", r, *p1, *p2 );
		if( 0 != r )
			break;
	}

	return r;
}
#endif

int s_isdigit( char c )
{
	if( c >= '0' && c<= '9' )
		return 1;
	return 0;
}



