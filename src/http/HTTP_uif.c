
/*----------------------------------------------------------------------------
 *      R T L  -  T C P N E T
 *----------------------------------------------------------------------------
 *      Name:    HTTP_UIF.C
 *      Purpose: HTTP Server User Interface Module
 *      Rev.:    V3.80
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2009 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include "Net_Config.h"
#include "os.h"

/*----------------------------------------------------------------------------
 * HTTP Server File Access CallBack Functions
 *---------------------------------------------------------------------------*/

/*--------------------------- http_fopen ------------------------------------*/

void *http_fopen (U8 *fname) {
   /* Open file 'fname' for reading. Return file handle. */
#if _d_sd
   return (fopen ((const char *)fname, "r"));
#elif _d_uffs
   {
	   char fn[64];
	   int fno = 0;
	   strcpy( fn, "/" );
	   strcat( fn, fname );
	   fno = uffs_open( fn, UO_RDONLY );
	   if( fno == -1 )
		   return NULL;
	   return (void*)fno;
   }
#else
   return (void*)-1;
#endif
}


/*--------------------------- http_fclose -----------------------------------*/

void http_fclose (void *f) {
#if _d_sd
   /* Close the file opened for reading. */
   fclose (f);
#elif _d_uffs
   uffs_close( (u32)f );
#endif
}


/*--------------------------- http_fread ------------------------------------*/

U16 http_fread (void *f, U8 *buf, U16 len) {
#if _d_sd
   /* Read 'len' bytes from file to buffer 'buf'. The file will be closed, */
   /* when the number of read bytes is less than 'len'. */
   return (fread (buf, 1, len, f));
#elif _d_uffs
   return (uffs_read ((u32)f, buf, len));
#endif
}


/*--------------------------- http_fgets ------------------------------------*/

BOOL http_fgets (void *f, U8 *buf, U16 size) {
#if _d_sd
   /* Read a string from file to buffer 'buf'. The file will be closed, */
   /* when this function returns __FALSE. */
   if (fgets ((char *)buf, size, f) == NULL) {
      return (__FALSE);
   }
   return (__TRUE);
#elif _d_uffs
      return (__FALSE);
#endif
      return (__FALSE);
}

/*--------------------------- http_finfo ------------------------------------*/

U32 http_finfo (U8 *fname) {
#if _d_sd
   /* Read last modification time of a file. Return lm time in UTC format. */
   FINFO *info;
   U32 utc;

   info = (FINFO *)alloc_mem (sizeof (FINFO));
   info->fileID = 0;
   utc = 0;
   if (ffind ((const char *)fname, info) == 0) {
      /* File found, save creation date in UTC format. */
      utc = http_date (&info->time);
   }
   free_mem ((OS_FRAME *)info);
   return (utc);
#elif _d_uffs
	{
   		U32 utc = 0;
		char fn[64];
		struct uffs_stat us;
	   	strcpy( fn, "/" );
	   	strcat( fn, fname );
		if( uffs_stat( fn, &us ) == 0 )
			utc = us.st_mtime;
		return utc;
	}
#endif
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
