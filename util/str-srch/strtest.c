/*===================================================================
 *   $Header:$
 *
 *   Main program to test and time the string and pattern
 *   searching routines ( BM_Srch, BYG_Srch, regex, strstr, etc ).
 *
 *   $Log:$
 *-------------------------------------------------------------------
 */
#define ITERATIONS  1
#define BYGSRCH
#define BMSRCH
/*
#define REGEX
#define STRSTR
#define IGNORESET ""
*/
#define IGNORESET "()|+/\\="
#define BYGERRS   0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include "tlg.h"
#include "bmsrch.h"
#include "bygsrch.h"
#include "regex.h"

/*
 * These are set by the individual search routines
 */
BYTE *beg_of_patt, *end_of_patt;

void
main( int argc, char *argv[] )
{
    BYTE c,textbuf[8192],*p;
    FILE *fileptr;
    int filehandle;
    long filesize;
    char *pattern, *filename;
    size_t readsize;
    unsigned long itcount=ITERATIONS;
    unsigned long lindex=0;
    time_t beg_t, end_t;
    int pattern_index,pattern_len;
    char ignore_set[10] = IGNORESET;

    if( argc < 3 )
    {
       fprintf(stderr,"USAGE : %s pattern textfile\n",argv[0] );
       exit(1);
    }
    pattern = (char *)calloc(strlen(argv[1]+1),sizeof(char));
    filename = (char *)calloc(strlen(argv[2]+1),sizeof(char));
    strcpy(pattern,argv[1]);
    strcpy(filename,argv[2]);

    fileptr = fopen(filename,"r");
    if( fileptr == ( FILE * ) NULL  )
    {
       fprintf(stderr,"Error opening %s\n",filename);
       exit(1);
    }

    filehandle = fileno( fileptr );
    filesize = filelength( filehandle ) -1;
    filesize = ( filesize > 8192 ) ? 8192 : filesize;

    printf("Reading %li bytes from %s\n",filesize,filename);
    if((readsize=fread(textbuf,sizeof(char),filesize,fileptr))!=filesize)
    {
       fprintf(stderr,"Error reading %li bytes from %s ( got %i )\n",
               filesize,filename, readsize );
    }
    else
    {
       fprintf(stderr,"Read %i bytes from %s\n",readsize,filename);
    }

    textbuf[readsize] = '\0';
    fclose(fileptr);

    printf("Total iteration count = %i\n",itcount);
    printf("Pattern = %s\n",pattern);
    printf("Ignore_set = %s\n",ignore_set);
    printf("Searching file %s\n",filename);

    /*
     *   Process the pattern to remove ignore_set characters
     */
    pattern_index = 0;
    pattern_len = strlen(pattern);
    while( pattern_index <= pattern_len-1  )
    {
       if( strchr((char *)ignore_set,pattern[pattern_index]))
       {
          memmove(&pattern[pattern_index],
                  &pattern[pattern_index+1],
                  (pattern_len-pattern_index));
          pattern_len--;
          pattern_index--;
      }
      pattern_index++;
   }

#ifdef STRSTR
    /*
     * Simple strstr() times ( for baseline comparison )
     */
    time ( &beg_t);
	for ( lindex = 0; lindex < itcount; lindex++ )
    {
        p = ( char * ) NULL;
        p = strstr(textbuf,pattern);
    }
    time ( &end_t);
    printf("\n");
    printf("** strstr() results **\n");
    printf("     Total elapsed time = %ld seconds\n",end_t - beg_t);
    printf("     elapsed time per search = %f seconds\n",
                 (float)(end_t - beg_t)/(float)itcount);
    printf("     found pattern = %s\n\n",p);
#endif

#ifdef BMSRCH
    /*
     * Boyer-Moore search ( BM_Srch() );
     */
    time( &beg_t );
	for ( lindex = 0; lindex < itcount; lindex++ )
	{
        p = ( char * ) NULL;
        p = BM_Srch(pattern,textbuf,readsize,ignore_set);
	}
    time ( &end_t );
    printf("\n");
    printf("** BM_Srch() results **\n");
    printf("     Total elapsed time = %ld seconds\n",end_t - beg_t);
    printf("     elapsed time per search = %f seconds\n",
                 (float)(end_t - beg_t)/(float)itcount);
    printf("     found pattern = %s\n\n",p);
#endif

#ifdef BYGSRCH
    /*
     * Baeza-Yates-Gonnet Search  ( BYG_Srch() );
     */
    if( BYG_SetErrors( BYGERRS ) != BYGERRS )
       printf("Error setting BYG number of errors\n");
    time( &beg_t );
	for ( lindex = 0; lindex < itcount; lindex++ )
	{
        p = ( char * ) NULL;
        p = BYG_Srch(pattern,textbuf,readsize,ignore_set);
	}
    time ( &end_t );
    printf("\n");
    printf("** BYG_Srch() results **\n");
    printf("     Total elapsed time = %ld seconds\n",end_t - beg_t);
    printf("     elapsed time per search = %f seconds\n",
                 (float)(end_t - beg_t)/(float)itcount);
    printf("     found pattern = %s\n\n",p);
#endif

#ifdef REGEX
    /*
     * REGEX search ( BM_Srch() );
     */
    time( &beg_t );
	for ( lindex = 0; lindex < itcount; lindex++ )
	{
        p = ( char * ) NULL;
        re_comp( ( char * ) pattern, ignore_set );
        p = ( BYTE * ) re_exec(textbuf);
	}
    time ( &end_t );
    printf("\n");
    printf("** regex() results **\n");
    printf("     Total elapsed time = %ld seconds\n",end_t - beg_t);
    printf("     elapsed time per search = %f seconds\n",
                 (float)(end_t - beg_t)/(float)itcount);
    printf("     found pattern = %s\n\n",p);
#endif
/*
    getch();
*/
}
