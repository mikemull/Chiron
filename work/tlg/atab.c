/*===================================================================
 *    $Header: C:\MIKE\TLG\CHIRON\RCS\atab.c 2.0 1993/09/18 16:14:09 mwm Exp mwm $
 *
 *    Functions to get information from the AUTHTAB.DIR file.
 *    $Log: atab.c $
 * Revision 2.0  1993/09/18  16:14:09  mwm
 * Start of new version.
 *
 * Revision 1.2  92/06/29  19:41:21  ROOT_DOS
 * Added code to retrieve author name list from initial pattern
 * 
 * Revision 1.1  92/03/04  16:31:57  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys\timeb.h>
#include "tlg.h"
#include "file.h"
#include "tlgmem.h"
#include "greek.h"   /* ISESCAPE MACRO */
#include "atab.h"

#define MAX_AUTHNAME_LEN    100
#define MAX_FILENAME_LEN    8
#define ATAB_RECORD_END     0xFF
#define ATABBUFSIZE     26624

int *auth_nums;
int  ATAB_AUTH_COUNT;
struct ATABDIR
{
    char *author_name;
    char *file_name;
} atabdir[MAX_AUTH];

#define ATAB_AUTH(a)    atabdir[a].author_name
#define ATAB_FILE(a)    atabdir[a].file_name



/*-------------------------------------------------------------------
 *   FileNametoNumber        03.03.90  GH
 *
 *   Function to take an ASCII file name and return an integer
 *
 *   last modified : 03.26.90
 *-------------------------------------------------------------------
 */
int
FileNametoNumber( char *filename )
{
	return( atoi( strpbrk(filename,"0123456789") ) );
}/*
END OF FileNametoNumber
*/



/*-------------------------------------------------------------------
 *   GetAuthName             01.03.90  GH
 *
 *   Function to read the AuthorName beginning at the current
 *   buffer position.  Modified to strip out escape characters
 *   from 'D' AUTHTAB.DIR disk
 *
 *   last modified : 06.14.93  GH
 *-------------------------------------------------------------------
 */
int
GetAuthName( int buf_index, char *auth_name, BYTE *ATABbuf )
{
    int authname_index=0, tmp_index=buf_index;

    while( (ATABbuf[tmp_index]<=0x7f) )
    {
      if( authname_index >= MAX_AUTHNAME_LEN )
         SYSTEMCRASH( EXIT_AUTHNAMELEN );
      if( ISESCAPE( ATABbuf[tmp_index] ) )
      {
         tmp_index++;
         while( isdigit( ATABbuf[tmp_index] ) )
            tmp_index++;
      }
      else
         auth_name[authname_index++] = ATABbuf[tmp_index++];
    }
    auth_name[authname_index] = '\0';
    return( tmp_index );
}/*
END OF GetAuthName
*/



/*-------------------------------------------------------------------
 *   GetFileName             01.03.90  GH
 *
 *   Function to read the 8-character file name from the current
 *   buffer position.
 *
 *   last modified : 06.14.93  GH
 *-------------------------------------------------------------------
 */
int
GetFileName( char *filename, int buf_index, BYTE *ATABbuf )
{

    int tmp_index=buf_index,fname_index=0;

    for( fname_index = 0; fname_index <= MAX_FILENAME_LEN; fname_index++ )
    {
        if((ATABbuf[tmp_index] == SPACE) || (ATABbuf[tmp_index] == 0x00))
           break;
        else
           filename[fname_index] = ATABbuf[tmp_index++];
    }
     filename[fname_index] = '\0';
     return( buf_index + MAX_FILENAME_LEN );

}/*
END OF GetFileName
*/



/*-------------------------------------------------------------------
 *   CheckAuth               01.03.90  GH
 *
 *   Function to check if the requested author name is a substring of
 *   the author name returned by the function GetAuthName.
 *
 *   last modified : 03.03.89  GH
 *-------------------------------------------------------------------
 */
BOOL
CheckAuth( char *to_find_auth, char *atab_auth)
{
    return( strstr(strlwr(atab_auth),strlwr(to_find_auth)) != NULL );
}/*
END OF CheckAuth
*/



/*-------------------------------------------------------------------
 *   MakeAtabIndex             09.06.90  GH
 *
 *   Reads AUTHTAB.DIR from buffer ATABbuf and fills in the
 *   atabdir struct with stripped authornames and filenames.
 *   Also fills in the anums_index array.  This routine
 *   replaces both of the olld MakeAtabIndex and MakeAnumIndex
 *   returns the total author count.
 *
 *   last modified : 06.14.93 GH
 *-------------------------------------------------------------------
 */
