/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright KEIL ELEKTRONIK GmbH 2003 - 2007                         */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Flash Programming Functions adapted                   */
/*               for Philips LPC2xxx 128kB Flash                       */
/*               using Flash Boot Loader with IAP                      */
/*                                                                     */
/***********************************************************************/

#include <File_Config.h>

#define PAGE_SZ 1024           // Page Size

U32 Page[PAGE_SZ/4];           // Page Buffer

U32 CCLK;                      // CCLK in kHz

struct sIAP {                  // IAP Structure
  U32 cmd;                     // Command
  U32 par[4];                  // Parameters
  U32 stat;                    // Status
} IAP;


/*
 * IAP Execute
 *    Parameter:      pIAP:  Pointer to IAP
 *    Return Value:   None (stat in IAP)
 */

void IAP_Execute (struct sIAP *pIAP);


/*
 * Get Sector Number
 *    Parameter:      adr:  Sector Address
 *    Return Value:   Sector Number
 */

U32 GetSecNum (U32 adr) {
  U32 n;

  n = (adr >> 13) & 0x1F;                      // Sector Number
  return (n);
}


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_Init (U32 adr, U32 clk)  {

  adr   = adr;

//CCLK /=  1000;                               // Clock Frequency in kHz
  CCLK  = (1049*(clk >> 10)) >> 10;            // Approximate (no Library Code)

  return (0);
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_EraseSector (U32 adr) {
  U32 n;

  n = GetSecNum(adr);                          // Get Sector Number

  IAP.cmd    = 50;                             // Prepare Sector for Erase
  IAP.par[0] = n;                              // Start Sector
  IAP.par[1] = n;                              // End Sector
  IAP_Execute (&IAP);                          // Execute IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  IAP.cmd    = 52;                             // Erase Sector
  IAP.par[0] = n;                              // Start Sector
  IAP.par[1] = n;                              // End Sector
  IAP.par[2] = CCLK;                           // CCLK in kHz
  IAP_Execute (&IAP);                          // Execute IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  return (0);                                  // Finished without Errors
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_ProgramPage (U32 adr, U32 sz, U8 *buf) {
  U32 padr, ofs, cnt, n;

  IAP.cmd    = 50;                             // Prepare Sector for Write
  IAP.par[0] = GetSecNum(adr);                 // Start Sector
  IAP.par[1] = GetSecNum(adr + sz - 1);        // End Sector
  IAP_Execute (&IAP);                          // Execute IAP Command
  if (IAP.stat) return (1);                    // Command Failed

  while (sz) {                                 // Go through all Data
    padr = adr & ~(PAGE_SZ - 1);               // Page Address
    ofs  = adr - padr;                         // Data Offset
    cnt  = PAGE_SZ - ofs;                      // Data Count
    if (cnt > sz) cnt = sz;                    // Adjust Data Count
    if (cnt != PAGE_SZ) {                      // Incomplete Page being written
      for (n = 0; n < PAGE_SZ/4; n++) {        // Go through complete Page
        Page[n] = *((U32 *)padr + n);          // Read Page Data from Flash
      }
    }
    for (n = 0; n < cnt; n++) {                // Go through Page Data
      *((U8 *)Page + ofs++) = *buf++;          // Copy & Align to Page Buffer
    }

    IAP.cmd    = 51;                           // Copy RAM to Flash
    IAP.par[0] = padr;                         // Destination Flash Address
    IAP.par[1] = (U32)Page;                    // Source RAM Address
    IAP.par[2] = PAGE_SZ;                      // Page Size
    IAP.par[3] = CCLK;                         // CCLK in kHz
    IAP_Execute (&IAP);                        // Execute IAP Command
    if (IAP.stat) return (1);                  // Command Failed

    adr += cnt;                                // Next Address
    sz  -= cnt;                                // Next Size
  }

  return (0);                                  // Finished without Errors
}
