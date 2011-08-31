
#ifndef _d_lcd_h_
#define _d_lcd_h_

#include "typedef.h"
#include <RTL.h>


#define LCD_INTERFACE_8080 1
#define LCD_INTERFACE_6800 0

#define LCD_SEL() LCD_CS1_SEL()
#define LCD_BL_SET() LCD_BL1_SET()
#define LCD_BL_CLR() LCD_BL1_CLR()

#define LCDS_BUSY					(1 << 7)
#define LCDS_DISPLAYOFF			(1 << 5)
#define LCDS_RESET					(1 << 4)
#define __LCD_WIDTH_LIMIT				(132)
#define __LCD_HEIGHT_LIMIT			(64)
#define __LCD_FB_X_BYTES				((__LCD_WIDTH_LIMIT / 8) + ((__LCD_WIDTH_LIMIT % 8) ? 1 : 0))
#define __LCD_FB_Y_PAGES				((__LCD_HEIGHT_LIMIT / 8) + ((__LCD_HEIGHT_LIMIT % 8) ? 1 : 0))
#define __LCD_FRAME_BUFFER_SIZE		(__LCD_FB_X_BYTES * __LCD_HEIGHT_LIMIT)
#define LCD_WIDTH_LIMIT				(128)
#define LCD_HEIGHT_LIMIT				(64)
#define LCD_FB_X_BYTES					((LCD_WIDTH_LIMIT / 8) + ((LCD_WIDTH_LIMIT % 8) ? 1 : 0))
#define LCD_FB_Y_PAGES					((LCD_HEIGHT_LIMIT / 8) + ((LCD_HEIGHT_LIMIT % 8) ? 1 : 0))
#define LCD_FRAME_BUFFER_SIZE			(LCD_FB_X_BYTES * LCD_HEIGHT_LIMIT)
extern BYTE *FrameBuffer;
#define LCD_GETFBPOINTER(x, yp)		(FrameBuffer + (DWORD)__LCD_WIDTH_LIMIT * (DWORD)yp + (DWORD)x)
#define LCD_Busy()				do { \
									int i; \
									for(i = 0; (LCD_Status() & LCDS_BUSY) && (i < 888); i++); \
									}while(0)


int DB_DirIn(void);
int DB_DirOut(void);
int DB_RD_SET(void);
int DB_RD_CLR(void);
int DB_WR_SET(void);
int DB_WR_CLR(void);
int DB_CLE_SET(void);
int DB_CLE_CLR(void);
BYTE DB_Read(void);
void DB_Write(BYTE x);
int LCD_CS1_SEL(void);
int LCD_CS1_UNSEL(void);
int LCD_CS2_SEL(void);
int LCD_CS2_UNSEL(void);
int LCD_UNSEL(void);
int LCD_BL1_SET(void);
int LCD_BL1_CLR(void);
//int LCD_BL2_SET();
//int LCD_BL2_CLR();
int LCD_RS_SET(void);
int LCD_RS_CLR(void);
int LCD_RW_SET(void);
int LCD_RW_CLR(void);
int LCD_E_SET(void);
int LCD_E_CLR(void);
int LCD_RD_SET(void);
int LCD_RD_CLR(void);
int LCD_WR_SET(void);
int LCD_WR_CLR(void);
int LCD_RST_SET(void);
int LCD_RST_CLR(void);
BYTE LCD_Status(void);
int CPU_Enter_Critical(void);
int CPU_Exit_Critical(void);
void _LCD_FBClear(BYTE BackGround);
void LCD_Insturction(BYTE Insturction);
BYTE LCD_Read(void);
void LCD_Write(BYTE Data);
BOOL _lcd_write_and_read_check(void);
void _LCD_FBTransmit(void);
BOOL _LCD_FBVerify(void);
void LCD_DisplayOn(BOOL On);
void LCD_SideLightOn(BOOL On);
BOOL _LCD_Init(void);
void LCD_On(BOOL On);
void LCD_Refresh(void);
void LCD_CLS(BYTE BackGround);
BOOL LCD_Init(BOOL KeepFB);
void delay(unsigned int i);
void delayms (unsigned int i) ;
void	delay1s(unsigned char i);
int lcd_init(void);


#endif
