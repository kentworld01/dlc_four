/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright KEIL ELEKTRONIK GmbH 2003 - 2007                         */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Flash Programming Functions adapted                   */
/*               for Dual AM29x800BB (32-bit Bus)                      */
/*                                                                     */
/***********************************************************************/

#include <File_Config.h>

#define M8(adr)  (*((volatile unsigned char  *) (adr)))
#define M16(adr) (*((volatile unsigned short *) (adr)))
#define M32(adr) (*((volatile unsigned long  *) (adr)))


union fsreg {                  // Flash Status Register
  struct b  {
    unsigned int q0l:1;
    unsigned int q1l:1;
    unsigned int q2l:1;
    unsigned int q3l:1;
    unsigned int q4l:1;
    unsigned int q5l:1;
    unsigned int q6l:1;
    unsigned int q7l:1;
    unsigned int rl:8;
    unsigned int q0h:1;
    unsigned int q1h:1;
    unsigned int q2h:1;
    unsigned int q3h:1;
    unsigned int q4h:1;
    unsigned int q5h:1;
    unsigned int q6h:1;
    unsigned int q7h:1;
    unsigned int rh:8;
  } b;
  unsigned int v;
} fsr;

unsigned long base_adr;


/*
 * Check if Program/Erase completed
 *    Parameter:      adr:  Block Start Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int Polling (unsigned long adr) {
  unsigned int q6l, q6h;

  fsr.v = M32(adr);
  q6l = fsr.b.q6l;
  q6h = fsr.b.q6h;
  do {
    fsr.v = M32(adr);
    if ((fsr.b.q6l == q6l) && (fsr.b.q6h == q6h)) {
      return (0);                  // Done
    }
    q6l = fsr.b.q6l;
    q6h = fsr.b.q6h;
  } while ((fsr.b.q5l == 0) || (fsr.b.q5h == 0));  // Check for Timeout
  fsr.v = M32(adr);
  q6l = fsr.b.q6l;
  q6h = fsr.b.q6h;
  fsr.v = M32(adr);
  if ((fsr.b.q6l == q6l) && (fsr.b.q6h == q6h)) {
    return (0);                    // Done
  }
  M32(adr) = 0x00F000F0;           // Reset Device
  return (1);                      // Failed
}


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_Init (U32 adr, U32 clk)  {
  clk      = clk;
  base_adr = adr;
  return (0);
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_EraseSector (U32 adr) {

  // Start Erase Sector Command
  M32(base_adr + (0xAAA << 1)) = 0x00AA00AA;
  M32(base_adr + (0x554 << 1)) = 0x00550055;
  M32(base_adr + (0xAAA << 1)) = 0x00800080;
  M32(base_adr + (0xAAA << 1)) = 0x00AA00AA;
  M32(base_adr + (0x554 << 1)) = 0x00550055;
  M32(adr) = 0x00300030;

  do {                             // Wait for Sector Erase Timeout
    fsr.v = M32(adr);
  } while ((fsr.b.q3l == 0) || (fsr.b.q3h == 0));

  return (Polling(adr));           // Wait until Erase completed
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_ProgramPage (U32 adr, U32 sz, U8 *buf) {
  int  i;

  for (i = 0; i < ((sz+3)/4); i++)  {
    // Start Program Command
    M32(base_adr + (0xAAA << 1)) = 0x00AA00AA;
    M32(base_adr + (0x554 << 1)) = 0x00550055;
    M32(base_adr + (0xAAA << 1)) = 0x00A000A0;
    // 'buf' might be unaligned
    M32(adr) = *((__packed unsigned long *) buf);
    if (Polling(adr)) return (1);  // Wait until Programming completed
    buf += 4;
    adr += 4;
  }
  return (0);                      // Done
}
