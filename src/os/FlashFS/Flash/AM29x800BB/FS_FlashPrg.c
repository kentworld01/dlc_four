/*----------------------------------------------------------------------------
 *      R T L  -  F l a s h   F i l e   S y s t e m
 *----------------------------------------------------------------------------
 *      Name:    FS_FLASHPRG.C 
 *      Purpose: Flash Programming Functions - Am29x800BB (16-bit Bus)
 *      Rev.:    V4.00
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2009 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <File_Config.h>

#define M8(adr)  (*((volatile U8  *) (adr)))
#define M16(adr) (*((volatile U16 *) (adr)))
#define M32(adr) (*((volatile U32 *) (adr)))

U32 base_adr;

/* Embedded Flash programming/erase commands   */

#define RESET       0x00F0              /* reset command                     */
#define ERASE       0x0080              /* erase command                     */
#define ERA_SECT    0x0030              /* erase sector command              */
#define PROGRAM     0x00A0              /* write command                     */

#define DQ7         0x0080              /* Flash Status Register bits        */
#define DQ6         0x0040
#define DQ5         0x0020
#define DQ3         0x0008

/*----------------------------------------------------------------------------
 * Check if Program/Erase completed
 *    Parameter:      adr:  Block Start Address
 *    Return Value:   0 - OK
 *                    1 - Operation Failed
 *---------------------------------------------------------------------------*/

static int Q6Polling (U32 adr) {
   U32 fsreg;
   U32 dqold;
  
   fsreg = M16(adr);
   do {
      dqold = fsreg & DQ6;
      fsreg = M16(adr);
      if ((fsreg & DQ6) == dqold) {
         /* OK, Done. */
         return (0);
      }
      dqold = fsreg & DQ6;
   } while ((fsreg & DQ5) < DQ5);
   fsreg = M16(adr);
   dqold = fsreg & DQ6;
   fsreg = M16(adr);
   if ((fsreg & DQ6) == dqold) {
      /* OK, Done */
      return (0);
   }
   /* Error, Reset Flash Device */
   M16(adr) = RESET;
   return (1);
} /* end of Q6Polling */


/*----------------------------------------------------------------------------
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *    Return Value:   0 - Init OK
 *                    1 - Init Failed
 *---------------------------------------------------------------------------*/

int fs_Init (U32 adr, U32 clk)  {
   /* Supress compiler warning of unreferenced 'clk' */
   clk      = clk;
   base_adr = adr;
   return (0);
}


/*----------------------------------------------------------------------------
 *  Erase Sector in Flash Memory.  
 *    Parameter:      adr: Sector Address
 *    Return Value:   0 - Erase OK
 *                    1 - Erase Failed
 *---------------------------------------------------------------------------*/

int fs_EraseSector (U32 adr)  {
   U32 fsreg;

   M16(base_adr | 0xAAA) = 0xAA;
   M16(base_adr | 0x554) = 0x55;
   M16(base_adr | 0xAAA) = ERASE;
   M16(base_adr | 0xAAA) = 0xAA;
   M16(base_adr | 0x554) = 0x55;
   M16(adr) = ERA_SECT;

   /* Wait for Sector Erase Timeout. */
   do {
      fsreg = M16(adr);
   } while ((fsreg & DQ3) < DQ3);

   /* Wait until Erase Completed */
   return (Q6Polling (adr));
} /* end of fs_EraseSector */


/*----------------------------------------------------------------------------
 *  Program Page in Flash Memory.  
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - Program OK
 *                    1 - Program failed
 *---------------------------------------------------------------------------*/

int fs_ProgramPage (U32 adr, U32 sz, U8 *buf)  {

   for (  ; sz; sz -= 2, adr += 2, buf += 2)  {
      M16(base_adr | 0xAAA) = 0xAA;
      M16(base_adr | 0x554) = 0x55;
      M16(base_adr | 0xAAA) = PROGRAM;
      /* 'buf' might be unaligned. */
      M16(adr) = *(__packed U16 *)buf;

      /* Wait until Programming completed */
      if (Q6Polling (adr)) {
         return (1);
      }
   }
   return (0);
} /* end of fs_ProgramPage */


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

 
