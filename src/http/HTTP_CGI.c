/*----------------------------------------------------------------------------
 *      R T L  -  T C P N E T
 *----------------------------------------------------------------------------
 *      Name:    HTTP_CGI.C
 *      Purpose: HTTP Server CGI Module
 *      Rev.:    V3.22
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "os.h"
#include <Net_Config.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <RTL.h>


/* ---------------------------------------------------------------------------
 * The HTTP server provides a small scripting language.
 *
 * The script language is simple and works as follows. Each script line starts
 * with a command character, either "i", "t", "c", "#" or ".".
 *   "i" - command tells the script interpreter to "include" a file from the
 *         virtual file system and output it to the web browser.
 *   "t" - command should be followed by a line of text that is to be output
 *         to the browser.
 *   "c" - command is used to call one of the C functions from the this file.
 *         It may be followed by the line of text. This text is passed to
 *         'cgi_func()' as a pointer to environment variable.
 *   "#' - command is a comment line and is ignored (the "#" denotes a comment)
 *   "." - denotes the last script line.
 *
 * --------------------------------------------------------------------------*/


/* Local variables. */
static S8 label[12] = "SD_CARD";

/* My structure of CGI status U32 variable. This variable is private for */
/* each HTTP Session and is not altered by HTTP Server. It is only set to  */
/* zero when the cgi_func() is called for the first time.                  */
typedef struct {
	U16 xcnt;
	U16 unused;
	uffs_DIR *d;
} MY_BUF;
#define MYBUF(p)        ((MY_BUF *)p)

/* Local functions */
static void dot_format (U32 val, char *sp);

/*----------------------------------------------------------------------------
 * HTTP Server Common Gateway Interface Functions
 *---------------------------------------------------------------------------*/

/*--------------------------- cgi_process_var -------------------------------*/

void cgi_process_var (U8 *qs) 
{
	/* This function is called by HTTP server to process the Querry_String   */
	/* for the CGI Form GET method. It is called on SUBMIT from the browser. */
	/*.The Querry_String.is SPACE terminated.                                */
	int kc;
	char *ki[ 30 ];
	int skc;
	char *ski[ 10 ];

	_d_line();
	kc = analysis_string_to_strings_by_decollator( (char*)qs, "& ", ki, 30 );
	if( kc < 1 )
		return ;

	// T command
	if( ki[0][0] == 'T' && ki[0][1] == ',' ){
		// ready T command
		t_cmd_main( ki[0] );
	}

	// other command
	skc = analysis_string_to_strings_by_decollator( ki[0], "= ", ski, 10 );
	if( skc < 1 )
		return ;
	if( strcmp( ski[0], "demo" ) == 0 ){
		// sub command set.
		//int demo_test(char**);
		_d_str( ski[1] );
		demo_test( ski );
	}
}


/*--------------------------- cgi_process_data ------------------------------*/

