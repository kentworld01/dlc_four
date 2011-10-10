#ifndef RTC_H
#define RTC_H


#if (MODULE_DEVICE_RTC == 0)

#define RTC_Init()				
#define RTC_GetTime(x)				
#define RTC_SetTime(x)				

#else

void RTC_Init(void);
BOOL RTC_32K_Good(void);
void RTC_SelectSubClock(void);
void RTC_SelectFPClock(void);

void rtc_enable_write_access(void);
BOOL rtc_configuration(void);
void _rtc_set(void *date_time);

void RTC_GetTime(void *DateTime);
void RTC_SetTime(void *DateTime);

void rtc_display_time(void);

unsigned long RTC_Get(void);
void RTC_Set( unsigned long );

#endif


#endif
