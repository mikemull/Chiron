/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\canon.c 2.0 1993/09/15 20:49:32 mwm Exp mwm $
 *
 *   Functions to manipulate the TLG document canon files.
 *
 *   last modified   02.13.91  GH
 *   re-worked       01.21.92  GH
 *
 *   $Log: canon.c $
 * Revision 2.0  1993/09/15  20:49:32  mwm
 * Start of new version.
 *
 *
 *-------------------------------------------------------------------
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "tlg.h"
#include "tlgmem.h"
#include "text.h"
#include "citlow.h"
#include "citation.h"
#include "canon.h"

/* --- Macros  ---*/
#define CANBIB_BLK_MAX_LINES    500
#define CANBIB_AUTHORNUM        "9999"
#define CANBIB_WORKNUM          "001"
#define SPACE                   0x20

/* --- Statics ---*/

static char *canbib_biblio[CAN_BIBLIO_MAX_LINES];
static citation_t *canbib_srch_cit;
static citation_t *canbib_curr_cit;
static BYTE *canbib_buf;
static int canbib_blk_line_count;
static int canbib_curr_line;

/*---- Externals  -----*/

extern BYTE *CANIDTbuf;
extern FILE *TLGcantxt;



/*-------------------------------------------------------------------
 *   CanBibInit              01.22.90  GH
 *
 *   last modfied  07/31/93
 *-------------------------------------------------------------------
 */
void
CanBibInit( )
{
    int i;

    for( i=0; i<CAN_BIBLIO_MAX_LINES; i++)
    {
        canbib_biblio[i] = NULL;
    }
}/*
END of CanBibInit

/*-------------------------------------------------------------------
 *   CanBibFree              01.22.90  GH
 *
 *   last modfied  07/31/93
 *-------------------------------------------------------------------
 */
void
CanBibFree( )
{
    int i;

    for( i=0; i<CAN_BIBLIO_MAX_LINES; i++)
    {
        if( canbib_biblio[i] )
        {
           TLGfree( canbib_biblio[i] );
           canbib_biblio[i] = NULL;
        }

    }
}/*
END of CanBibFree
*/


/*-------------------------------------------------------------------
 *  CanBibFindCit
 *
 *-------------------------------------------------------------------
 */
int
CanBibFindCit( char *cit_index[] )
{
    int i=0;
    InitCit( canbib_curr_cit );
    for( i=0; i<canbib_blk_line_count; i++ )
    {
       CitFromTextIndex( cit_index, i, canbib_curr_cit );
       if( CompareCit( canbib_curr_cit, canbib_srch_cit ) >= 0 )
          break;
    }
    if( i == canbib_blk_line_count )
       SYSTEMCRASH( EXIT_BADCANCITINDEX );
    return( i );
}

/*-------------------------------------------------------------------
 *   CanBibGetBlock              01.22.90  GH
 *
 *   last modfied  07/31/93
 *-------------------------------------------------------------------
 */
int
CanBibGetBlock( long block_num, BYTE *text_index[], BYTE *cit_index[] )
{
    int line_count;
    long seek_pos;
    BYTE *buf_ptr = canbib_buf;

    seek_pos = ( block_num * (long) TLGBLOCKSIZE );
    fseek( TLGcantxt, seek_pos, SEEK_SET );
    CHECKREAD( buf_ptr,sizeof(BYTE),TLGBLOCKSIZE, TLGcantxt );
    line_count = IndexBlock( buf_ptr, text_index, cit_index );
    if( line_count >= CANBIB_BLK_MAX_LINES )
       SYSTEMCRASH( EXIT_BLOCKTOOBIG );
    return( line_count );
}

/*-------------------------------------------------------------------
 *   CanBibReadLine              01.22.90  GH
 *
 *   last modfied  07/31/93
 *-------------------------------------------------------------------
 */
void
CanBibReadLine( BYTE *text_line, char **can_bib_line )
{
    int j=0;
    BYTE *text_ptr=text_line;
    char tmp_line[100];

    j=0;
    while( !HIBITSET( *text_ptr ) )
    {
       tmp_line[j] = *text_ptr;
       j++;
       text_ptr++;
       if( j >= 100 )
          SYSTEMCRASH( EXIT_CANLINETOOLONG );
    }
    if( tmp_line[j-1] == SPACE )
       tmp_line[j-1] = 0x00;
    else
    {
       tmp_line[j] = 0x00;
       j++;
    }
    *can_bib_line = ( char * )TLGmalloc( j * sizeof( char ) );
    if( can_bib_line == NULL )
       SYSTEMCRASH(EXIT_READLINEMALLOCFAIL);
    strncpy( *can_bib_line, tmp_line, j );
}

/*-------------------------------------------------------------------
 *   CanBibGetAuthInfo              01.22.90  GH
 *
 *   last modfied  07/31/93
 *-------------------------------------------------------------------
 */