void cgi_process_data (U8 code, U8 *dat, U16 len) 
{
	/* This function is called by HTTP server to process the returned Data    */
	/* for the CGI Form POST method. It is called on SUBMIT from the browser. */
	/* Parameters:                                                            */
	/*   code  - callback context code                                        */
	/*           0 = www-url-encoded form data                                */
	/*           1 = filename for file upload (0-terminated string)           */
	/*           2 = file upload raw data                                     */
	/*           3 = end of file upload (file close requested)                */
	/*   dat   - pointer to POST received data                                */
	/*   len   - received data length                                         */
	static FILE *f = NULL;
	U8 *var,*p;
	U32 n;
	BOOL do_format;
	char fn[64];

	switch (code) {
		case 0:
			/* Url encoded form data received. */
			break;

		case 1:
			/* Filename for file upload received as encoded by the browser. */
			/* It might contain an absolute path to a file from the sending */
			/* host. Open a file for writing. */
			var = dat;
			/* Remove path info from filename, keep only the name. */
			for (p = dat; *p; p++) {
				if (*p == '\\') var = p + 1;
			}
			if (*var == 0) {
				/* Send was clicked but a filename was not selected. */
				return;
			}
			/* Filename is OK, initialize the card. */
#if _d_sd
			if (finit() != 0) {
				f = NULL;
				return;
			}
			/* Files will be stored to the root directory of SD card. */
			f = fopen ((const char *)var,"w");
#elif _d_uffs
			if( strcmp( (char*)var, "fonts.dat" ) == 0 ){
				u32 addr;
				f = (FILE*) (void*)(3*1024*1024 + 64*1024); // set 3 m address
				// erase 3m - 4m block
				for( n=0; n<16; n++ ){
					addr = ( 3*1024*1024 + 64*1024*n );
					W25X_BlockErase( addr );
					W25X_ReadData( addr, fn, sizeof(fn) );
				}
			}
			else{
				strcpy( fn, "/" );
				strcat( fn, (char*)var );
				f = (FILE*)uffs_open( fn, UO_RDWR|UO_CREATE);
			}
#else
			f = NULL;
#endif
			return;

		case 2:
			/* File content data received. Write data to a file. */
			/* This function will be called several times with   */
			/* code 2 when a big file is being uploaded.         */

#if _d_sd
			if (f != NULL) 
#elif _d_uffs
			if ( f != (FILE*)-1 ) 
#else
			return;
#endif
			{
				/* Write in 512 byte blocks. This is the optimal way for */
				/* the FAT FS with caching enabled. For cache buffer     */
				/* size of 4KB the file write speed is 1.2 MByte/sec.    */
				while ((n = len) > 0) {
					if (n > 512) n = 512;
#if _d_sd
					fwrite (dat,1,n,f);
#elif _d_uffs
					if( 3*1024*1024 <= (u32)f && (u32)f < 4*1024*1024 ){
						// in 3m - 4m 
						W25X_ReadData( 0x310000, fn, 64 );
						w25x_program( (u32)f, dat, n );
						f = (FILE*)( (u32)f + n );
						W25X_ReadData( 0x310000, fn, 64 );
					}
					else{
						uffs_write ((int)f,dat,n);
					}
#endif
					dat += n;
					len -= n;
				}
			}
			return;

		case 3:
			/* File upload finished. Close a file. */
#if _d_sd
			if (f != NULL) {
				fclose (f);
			}
#elif _d_uffs
			if( f != (FILE*)-1 )
				uffs_close( (int)f );
#endif
			return;

		default:
			return;
	}

	if (len == 0) {
		/* No data or all items (radio, checkbox) are off. */
		return;
	}
	var = (U8 *)alloc_mem (40);
	do_format = __FALSE;
	do {
		/* Parse all returned parameters. */
		dat = http_get_var (dat, var, 40);
		if (var[0] != 0) {
			/* Parameter found, returned string is non 0-length. */
			if (str_scomp (var, "label=") == __TRUE) {
				str_copy ((U8 *)label, var+6);
				/* Convert 'label' to upper case. */
				for (p = (U8 *)&label; *p; p++) {
					*p = toupper (*p);
				}
			}
			else if (str_scomp (var, "format=yes") == __TRUE) {
				do_format = __TRUE;
			}
		}
	}while (dat);
	free_mem ((OS_FRAME *)var);

	/* Format SD Card if requested. */
	if (do_format == __TRUE) {
		if (finit() != 1) {
			/* Return value 1 - SD Card is missing. */
			fformat ((const char *)label);
		}
	}
}


/*--------------------------- cgi_func --------------------------------------*/