int
MakeAtabIndex( void )
{
    int buf_index=0,auth_count=0,auth_num=0;
    char tmp_auth_name[MAX_AUTHNAME_LEN];
    BYTE *ATABbuf;
    FILE *TLGatab;

    ATABbuf = (BYTE *) TLGmalloc( ATABBUFSIZE*sizeof(BYTE) );
    if(!(TLGatab = OpenTLG( "AUTHTAB.DIR" )))
       SYSTEMCRASH(EXIT_BADOPEN);
    CHECKREAD(ATABbuf, sizeof(BYTE), ATABBUFSIZE, TLGatab );
    fclose( TLGatab );
    /* Create the auth_nums array */
    auth_nums = ( int * )TLGmalloc( MAX_AUTH_NUM * sizeof(int));
    /* First pass we get the first filename outside of loop */
    ATAB_FILE(auth_count) = (char *)TLGmalloc(MAX_FILENAME_LEN+1);
    buf_index = GetFileName(ATAB_FILE(auth_count),buf_index, ATABbuf);
    while( stricmp( ATAB_FILE(auth_count),"*END" ) !=0 )
    {
       if( auth_count >= MAX_AUTH )
          SYSTEMCRASH(EXIT_ATABMAXAUTH );
       /* fill in the auth_nums entry for this author/pass */
       if(( ATAB_FILE(auth_count)[0] != '*' ) &&
          ( strnicmp( ATAB_FILE(auth_count),"DOCCAN",6)!=0))
       {
          auth_num = FileNametoNumber(ATAB_FILE(auth_count));
          if(( auth_num > MAX_AUTH_NUM ) || ( auth_num <= 0 ))
             SYSTEMCRASH( EXIT_ATABAUTHNUM );
          auth_nums[auth_num]=auth_count;
       }
       /* Get author_name for this author/pass */
       buf_index = GetAuthName( buf_index, tmp_auth_name, ATABbuf );
       ATAB_AUTH(auth_count) = ( char * )TLGmalloc( strlen( tmp_auth_name)+1);
       strncpy( ATAB_AUTH(auth_count),tmp_auth_name, strlen(tmp_auth_name)+1);
       /* Skip to beginning of next record ( author ) */
       while( ATABbuf[buf_index] != ATAB_RECORD_END ) buf_index++;
       while( ATABbuf[buf_index] == ATAB_RECORD_END ) buf_index++;
       /* update auth_count and get filename for next pass */
       auth_count++;
       ATAB_FILE(auth_count) = (char *)TLGmalloc(MAX_FILENAME_LEN+1);
       buf_index = GetFileName(ATAB_FILE(auth_count),buf_index, ATABbuf);
       if( buf_index == 0x00 ) break;
    }
    /* put the author_count into a static so we can get at it later */
    ATAB_AUTH_COUNT = --auth_count;
    /* and return it just to make everyone happy */
    TLGfree( ATABbuf );
    return(auth_count);
}/*
END OF MakeAtabIndex */



/*-------------------------------------------------------------------
 *   GetAuths                03.23.90 GH
 *
 *   searches through the atab_dir structure array and returns
 *   a listing of all author names and file names for a given
 *   approximate author name.
 *   fills in two arrays of char pointers. 1 contains full
 *   author names, the other contains associated file names
 *   Returns the total number of approximate author names found.
 *   This function is a replacement for SearchForAuth
 *
 *   last modified : 06.14.90  GH
 *-------------------------------------------------------------------
 */
int
GetAuths( char *authname, char *names[], int *anums, int max_count )
{

    int auth_count=0,atab_index=0,name_len;
    /*
    ** Remove leading blanks and bail if rest is null
    */
    while(*authname == ' ')
        authname++;

    Tcl_ValidateAllMemory(__FILE__,__LINE__);
    if( strlen(authname) )
    {
       for( atab_index=0; atab_index<=ATAB_AUTH_COUNT; atab_index++ )
       {
          if( auth_count >= max_count )
             break;
          if(strnicmp(authname,ATAB_AUTH(atab_index),strlen(authname))==0)
          {
             name_len = strlen(ATAB_AUTH(atab_index))+1;
             names[auth_count] = ( char * ) TLGmalloc( name_len );
             anums[auth_count] = FileNametoNumber( ATAB_FILE(atab_index) );
             strncpy(names[auth_count],ATAB_AUTH(atab_index),name_len );
             auth_count++;
          }
       }
    }
    names[auth_count] = NULL;
    Tcl_ValidateAllMemory(__FILE__,__LINE__);
    return( auth_count );
}/*
END OF GetAuths
*/


/*-------------------------------------------------------------------
 *    ATABNumbertoName       03.03.90  GH
 *
 *    function to search the atab_dir structure array and convert a
 *    specified author number to an author name
 *
 *    last modified : 06.14.93
 *-------------------------------------------------------------------
 */
BOOL
ATABNumbertoName( int auth_num, char *auth_name )
{
    strncpy( auth_name, ATAB_AUTH( auth_nums[auth_num] ),80 );
    return( TRUE );
}/*
END OF ATABNumbertoName
*/

