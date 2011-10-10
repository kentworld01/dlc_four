/*----------------------------------------------------------------------------
 *      R T L  -  F l a s h   F i l e   S y s t e m
 *----------------------------------------------------------------------------
 *      Name:    FS_SPI_FLASHPRG.C 
 *      Purpose: Flash Programming Functions - Atmel Data Flash AT45DB321C (SPI)
 *      Rev.:    V4.00
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2007 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <File_Config.h>

#define PAGE_SZ         528             /* Page size                   */
#define BLOCK_SZ        (8*PAGE_SZ)     /* Block size (used for erase) */
#define SECT_SZ         (16*BLOCK_SZ)   /* Virtual sector size         */

/* SPI Data Flash Commands */
#define SPI_READ_DATA           0xE8
#define SPI_READ_STATUS         0xD7
#define SPI_BLOCK_ERASE         0x50
#define SPI_BUF1_WRITE          0x84
#define SPI_PAGE_PROGRAM        0x83
#define SPI_PAGE_READ           0x53


/* Local functions */
static void set_adr (U8 *cmd, U32 adr);
static void spi_write (U8 cmnd, U8 cmndlen, U32 adr, U8 *buf, U32 sz);

/*----------------------------------------------------------------------------
 *  Initialize Flash Programming Functions
 *    Return Value:   0 - Init OK
 *                    1 - Init Failed
 *---------------------------------------------------------------------------*/
int fs_spi_Init (void)  {
   /* Initialize SPI host interface. */
   spi_init ();

   return (0);
} /* end of fs_spi_Init */


/*----------------------------------------------------------------------------
 *  Erase Sector in Flash Memory.  
 *    Parameter:      adr: Virtual Sector Address
 *    Return Value:   0 - Erase OK
 *                    1 - Erase Failed
 *---------------------------------------------------------------------------*/
int fs_spi_EraseSector (U32 adr)  {
   U32 i;

   /* Virtual sector size is 16 blocks. */
   for (i = 0; i < (SECT_SZ/BLOCK_SZ); i++) {
      spi_write (SPI_BLOCK_ERASE, 4, adr, NULL, 0);
      adr += BLOCK_SZ;
   }
   return (0);
} /* end of fs_spi_EraseSector */


/*----------------------------------------------------------------------------
 *  Program Page in Flash Memory.  
 *    Parameter:      adr:  Start Address
 *                    sz:   Size
 *                    buf:  Data
 *    Return Value:   0 - Program OK
 *                    1 - Program failed
 *---------------------------------------------------------------------------*/
int fs_spi_ProgramPage (U32 adr, U32 sz, U8 *buf)  {
   U32 cnt;

   while (sz) {
      /* Read Flash Page to Buf 1. */
      spi_write (SPI_PAGE_READ, 4, adr, NULL, 0);

      cnt = PAGE_SZ - (adr % PAGE_SZ);
      if (cnt > sz) cnt = sz;
      spi_write (SPI_BUF1_WRITE, 4, adr, buf, cnt);
      spi_write (SPI_PAGE_PROGRAM, 4, adr, buf, PAGE_SZ);

      adr += cnt;
      buf += cnt;
      sz  -= cnt;
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
   U8  adrByte[3];
   U32 i;

   set_adr (&adrByte[0], adr);

   spi_ss (0);
   spi_send (SPI_READ_DATA);
   spi_send (adrByte[0]);
   spi_send (adrByte[1]);
   spi_send (adrByte[2]);
   for (i = 0; i < 4; i++) {
      spi_send (0xFF);               /* 4 bytes dont care */
   }
   for (i = 0; i < sz; i++) {
      *(buf+i) = spi_send (0xFF);
   }
   spi_ss (1);

   return (0);
} /* end of fs_spi_ReadData */


/*----------------------------------------------------------------------------
 *  Send SPI Command to Data Flash.
 *---------------------------------------------------------------------------*/
static void spi_write (U8 cmnd, U8 cmndlen, U32 adr, U8 *buf, U32 sz) {
   U8  adrByte[3];
   U8  sr;
   U32 i;

   set_adr (&adrByte[0], adr);

   switch (cmnd) {
      case SPI_BLOCK_ERASE:
      case SPI_PAGE_READ:
      case SPI_PAGE_PROGRAM:
         spi_ss (0);
         spi_send (cmnd);
         spi_send (adrByte[0]);
         spi_send (adrByte[1]);
         spi_send (adrByte[2]);
         spi_ss (1);
         break;

     case SPI_BUF1_WRITE:
         spi_ss (0);
         spi_send (cmnd);
         spi_send (adrByte[0]);
         spi_send (adrByte[1]);
         spi_send (adrByte[2]);
         for (i = 0; i < sz; i++) {
            spi_send (*(buf+i));
         }
         spi_ss (1);
         break;
   }

   /* Wait while device is Busy. */
   spi_ss (0);
   spi_send (SPI_READ_STATUS);
   spi_send (0xFF);                /* dummy byte */
   do {
      sr = spi_send (0xFF);
   } while ((sr & 0x80) == 0);
   spi_ss (1);
}

/*----------------------------------------------------------------------------
 *  Convert linear address to segmented Data Flash address
 *---------------------------------------------------------------------------*/
static void set_adr (U8 *cmd, U32 adr) {
   U32 bl_adr, pg_adr;

   bl_adr = adr / PAGE_SZ;
   pg_adr = adr % PAGE_SZ;

   adr = (bl_adr << 10) | pg_adr;
   cmd[0] = (U8)(adr >> 16);
   cmd[1] = (U8)(adr >> 8 );
   cmd[2] = (U8)(adr >> 0 );
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