void
CanBibGetAuthInfo( int auth_num )
{
    BYTE *result_ptr;
    int block_number,i;
    BYTE *text_index[CANBIB_BLK_MAX_LINES],*cit_index[CANBIB_BLK_MAX_LINES];

    /******************************************************
     **  Initialize the Canon search-for Citation
     *****************************************************/
    InitCit( canbib_srch_cit );
    strcpy( (*canbib_srch_cit)[CIT_AUTHOR].level_val,CANBIB_AUTHORNUM );
    strcpy( (*canbib_srch_cit)[CIT_WORK].level_val,CANBIB_WORKNUM );
    sprintf( (*canbib_srch_cit)[CIT_X].level_val,"%i",auth_num );
    strcpy(  (*canbib_srch_cit)[CIT_Y].level_val,"a" );
    strcpy(  (*canbib_srch_cit)[CIT_Z].level_val,"1");
    /******************************************************
     **  Read in the doccan1.txt block containing the
     **  desired bibliography
     *****************************************************/
    result_ptr = WhereisCit( CANIDTbuf, canbib_srch_cit, &block_number);
    if( result_ptr == ( BYTE * ) NULL )
       SYSTEMCRASH( EXIT_BADCANAUTHCIT );
    canbib_blk_line_count = CanBibGetBlock( (long) block_number,
                                            text_index, cit_index  );
    /******************************************************
     **  Find the first line of the desired bibliography
     *****************************************************/
    i = CanBibFindCit( cit_index );
    while( (*canbib_curr_cit)[CIT_Y].level_val[0] == 'a' )
    {
       CanBibReadLine( text_index[i], &canbib_biblio[canbib_curr_line] );
       canbib_curr_line++;
       i++;
       /******************************************************
        **  Read in the next block if we've reached the end
        **  of the current block
        *****************************************************/
       if( i==canbib_blk_line_count )
       {
          canbib_blk_line_count = CanBibGetBlock( (long) ++block_number,
                                                  text_index, cit_index  );
          i=0;
       }
       CitFromTextIndex( cit_index, i, canbib_curr_cit );
    }
}



/*-------------------------------------------------------------------
 *   CanBibGetWorkInfo              01.22.90  GH
 *
 *   last modfied  07/31/93
 *-------------------------------------------------------------------
 */
void
CanBibGetWorkInfo( int auth_num, int work_num )
{
    BYTE *result_ptr;
    int block_number,i;
    BYTE *text_index[CANBIB_BLK_MAX_LINES], *cit_index[CANBIB_BLK_MAX_LINES];

    /******************************************************
     **  Initialize the Canon search-for Citation
     *****************************************************/
    InitCit( canbib_srch_cit );
    strcpy( (*canbib_srch_cit)[CIT_AUTHOR].level_val,CANBIB_AUTHORNUM );
    strcpy( (*canbib_srch_cit)[CIT_WORK].level_val,CANBIB_WORKNUM );
    sprintf( (*canbib_srch_cit)[CIT_X].level_val,"%i",auth_num );
    sprintf( (*canbib_srch_cit)[CIT_Y].level_val,"%i",work_num );
    strcpy( (*canbib_srch_cit)[CIT_Z].level_val,"1");

    /******************************************************
     **  Read in the doccan1.txt block containing the
     **  desired bibliography
     *****************************************************/
    result_ptr = WhereisCit( CANIDTbuf, canbib_srch_cit, &block_number );
    if( result_ptr == ( BYTE * ) NULL )
       SYSTEMCRASH( EXIT_BADCANAUTHCIT );
    canbib_blk_line_count = CanBibGetBlock( (long) block_number,
                                            text_index, cit_index  );

    /******************************************************
     **  Find the first line of the desired bibliography
     *****************************************************/
    i = CanBibFindCit( cit_index );
    while( atoi((*canbib_curr_cit)[CIT_X].level_val) == auth_num &&
           atoi((*canbib_curr_cit)[CIT_Y].level_val) == work_num )
    {
       CanBibReadLine( text_index[i], &canbib_biblio[canbib_curr_line]);
       canbib_curr_line++;
       i++;
       /******************************************************
        **  Read in the next block if we've reached the end
        **  of the current block
        *****************************************************/
       if( i==canbib_blk_line_count )
       {
          canbib_blk_line_count = CanBibGetBlock( (long) ++block_number,
                                                  text_index, cit_index  );

          i=0;
       }
       CitFromTextIndex( cit_index, i, canbib_curr_cit );
    }
}



/*-------------------------------------------------------------------
 *   CanBibGetBiblio               07.31.93  GH
 *
 *   Function to extract the canon bibliography information
 *   from the doccan1.txt file.
 *
 *-------------------------------------------------------------------
 */
char **
CanBibGetBiblio( int auth_num, int work_num )
{

    CanBibInit();

    canbib_buf = ( BYTE * ) TLGmalloc(TLGBLOCKSIZE * sizeof(BYTE));
    canbib_srch_cit = ( citation_t * ) TLGmalloc(sizeof(citation_t));
    canbib_curr_cit = ( citation_t * ) TLGmalloc(sizeof(citation_t));
    canbib_curr_line=0;

    if( auth_num != 0 )
       CanBibGetAuthInfo( auth_num );

    if( work_num != 0 )
       CanBibGetWorkInfo( auth_num, work_num );

    TLGfree( canbib_buf );
    TLGfree( canbib_srch_cit );
    TLGfree( canbib_curr_cit );
    return( canbib_biblio );
}/*
END of CanBibGetBiblio
*/



/*-----------------------------------------------
 *    CanBibToDisk
 *
 *    Function to write the current canon biblio
 *    to a disk file ...
 *----------------------------------------------*/
int
CanBibToDisk( char *filename )
{
    FILE *can_file;
    int result=0,i;
        
    if( can_file = fopen( filename,"at+") )
    {
        result = 1;
        fputs("\n\n******************************\n\n",can_file);
        fputs("  TLG Canon:  Bibliography Format\n",can_file);
        fputs("\n******************************\n",can_file);

        for( i=0; i<CAN_BIBLIO_MAX_LINES; i++)
        {
           if( canbib_biblio[i] )
              fprintf(can_file,"\n%s",canbib_biblio[i]);
        }
        fclose(can_file);
    }
    return( result );
}
