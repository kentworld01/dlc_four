/*----------------------------------------------------------------------------
 *      R T L  -  F l a s h   F i l e   S y s t e m
 *----------------------------------------------------------------------------
 *      Name:    FS_SPI_FLASHPRG.C 
 *      Purpose: Flash Programming Functions - Intel Serial Flash S33 (SPI)
 *      Rev.:    V4.00
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2009 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <File_Config.h>

#define PAGE_SZ                 256     /* Page Size */


/* SPI Flash Commands */
#define SPI_WRITE_SR            0x01
#define SPI_PAGE_PROGRAM        0x02
#define SPI_READ_DATA           0x03
#define SPI_WRITE_DISABLE       0x04
#define SPI_READ_SR             0x05
#define SPI_WRITE_ENABLE        0x06
#define SPI_CLEAR_SR_FLAGS      0x30
#define SPI_SECTOR_ERASE        0xD8

/* Status Register Bits */
#define SR_WIP                  0x01
#define SR_E_FAIL               0x20
#define SR_P_FAIL               0x40


/*----------------------------------------------------------------------------
 *  Initialize Flash Programming Functions
 *    Return Value:   0 - Init OK
 *                    1 - Init Failed
 *---------------------------------------------------------------------------*/

int fs_spi_Init (void)  {

  /* Initialize SPI host interface. */
  spi_init ();

  spi_ss (0);
  spi_send (SPI_WRITE_ENABLE);
  spi_ss (1);

  spi_ss (0);
  spi_send (SPI_WRITE_SR);
  spi_send (0x00);
  spi_ss (1);
  spi_ss (0);
  spi_send (SPI_CLEAR_SR_FLAGS);
  spi_ss (1);

  return (0);
} /* end of fs_spi_Init */


/*----------------------------------------------------------------------------
 *  Erase Sector in Flash Memory.  
 *    Parameter:      adr: Sector Address
 *    Return Value:   0 - Erase OK
 *                    1 - Erase Failed
 *---------------------------------------------------------------------------*/

int fs_spi_EraseSector (U32 adr)  {
  U8  sr;

  /* Write Enable */
  spi_ss (0);
  spi_send (SPI_WRITE_ENABLE);
  spi_ss (1);

  /* Erase Sector */
  spi_ss (0);
  spi_send (SPI_SECTOR_ERASE);
  spi_send ((U8)(adr >> 16));
  spi_send ((U8)(adr >>  8));
  spi_send ((U8)(adr >>  0));
  spi_ss (1);

  /* Wait until done */
  spi_ss (0);
  spi_send (SPI_READ_SR);
  do {
    sr = spi_send (0xFF);
  } while (sr & SR_WIP);
  spi_ss (1);

  /* Check for Error */
  if (sr & SR_E_FAIL) {
    spi_ss (0);
    spi_send (SPI_CLEAR_SR_FLAGS);
    spi_ss (1);
    return (1);
  }

  return (0);
} /* end of fs_spi_EraseSector */


/*----------------------------------------------------------------------------
 *  Program Page in Flash Memory.  
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - Program OK
 *                    1 - Program failed
 *---------------------------------------------------------------------------*/

int fs_spi_ProgramPage (U32 adr, U32 sz, U8 *buf)  {
  U32 cnt;
  U8  sr;

  while (sz) {
    cnt = PAGE_SZ - (adr & (PAGE_SZ - 1));
    if (cnt > sz) cnt = sz;

    /* Write Enable */
    spi_ss (0);
    spi_send (SPI_WRITE_ENABLE);
    spi_ss (1);

    /* Program Page */
    spi_ss (0);
    spi_send (SPI_PAGE_PROGRAM);
    spi_send ((U8)(adr >> 16));
    spi_send ((U8)(adr >>  8));
    spi_send ((U8)(adr >>  0));
    adr += cnt;
    sz  -= cnt;
    while (cnt--) {
      spi_send (*buf++);
    }
    spi_ss (1);

    /* Wait until done */
    spi_ss (0);
    spi_send (SPI_READ_SR);
    do {
      sr = spi_send (0xFF);
    } while (sr & SR_WIP);
    spi_ss (1);

    /* Check for Error */
    if (sr & SR_P_FAIL) {
      spi_ss (0);
      spi_send (SPI_CLEAR_SR_FLAGS);
      spi_ss (1);
      return (1);
    }
  }

  return (0);
} /* end of fs_spi_ProgramPage */


/*----------------------------------------------------------------------------
 *  Read Data from Flash Memory.  
 *    Parameter:      adr:  Start Address
 *                    sz:   Size
 *                    buf:  Data
 *    Return Value:   0 - Read OK
 *                    1 - Read failed
 *---------------------------------------------------------------------------*/

int fs_spi_ReadData (U32 adr, U32 sz, U8 *buf)  {

  /* Read Data */
  spi_ss (0);
  spi_send (SPI_READ_DATA);
  spi_send ((U8)(adr >> 16));
  spi_send ((U8)(adr >>  8));
  spi_send ((U8)(adr >>  0));
  while (sz--) {
    *buf++ = spi_send (0xFF);
  }
  spi_ss (1);

  return (0);
} /* end of fs_spi_ReadData */


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