U16 cgi_func (U8 *env, U8 *buf, U16 buflen, U32 *pcgi) 
{
	/* This function is called by HTTP server script interpreter to make a    */
	/* formated output for 'stdout'. It returns the number of bytes written   */
	/* to the output buffer. Hi-bit of return value (len is or-ed with 0x8000)*/
	/* is a repeat flag for the system script interpreter. If this bit is set */
	/* to 1, the system will call the 'cgi_func()' again for the same script  */
	/* line with parameter 'pcgi' pointing to a 4-byte buffer. This buffer    */
	/* can be used for storing different status variables for this function.  */
	/* It is set to 0 by HTTP Server on first call and is not altered by      */
	/* HTTP server for repeated calls. This function should NEVER write more  */
	/* than 'buflen' bytes to the buffer.                                     */
	/* Parameters:                                                            */
	/*   env    - environment variable string                                 */
	/*   buf    - HTTP transmit buffer                                        */
	/*   buflen - length of this buffer (500-1400 bytes - depends on MSS)     */
	/*   pcgi   - pointer to session local buffer used for repeated loops     */
	/*            This is a U32 variable - size is 4 bytes. Value is:         */
	/*            - on 1st call = 0                                           */
	/*            - 2nd call    = as set by this function on first call       */
	const char mask[] = "*.*";
	static FINFO info;
	char temp[16];
	U32 len = 0;
	int get_input_switch( int no );


	switch (env[0]) {
		/* Analyze the environment string. It is the script 'c' line starting */
		/* at position 2. What you write to the script file is returned here. */
		case 'f' :
			/* Format SD Card - file 'format.cgi' */
			switch (env[2]) {
				case '1':
					/* Format Label */
					len += sprintf ((char *)buf,(const char *)&env[4],label);
					break;
				case '2':
					{
						extern unsigned g_card_no;
						char str[30];
						sprintf( str, "Card No : %d(%x)", g_card_no, g_card_no );
						len += sprintf ((char *)buf,(const char *)&env[4],str);
					}
					break;
				case '3':
					{
						char str[30];
						int val1, val2;
						val1 = get_input_switch( 0 );
						val2 = get_input_switch( 1 );
						sprintf( str, "Switch State : %s, %s", val1?"OFF":"ON", val2?"OFF":"ON" );
						len += sprintf ((char *)buf,(const char *)&env[4],str);
					}
					break;
				case 'T':
					{
						extern char* t_cmd_result_buffer;
						len += sprintf ((char *)buf,(const char *)&env[4],t_cmd_result_buffer);
					}
					break;
				case 'V':
					{
						system_string_value( (const char *)&env[4], (char *)buf, buflen );
						len += strlen( buf );
					}
			}
			break;

		case 'D':
#if _d_sd
			/* Directory of SD Card - file 'dir.cgi' */
			if (MYBUF(pcgi)->xcnt == 0) {
				/* First Call, set initial value for 'ffind' function. */
				info.fileID = 0;
				if (finit() != 0) {
					/* No card or failed to initialize. */
					break;
				}
			}
			/* Repeat for all files, ignore Folders. */
			MYBUF(pcgi)->xcnt++;
			if (ffind (mask,&info) == 0) {
				dot_format (info.size, temp);
				len += sprintf ((char *)buf,"%s\n", info.name);
				/* Hi bit is a repeat flag. */
				len |= 0x8000;
			}
#elif _d_uffs
			// uffs_readdir
			//struct uffs_dirent * uffs_readdir(uffs_DIR *dirp)
			{
				int i;
				struct uffs_dirent *ud;
				static uffs_DIR *d;
				if( MYBUF(pcgi)->xcnt == 0 ){
					MYBUF(pcgi)->d = uffs_opendir( "/" );
				}
//MYBUF(pcgi)->d
				if( MYBUF(pcgi)->d ){
					ud = uffs_readdir( MYBUF(pcgi)->d );
					if( ud ){
						len += sprintf( (char*)buf, "%s\n", ud->d_name );
						len |= 0x8000;
					}
					else{
						uffs_closedir( MYBUF(pcgi)->d );
					}
				}
				MYBUF(pcgi)->xcnt++;
			}
#endif
			break;
		case 'd':
#if _d_sd
			/* Directory of SD Card - file 'dir.cgi' */
			if (MYBUF(pcgi)->xcnt == 0) {
				/* First Call, set initial value for 'ffind' function. */
				info.fileID = 0;
				if (finit() != 0) {
					/* No card or failed to initialize. */
					break;
				}
			}
			/* Repeat for all files, ignore Folders. */
			MYBUF(pcgi)->xcnt++;
			if (ffind (mask,&info) == 0) {
				dot_format (info.size, temp);
				len += sprintf ((char *)buf,"<tr align=center><td>%d.</td>"
						"<td align=left><a href=\"/%s\">%s</a></td>"
						"<td align=right>%s</td>"
						"<td>%02d.%02d.%04d - %02d:%02d</td></tr>\r\n",
						MYBUF(pcgi)->xcnt,info.name,info.name,temp,
						info.time.day, info.time.mon, info.time.year,
						info.time.hr, info.time.min);
				/* Hi bit is a repeat flag. */
				len |= 0x8000;
			}
#elif _d_uffs
			// uffs_readdir
			//struct uffs_dirent * uffs_readdir(uffs_DIR *dirp)
			{
				static uffs_DIR *d;
				int i;
				struct uffs_dirent *ud;
				d = uffs_opendir( "/" );
				for( i=0; i<= MYBUF(pcgi)->xcnt; i++ ){
					ud = uffs_readdir( d );
					if( ud == NULL )
						break;
				}
				if( ud ){
					len += sprintf( (char*)buf, "%s\n", ud->d_name );
					len |= 0x8000;
				}
				uffs_closedir( d );
				MYBUF(pcgi)->xcnt++;
			}
#endif
			break;
	}
	return ((U16)len);
}

/*----------------------------------------------------------------------------
 *        Print size in dotted fomat
 *---------------------------------------------------------------------------*/
static void dot_format (U32 val, char *sp) 
{

	if (val >= (U32)1e9) {
		sp += sprintf (sp,"%d.",val/(U32)1e9);
		val %= (U32)1e9;
		sp += sprintf (sp,"%03d.",val/(U32)1e6);
		val %= (U32)1e6;
		sprintf (sp,"%03d.%03d",val/1000,val%1000);
		return;
	}
	if (val >= (U32)1e6) {
		sp += sprintf (sp,"%d.",val/(U32)1e6);
		val %= (U32)1e6;
		sprintf (sp,"%03d.%03d",val/1000,val%1000);
		return;
	}
	if (val >= 1000) {
		sprintf (sp,"%d.%03d",val/1000,val%1000);
		return;
	}
	sprintf (sp,"%d",val);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
