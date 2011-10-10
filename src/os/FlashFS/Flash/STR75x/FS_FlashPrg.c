/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright KEIL ELEKTRONIK GmbH 2003 - 2006                         */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Flash Programming Functions adapted                   */
/*               for ST Microelectronics STR75x Flash                  */
/*                                                                     */
/***********************************************************************/

#include "File_Config.h"

// Flash Programming Registers
#define FCR0    (*((volatile unsigned long  *) 0x20100000))
#define FCR1    (*((volatile unsigned long  *) 0x20100004))
#define FDR0    (*((volatile unsigned long  *) 0x20100008))
#define FDR1    (*((volatile unsigned long  *) 0x2010000C))
#define FAR     (*((volatile unsigned long  *) 0x20100010))
#define FER     (*((volatile unsigned long  *) 0x20100014))
#define FNVWPAR (*((volatile unsigned long  *) 0x2010DFB0))
#define FNVAPR0 (*((volatile unsigned long  *) 0x2010DFB8))
#define FNVAPR1 (*((volatile unsigned long  *) 0x2010DFBC))

// FCRO Bit Masks
#define mWMS    0x80000000
#define mSUSP   0x40000000
#define mWPG    0x20000000
#define mDWPG   0x10000000
#define mSER    0x08000000
#define mSPR    0x01000000
#define mINTM   0x00200000
#define mINTP   0x00100000
#define mPWD    0x00008000
#define mLOCK   0x00000010
#define mBSY1   0x00000004
#define mBSY0   0x00000002

// FCR1 Bit Masks
#define mB0F0   0x00000001
#define mB0F1   0x00000002
#define mB0F2   0x00000004
#define mB0F3   0x00000008
#define mB0F4   0x00000010
#define mB0F5   0x00000020
#define mB0F6   0x00000040
#define mB0F7   0x00000080
#define mB1F0   0x00010000
#define mB1F1   0x00020000
#define mB0S    0x01000000
#define mB1S    0x02000000

#define mBS    (mB0F0 | mB0F1 | mB0F2 | mB0F3 | mB0F4 | mB0F5 | mB0F6 | mB0F7 | \
                mB1F0 | mB1F1 | mB0S  | mB1S)

// FER Bit Masks
#define mWPF    0x00000100
#define mRESER  0x00000080
#define mSEQER  0x00000040
#define m10ER   0x00000008
#define mPGER   0x00000004
#define mERER   0x00000002
#define mERR    0x00000001

#define mERROR (mERR | mERER | mPGER | m10ER | mSEQER | mRESER | mWPF)


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_Init (U32 adr, U32 clk) {

  FCR0  = 0x00000000;
  FCR1  = 0x00000000;
  FER   = 0x00000000;

  FCR0 |= 0x01000000;                          // Set Protection
  FAR   = 0x0010DFB0;                          // FLASH_NVWPAR Address
  FDR0  = 0xFFFFFFFF;                          // All Sectors Unprotected
  FCR0 |= mWMS;                                // Start Operation

  while (FCR0 & (mLOCK | mBSY0 | mBSY1));      // Wait until completed

  if ((FCR1 & mBS) || (FER &mERROR)) {
    FER = 0;                                   // Clear Errors
    return (1);                                // Failed
  }

  return (0);
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_EraseSector (U32 adr) {
  unsigned long n = 0;

  if (FCR0 & mLOCK) return (1);                // Flash is Locked

  if (adr < 0x20000000) return (1);            // Invalid Address
  if (adr > 0x200FFFFF) return (1);            // Invalid Address
  adr &= 0x000FFFFF;                           // Mask Address (1MB)

  n = (adr >> 13) & 0x01;                      // Sector Number
  n = 0x10000 << n;                            // 8kB Sector 0..1

  FCR0 |= mSER;                                // Sector Erase
  FCR1 |= n;                                   // Select Sectors
  FCR0 |= mWMS;                                // Start Erasing
  while (FCR0 & (mLOCK | mBSY0 | mBSY1));      // Wait until completed

  if ((FCR1 & mBS) || (FER &mERROR)) {
    FER = 0;                                   // Clear Errors
    return (1);                                // Failed
  }

  return (0);                                  // Done
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_ProgramPage (U32 adr, U32 sz, U8 *buf) {
  unsigned long i;

  if (FCR0 & mLOCK) return (1);                // Flash is Locked

  if (adr < 0x20000000) return (1);            // Invalid Address
  if (adr > 0x200FFFFF) return (1);            // Invalid Address
  adr &= 0x000FFFFF;                           // Mask Address (1MB)

  for (i = 0; i < ((sz+3)/4); i++)  {
    FCR0 |= mWPG;                              // Single Word Programming
    FAR   = adr;                               // Address
    adr  += 4;                                 // Next Address
    FDR0  = *((__packed unsigned long *)buf);  // First 32-bit Word
    buf  += 4;                                 // Go to next Word
    FCR0 |= mWMS;                              // Start Programming
    while (FCR0 & (mLOCK | mBSY0 | mBSY1));    // Wait until completed
  }

  if (FER &mERROR) {
    FER = 0;                                   // Clear Errors
    return (1);                                // Failed
  }

  return (0);                                  // Done
}
