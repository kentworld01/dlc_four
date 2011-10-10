/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright KEIL ELEKTRONIK GmbH 2003 - 2006                         */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Flash Programming Functions adapted                   */
/*               for ST Microelectronics STR91x Flash                  */
/*                                                                     */
/***********************************************************************/

#include <file_config.h>                 // FlashOS Structures

#define M8(adr)  (*((volatile unsigned char  *) (adr)))
#define M16(adr) (*((volatile unsigned short *) (adr)))
#define M32(adr) (*((volatile unsigned long  *) (adr)))

#define STACK_SIZE      64              // Stack Size


#define BANK0_ADR       0               // Bank0 Address

#ifdef STR91xFxx2
#define BANK0_SZ        3               // Bank0 Size = 256kB
#define BANK0_SEC_CNT   4               // Bank0 Sector Count
#endif
#ifdef STR91xFxx4
#define BANK0_SZ        4               // Bank0 Size = 512kB
#define BANK0_SEC_CNT   8               // Bank0 Sector Count
#endif

#define BANK0_SEC_SZ    0x10000         // Bank0 Sector Size

#define BANK1_ADR       0x400000        // Bank1 Address
#define BANK1_SZ        2               // Bank1 Size =  32kB
#define BANK1_SEC_CNT   4               // Bank1 Sector Count
#define BANK1_SEC_SZ    0x2000          // Bank1 Sector Size

#ifdef BANK0
#define BANK_ADR        BANK0_ADR       // Bank Address
#define BANK_SEC_CNT    BANK0_SEC_CNT   // Bank Sector Count
#define BANK_SEC_SZ     BANK0_SEC_SZ    // Bank Sector Size
#define BANK_SEC_MASK   ((1 << BANK0_SEC_CNT) - 1)
#endif

#ifdef BANK1
#define BANK_ADR        BANK1_ADR       // Bank Address
#define BANK_SEC_CNT    BANK1_SEC_CNT   // Bank Sector Count
#define BANK_SEC_SZ     BANK1_SEC_SZ    // Bank Sector Size
#define BANK_SEC_MASK   (((0x100 << BANK1_SEC_CNT) - 1) & ~0xFF)
#endif


// System Control Unit Registers
#define SCU_CLKCNTR     (*((volatile unsigned long  *) 0x5C002000))
#define SCU_SCR0        (*((volatile unsigned long  *) 0x5C002034))

// Flash Memory Interface Registers
#define FMI_BBSR        (*((volatile unsigned long  *) 0x54000000))
#define FMI_BBADR       (*((volatile unsigned long  *) 0x5400000C))
#define FMI_NBBSR       (*((volatile unsigned long  *) 0x54000004))
#define FMI_NBBADR      (*((volatile unsigned long  *) 0x54000010))
#define FMI_CR          (*((volatile unsigned long  *) 0x54000018))
#define FMI_SR          (*((volatile unsigned long  *) 0x5400001C))

// Flash Commands
#define CMD_PROT1CFM    0x01            // Protect Level 1 Confirm
#define CMD_SERS        0x20            // Sector Erase Set-up
#define CMD_PRGS        0x40            // Program Set-up
#define CMD_CLRSTAT     0x50            // Clear Status Register
#define CMD_PROT1S      0x60            // Protect Level 1 Set-up
#define CMD_RDSTAT      0x70            // Read Status Register
#define CMD_BNKERS      0x80            // Bank Erase Set-up
#define CMD_RSIG        0x90            // Read Electronic Signature
#define CMD_RDOTP       0x98            // Read OTP Sector
#define CMD_CFM         0xD0            // Prog/Ers Resume, Ers Confirm
                                        // Level 1 Unprotect Confirm
#define CMD_RDARR       0xFF            // Read Array

// Status register bits
#define PECS            0x80            // Prog/Ers Controller Status
#define ESS             0x40            // Erase Suspend Status
#define ES              0x20            // Erase Status
#define PS              0x10            // Program Status
#define PSS             0x04            // Program Suspend Status
#define SP              0x02            // Sector Protection Status


unsigned long base_adr;


/*
 *  Delay
 */

void Delay (void) {
  int i;

  for (i = 0; i < 10; i++);
}


/*
 *  Wait for last operation with Flash to finish and return status
 *    Parameter:      adr:  Address on which operation is running
 *    Return Value:   status
 */

unsigned char WaitWithStatus (unsigned long adr) {
  unsigned char status;

  while (!(M16(adr) & PECS));           // Wait for operation finish

  status   = M16(adr);                  // Read status

  M16(adr) = CMD_CLRSTAT;               // Clear Status Register
  M16(adr) = CMD_RDARR;                 // exit Read Status Register mode

  return (status);
}


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_Init (U32 adr, U32 clk) {
  int i, j;

  base_adr = adr;

  // Setup System Control Unit
// MG 
// MG  SCU_CLKCNTR = 0x00020002;             // Main Clock Source is Oscillator
// MG  SCU_SCR0   &=~0x00000001;             // PFQBC Unit Disable

  // Setup Flash Memory Interface
  FMI_BBSR    = BANK0_SZ;
  FMI_BBADR   = BANK0_ADR >> 2;
  FMI_NBBSR   = BANK1_SZ;
  FMI_NBBADR  = BANK1_ADR >> 2;
  FMI_CR      = 0x00000018;             // Enable Bank 0 & 1

  // Clear Level 1 Protection (unprotect all sectors)
  for (i = 0, j = BANK_ADR; i < BANK_SEC_CNT; i++, j += BANK_SEC_SZ) {
    M16(j) = CMD_PROT1S;
    M16(j) = CMD_CFM;
    M16(j) = CMD_RDARR;
    Delay();
  }

  // Check if all sectors are unprotected
  M16(BANK1_ADR)  = CMD_RSIG;
  Delay();
  i = M16(BANK1_ADR + 0x10) & BANK_SEC_MASK;
  M16(BANK1_ADR)  = CMD_RDARR;          // Leave RSIG mode

  if (i)
    return (1);                         // Not unprotected

  return (0);
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int fs_EraseSector (U32 adr) {


  // Issue Erase Sector procedure
  M16(adr) = CMD_SERS;
  M16(adr) = CMD_CFM;

  if (WaitWithStatus(adr) & (ES | SP))  // Erase unsuccessful
    return (1);

  return (0);                           // Done
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

  for (i = 0; i < ((sz+1)/2); i++)  {
    M16(adr & ~3) = CMD_PRGS;           // Write Program Set-up Command
    M16(adr) = *((__packed unsigned short *)buf);// Write 2 byte data
    if (WaitWithStatus(adr & ~3) & (PS | SP))    // Program unsuccessful
      return (1);
    buf += 2;
    adr += 2;
  }

  return (0);                           // Done successfully
}
