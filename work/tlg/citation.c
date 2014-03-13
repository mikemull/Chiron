/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\citation.c 2.1 1993/11/21 16:29:52 mwm Exp mwm $
 *
 *   File contains code to perform TLG citation manipulations.
 *
 *   last modified : 09.19.90  GH
 *
 *   $Log: citation.c $
 * Revision 2.1  1993/11/21  16:29:52  mwm
 * Fix for bug #8 (mwm for gah).
 *
 * Revision 2.0  1993/09/15  20:44:43  mwm
 * Start of new version.
 *
 *
 *===================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <alloc.h>
#include <mem.h>
#include <sys\stat.h>
#include <graphics.h>
#include "tlg.h"
#include "tlgmem.h"
#include "canon.h"
#include "print.h"
#include "bit.h"
#include "citlow.h"
#include "citation.h"

/*************** ENUMS ***************/

enum cit_stat { CIT_GOOD, CIT_NO_LOW, CIT_NO_WORK, CIT_BAD
              };

/*************** MACROS **************/
#define MAXIDTSIZE     65536



/********************************************************************
 *  The following functions are only used internally in citation.c
 *
 *  FindAuthBlockByNumber, FindWorkBlockByNumber,
 *  FindWorkBlockByNumber, FindWorkBlockByName, CheckCit,
 *  WhereisAuthWorkNum
 *
 *  These may be moved to another source file at some future time.
 *******************************************************************/
/*-------------------------------------------------------------------
 *   FindAuthBlockByNumber        10.25.89  GH
 *
 *   Steps through an .IDT buffer looking for the specified author
 *   number : returns ( BYTE * )NULL if author_number not found,
 *   else returns the updated buf_ptr and stores the block in
 *   *block_num.
 *-------------------------------------------------------------------
 */
BYTE *
FindAuthBlockByNumber( BYTE *buffer, unsigned auth_num, unsigned *block_num )
{
    unsigned section_len=0;
    BYTE *buf_ptr=buffer;
    citation_t *citation;

    if( *buf_ptr != TYPE_NEWAUTH )
    {
      if( (buf_ptr=FindNextTypeCodeinBuf(buf_ptr,TYPE_NEWAUTH) )
          == (BYTE *)NULL )
      {
        *block_num = AUTH_NOT_FOUND;
        return( ( BYTE * ) NULL );
      }
    }

    citation = ( citation_t * ) TLGmalloc(sizeof(citation_t));

    while( *buf_ptr )
	{
       if( *buf_ptr != TYPE_NEWAUTH )
       {
          *block_num = FIND_AUTH_BAD_CODE;
          return( ( BYTE * ) NULL );
       }
       DecodeNewAuthWork(buf_ptr,&section_len,block_num,citation);
       if( atoi(CIT_VALUE(CIT_AUTHOR)) == auth_num )
           break;
       buf_ptr += section_len;
	}
    TLGfree( citation );
    if( !(*buf_ptr) ) *block_num = AUTH_NOT_FOUND;
    return( buf_ptr );
}/*
END OF FindAuthBlockByNumber
*/



/*-------------------------------------------------------------------
 *   FindWorkBlockByNumber    11.04.89  GH
 *
 *   Steps through an .IDT buffer looking for the specified work
 *   number : returns WORK_NOT_FOUND if author_number not found,
 *   else returns the block number in the associated .txt file
 *   where the author number can be found.
 *-------------------------------------------------------------------
 */
BYTE *
FindWorkBlockByNumber( BYTE *buffer, unsigned work_num, unsigned *block_num )
{
    unsigned section_len=0;
    BYTE *buf_ptr=buffer;
    citation_t *citation;

    if( *buf_ptr != TYPE_NEWWORK )
    {
      if( (buf_ptr=FindNextTypeCodeinBuf(buf_ptr,TYPE_NEWWORK) )
          == (BYTE *)NULL )
      {
        *block_num = WORK_NOT_FOUND;
        return( ( BYTE * ) NULL );
      }
    }

    citation = ( citation_t * ) TLGmalloc(sizeof(citation_t));

    while( *buf_ptr )
	{
       if( *buf_ptr != TYPE_NEWWORK )
       {
          *block_num = FIND_WORK_BAD_CODE;
          return( ( BYTE * ) NULL );
       }
       DecodeNewAuthWork(buf_ptr,&section_len,block_num,citation);
       if( atoi(CIT_VALUE(CIT_WORK)) == work_num )
           break;
       buf_ptr += section_len;
	}
    TLGfree( citation );
    if( !(*buf_ptr) ) *block_num = WORK_NOT_FOUND;
    return( buf_ptr );
}/*
END OF FindWorkBlockByNumber
*/



/*-------------------------------------------------------------------
 *   WhereisAuthWorkNum      10.25.89  GH
 *
 *   Function to determine the block number where a specified
 *   AuthorNumber and Work Number exist in a TXT file. The IDT
 *   file for is loaded into memory in it's entirety.
 *   If either Auth or Work do not exist in the IDTable
 *   WhereisAuthWork returns ( BYTE * ) NULL, otherwise it
 *   returns the updated buffer position and puts the block
 *   number in *block_num.
 *
 *-------------------------------------------------------------------
 */
BYTE *
WhereisAuthWorkNum( BYTE *buffer, unsigned auth, unsigned work,
                    unsigned *block_num )
{
    BYTE *buf_ptr=buffer;

    buf_ptr = FindAuthBlockByNumber( buf_ptr, auth, block_num);
    if( work && *buf_ptr )
       buf_ptr = FindWorkBlockByNumber( buf_ptr, work, block_num );
    return( buf_ptr );
}/*
END OF WhereisAuthWorkNum
*/



/*-------------------------------------------------------------------
 *    FindAuthBlockByName    12.10.89  GH
 *
 *    Operates identical to FindAuthNumber except the Author is
 *    matched by name
 *
 *    last modified  02.09.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
FindAuthBlockByName( BYTE *buffer, char *auth_name, unsigned *block_num )
{

    unsigned section_len=0;
    BYTE *buf_ptr=buffer, *tmp_ptr;
    citation_t *citation;

    if( *buf_ptr != TYPE_NEWAUTH )
    {
      if( (buf_ptr=FindNextTypeCodeinBuf(buf_ptr,TYPE_NEWAUTH) )
          == (BYTE *)NULL )
      {
        *block_num = AUTH_NOT_FOUND;
        return( ( BYTE * ) NULL );
      }
    }

    citation = ( citation_t * ) TLGmalloc(sizeof(citation_t));

    while( *buf_ptr )
	{
       if( *buf_ptr != TYPE_NEWAUTH )
       {
          *block_num = FIND_AUTH_BAD_CODE;
          return( ( BYTE * ) NULL );
       }
       tmp_ptr = DecodeNewAuthWork(buf_ptr,&section_len,block_num,citation);
       if( *tmp_ptr != TYPE_AWDESC )
       {
         if( (tmp_ptr=FindNextTypeCodeinBuf(tmp_ptr,TYPE_AWDESC) )
              == ( BYTE * ) NULL )
         {
            *block_num = AUTH_NOT_FOUND;
            return( ( BYTE * ) NULL );
         }
       }
       DecodeAuthWorkDesc( buf_ptr, citation );
       if( !(stricmp( auth_name,CIT_DESC(CIT_AUTHOR) ) ) )
          break;
       buf_ptr += section_len;
	}
    TLGfree( citation );
    if( !(*buf_ptr) ) *block_num = AUTH_NOT_FOUND;
    return( buf_ptr );
}/*
END OF FindAuthBlockByNumber
*/



/*-------------------------------------------------------------------
 *   FindWorkBlockByName            12.10.89  GH
 *
 *   Works just like FindWorkNumber but comparisons done on
 *   work description strings instead of Work Numbers.
 *
 *   last modified  02.03.93 GH
 *-------------------------------------------------------------------
 */
BYTE *
FindWorkBlockByName( BYTE *buffer, char *work_name, unsigned *block_num )
{

    unsigned section_len=0;
    BYTE *buf_ptr=buffer, *tmp_ptr;
    citation_t *citation;

    if( *buf_ptr != TYPE_NEWWORK )
    {
      if( (buf_ptr=FindNextTypeCodeinBuf(buf_ptr,TYPE_NEWWORK) )
          == (BYTE *)NULL )
      {
        *block_num = WORK_NOT_FOUND;
        return( ( BYTE * ) NULL );
      }
    }

    citation = ( citation_t * ) TLGmalloc(sizeof(citation_t));

    while( *buf_ptr && ( *buf_ptr != TYPE_NEWAUTH ) )
	{
       if( *buf_ptr != TYPE_NEWWORK )
       {
          *block_num = FIND_WORK_BAD_CODE;
          return( ( BYTE * ) NULL );
       }
       tmp_ptr = DecodeNewAuthWork(buf_ptr,&section_len,block_num,citation);
       if( *tmp_ptr != TYPE_AWDESC )
       {
         if( (tmp_ptr=FindNextTypeCodeinBuf(tmp_ptr,TYPE_AWDESC) )
              == ( BYTE * ) NULL )
         {
            *block_num = WORK_NOT_FOUND;
            return( ( BYTE * ) NULL );
         }
       }
       DecodeAuthWorkDesc( buf_ptr, citation );
       if( !(stricmp( work_name,CIT_DESC(CIT_WORK) ) ) )
         break;
       buf_ptr += section_len;
	}
    TLGfree( citation );
    if( !(*buf_ptr) || ( *buf_ptr == TYPE_NEWAUTH ))
       *block_num = WORK_NOT_FOUND;
    return( buf_ptr );
}/*
END OF FindAuthBlockByNumber
*/



/*-------------------------------------------------------------------
 *   FindAuthBlock
 *
 *   Returns the block number containing the author specified
 *   in citation.  Author may be specified by name or number.
 *   Searches IDT table loaded into buffer
 *-------------------------------------------------------------------
 */
BYTE *
FindAuthBlock( BYTE *buffer, citation_t *citation, unsigned *block_loc )
{
    BYTE *buf_ptr = buffer;

    if( buf_ptr == ( BYTE * )NULL )
       SYSTEMCRASH( EXIT_CITBADBUFPTR );

    if( CIT_DESC(CIT_AUTHOR)[0] != NULL )
       buf_ptr = FindAuthBlockByName( buf_ptr, CIT_DESC(CIT_AUTHOR),
                                      block_loc);
    else if( CIT_VALUE(CIT_AUTHOR)[0] != NULL )
       buf_ptr = FindAuthBlockByNumber( buf_ptr, atoi(CIT_VALUE(CIT_AUTHOR)),
                                        block_loc);
    else
       SYSTEMCRASH( EXIT_CITAUTHERROR );
    return( buf_ptr );
}

/*-------------------------------------------------------------------
 *   FindWorkBlock
 *
 *   Returns the block number containing the work specified
 *   in citation.  Work may be specified by name or number.
 *   Searches IDT table loaded into buffer.
 *-------------------------------------------------------------------
 */
BYTE *
FindWorkBlock( BYTE *buffer, citation_t *citation, unsigned *block_loc )
{
    BYTE *buf_ptr = buffer;

    if( buf_ptr == ( BYTE * )NULL )
       SYSTEMCRASH( EXIT_CITBADBUFPTR );

    if( CIT_DESC(CIT_WORK)[0] != NULL )
       buf_ptr = FindWorkBlockByName( buf_ptr, CIT_DESC(CIT_WORK),
                                      block_loc);
    else if( CIT_VALUE(CIT_WORK)[0] != NULL )
       buf_ptr = FindWorkBlockByNumber( buf_ptr, atoi(CIT_VALUE(CIT_WORK)),
                                        block_loc);
    else
       SYSTEMCRASH( EXIT_CITWORKERROR );
    return( buf_ptr );
}


/*-------------------------------------------------------------------
 *   FindLowBlock              02.26.93  GH
 *
 *   Finds a user-specified citation beginning at the current
 *   location.  This is called after the Work and Author
 *   have already been found.
 *
 *   last modified
 *-------------------------------------------------------------------
 */
BYTE *
FindLowBlock( BYTE *buffer, citation_t *citation, unsigned *block_loc )
{
    BYTE *buf_ptr = buffer;
    /* int suspect_cit = FALSE; */
    citation_t *curr_cit, *tmp_cit, *block_end_cit, *except_beg_cit;
    unsigned  curr_block=0, excep_blk=0;
    BOOL      new_section=FALSE,done=FALSE;

    if( buf_ptr == ( BYTE * )NULL )
       SYSTEMCRASH( EXIT_CITBADBUFPTR );

    curr_cit = ( citation_t * ) TLGmalloc( sizeof(citation_t ));
    tmp_cit  = ( citation_t * ) TLGmalloc(sizeof(citation_t ));
    block_end_cit  = ( citation_t * ) TLGmalloc(sizeof(citation_t ));
    except_beg_cit  = ( citation_t * ) TLGmalloc(sizeof(citation_t ));

    strcpy( (*curr_cit)[CIT_AUTHOR].level_desc, CIT_DESC(CIT_AUTHOR) );
    strcpy( (*curr_cit)[CIT_AUTHOR].level_val , CIT_VALUE(CIT_AUTHOR) );
    strcpy( (*curr_cit)[CIT_WORK].level_desc,   CIT_DESC(CIT_WORK) );
    strcpy( (*curr_cit)[CIT_WORK].level_val ,   CIT_VALUE(CIT_WORK) );

    curr_block = *block_loc;
    buf_ptr = FindNextTypeCodeinBuf( buf_ptr, TYPE_NEWSECTION );
    done = FALSE;
    while( *buf_ptr && ( *buf_ptr != TYPE_NEWWORK ) &&
           ( *buf_ptr != TYPE_NEWAUTH ) && ( *buf_ptr != TYPE_EOF ) &&
           !done )
    {
       memcpy(tmp_cit,curr_cit,sizeof(citation_t));
       switch ( *buf_ptr )
       {
         case TYPE_NEWSECTION  : buf_ptr = DecodeNewSection(buf_ptr,
                                                            &curr_block );
                                 new_section = TRUE;
                                 break;
         case TYPE_BEGCIT   : buf_ptr = DecodeSectionStartCit( buf_ptr,tmp_cit );
                              if( CompareCit( citation,tmp_cit ) <= 0 )
                                done = TRUE;
                              break;
         case TYPE_ENDCIT  :   buf_ptr=DecodeSectionEndCit(buf_ptr,tmp_cit);
                               if( CompareCit(citation,tmp_cit) <= 0 )
                               {
                                 if( CheckCit(block_end_cit ) )
                                 {
                                   if( CompareCit(citation,block_end_cit) <= 0)
                                     *block_loc = curr_block;
                                   else
                                     *block_loc = curr_block + 1;
                                 }
                                 else
                                   *block_loc = curr_block;
                                 done = TRUE;
                               }
                               else
                                 /*
                                 suspect_cit = TRUE;
                                 */
                               break;
         case TYPE_LASTCIT : if( !new_section )
                               curr_block++;
                             else
                               new_section = FALSE;
                             buf_ptr=DecodeBlockEndCit(buf_ptr,tmp_cit);
                             if( CompareCit(citation,tmp_cit) <= 0 )
                             {
                               *block_loc = curr_block;
                               done = TRUE;
                             }
                             memcpy(block_end_cit,tmp_cit,sizeof(citation_t));
                             break;

         case TYPE_BEGEXCEPT : buf_ptr = DecodeExceptionStart(buf_ptr,
                                                              &excep_blk,
                                                              tmp_cit);
                               memcpy(except_beg_cit,tmp_cit,
                                      sizeof(citation_t));
                               break;

         case TYPE_ENDEXCEPT : buf_ptr = DecodeExceptionEnd( buf_ptr,tmp_cit);
                               if(((CompareCit(citation,except_beg_cit)>0) &&
                                   (CompareCit(citation,tmp_cit) <= 0 )))
                               {
                                 *block_loc = excep_blk;
                                 done = TRUE;
                               }
                               break;
         case UNKNOWN_13     : buf_ptr = DecodeUnknownType13 ( buf_ptr );
                               break;
         default             : SYSTEMCRASH(EXIT_BADTYPECODE);
                               break;
       }/*end of switch*/
       memcpy(curr_cit,tmp_cit,sizeof(citation_t));
    }
    if( curr_cit ) TLGfree(curr_cit);
    if( tmp_cit  ) TLGfree(tmp_cit );
    if( block_end_cit ) TLGfree(block_end_cit);
    if( except_beg_cit ) TLGfree(except_beg_cit);
    return( buf_ptr );
}/*
END OF FindLowBlock
*/



/*-------------------------------------------------------------------
 *   CheckCit                12.10.89  GH
 *
 *   Function to check a citation and makes sure it contains
 *   enough information to make a citation specific search possible.
 *   the citation must contain at least the following information.
 *   1.)   Author Name or Number
 *   2.)   Work Name or Number
 *   3.)   At least one non NULL low level ( levels v - z ) value.
 *
 *   Merged with CitLevels function on 02.06.92.  CitLevels now
 *   returns the following
 *       ==>   3: No Auth level / ? Work / ? low levels  (CITBAD)
 *       ==>   2: Author level set / no work level / ? low levels (CITNOWORK)
 *       ==>   1: Author / Work levels set / low levels not set (CITNOLOW)
 *       ==>   0: Author / Work and at least one low level set. (CITOK)
 *
 *   last modified  02.09.93  GH
 *-------------------------------------------------------------------
 */
int
CheckCit( citation_t *citation )
{
    int result  = CIT_BAD;

    if( CIT_VALUE(CIT_AUTHOR)[0] || CIT_DESC(CIT_AUTHOR)[0] )
    {
       result = CIT_NO_WORK;
       if( CIT_VALUE(CIT_WORK)[0] || CIT_DESC(CIT_WORK)[0] )
       {
          result = CIT_NO_LOW;
          if( CIT_VALUE(CIT_V)[0] || CIT_VALUE(CIT_W)[0] ||
              CIT_VALUE(CIT_X)[0] || CIT_VALUE(CIT_Y)[0] ||
              CIT_VALUE(CIT_Z)[0] )
          {
             result = CIT_GOOD;
          }
       }
    }
    return( result );

}/*
END OF CheckCit
*/



/********************************************************************
 *  None of the routines above this point are used outside of the
 *  citation.c source file.
 *
 *  These may be moved to another source file at some future time.
 *******************************************************************/



/*-------------------------------------------------------------------
 *   CitationLoadFromIdt     05.29.93 MM
 *
 *   Load the entire IDT file into a buffer.  Returns a pointer to
 *   the buffer, or NULL if an error occurs.
 *   One major source of possible error is if the IDT file is
 *   64KBytes ( 65536 ) or larger.  In this case TLGmalloc may
 *   fail and fread() will definitly fail. ( this is an MS-DOS-
 *   Turbo C bug. )
 *
 *-------------------------------------------------------------------
 */
BYTE *
CitationLoadFromIdt( FILE *idt_file )
{
    struct stat file_stat;
    BYTE *idt_buffer;
    UL calloc_size,read_size;

    rewind(idt_file);
    fstat(fileno(idt_file), &file_stat);
    /***************************************
     **      KLUDGE -- KLUDGE             **
     **  for 64K or larger IDT files      **
     **  See explanation in function      **
     **  header for details               **
     **  GAH ( 9-93 )                     **
     ***************************************/
    if( file_stat.st_size == MAXIDTSIZE )
    {
       /* Exactly 64K files, we just don't read the last byte */
       calloc_size = file_stat.st_size-1;
       idt_buffer = (BYTE *) calloc(calloc_size,(sizeof(BYTE)) );
       if( !idt_buffer )
          SYSTEMCRASH( EXIT_CITBADIDTMALLOC );
       read_size = fread(idt_buffer,sizeof(BYTE),calloc_size,idt_file);
       if( read_size != calloc_size )
          SYSTEMCRASH( EXIT_CITBADIDTREAD );
    }
    else if( file_stat.st_size > MAXIDTSIZE )
    {
        /* Greater then 64K, we give up */
        SYSTEMCRASH( EXIT_CITIDTTOOLARGE );
    }
    else
    {
       /* Less than 64K we handle normally */
       idt_buffer = ( BYTE * ) TLGmalloc(file_stat.st_size);
       read_size = fread(idt_buffer,1/*sizeof(BYTE)*/,file_stat.st_size,idt_file);
       if( read_size != file_stat.st_size )
          SYSTEMCRASH( EXIT_CITIDTTOOLARGE);
    }
    return(idt_buffer);
}/*
END of CitationLoadFromIdt */


/*-------------------------------------------------------------------
 *   WhereisCit              12.12.89  GH
 *
 *   Function to determine the block location of a user-specified
 *   Citation.  Puts location in block_loc and returns the
 *   updated buffer position if citation is found.
 *   Set block_loc to an error value if citation is not found
 *   and returns ( BYTE * ) NULL
 *   *buffer must be pointing to the very beginning of a buffer
 *   holding the entire IDT table for this to work reliably.
 *
 *   last modified  02.26.93
 *-------------------------------------------------------------------
 */
BYTE *
WhereisCit( BYTE *buffer, citation_t *citation, unsigned *block_num )
{

   BYTE *buf_ptr= buffer;
   /***************************************
    **      KLUDGE -- KLUDGE             **
    **                                   **
    **  This hack fixes a problem with   **
    **  searching for citations in the   **
    **  TLG Canon IDT table              **
    ***************************************/
    if( stricmp( CIT_DESC(CIT_AUTHOR),"TLG2" ) ==0 )
    {
       buf_ptr = FindLowBlock( buf_ptr, citation,
                               block_num );
       return( buf_ptr );
    }
 
    switch( CheckCit( citation ) )
    {
       case CIT_BAD     :  *block_num = CIT_NOT_FOUND;
                           buf_ptr = ( BYTE * ) NULL;
                           break;
       case CIT_NO_WORK :  buf_ptr = FindAuthBlock( buf_ptr, citation,
                                                    block_num );
                           break;
       case CIT_NO_LOW  :  buf_ptr = FindAuthBlock( buf_ptr, citation,
                                                    block_num );
                           if( buf_ptr != ( BYTE * )NULL )
                              buf_ptr = FindWorkBlock( buf_ptr, citation,
                                                       block_num );
                           break;
       case CIT_GOOD      :  buf_ptr = FindAuthBlock( buf_ptr, citation,
                                                    block_num );
                           if( buf_ptr != ( BYTE * )NULL )
                           {
                              buf_ptr = FindWorkBlock( buf_ptr, citation,
                                                       block_num );
                              if( buf_ptr != ( BYTE * )NULL )
                                 buf_ptr = FindLowBlock( buf_ptr, citation,
                                                         block_num );
                           }
                           break;
       default          :  SYSTEMCRASH( EXIT_WHEREISCIT );
                           break;
    }
    return( buf_ptr );
}


/*---------------------------------------------------------
 *   CitFromTextIndex        05-14-91
 *
 *   Build a citation structure for a specified line
 *   from the citation index array of a block
 *
 *   last modified :  05-14-91
 *---------------------------------------------------------
 */
void
CitFromTextIndex( char *cit_index[], int line_number,
          citation_t *citation )
{
    int i=0;

    InitCit( citation );
    for( i=0; i<=line_number; i++ )
	{
           DecodeBinCitBuf( (BYTE *) cit_index[i], citation );
	}
}/*
End of CitFromTextIndex
*/



/*---------------------------------------------------------
 *   CitStrfromStruc         05-15-91   GH
 *
 *   Creates a single string of citation structure values
 *   only.
 *
 *   last modified : 05-15-91  GH
 *---------------------------------------------------------
 */
 void
 CitStrfromStruc( citation_t *citation,  char *cit_str )
 {
    int i;

    cit_str[0] = '\0';
    for( i=CIT_AUTHOR; i<= CIT_Z; i++ )
    {
       if( strlen( CIT_VALUE(i) ) )
       {
          strcat(cit_str,CIT_VALUE(i));
          strcat(cit_str,".");
       }
       else
          strcat(cit_str,"0.");
    }
}



/*---------------------------------------------------------
 *    CitStrtoStruct
 *
 *    Convert a string of the form "v.w.x.y.z."
 *    to a citation structure.
 *    03-18-92   GH
 *---------------------------------------------------------
 */
void
CitStrtoStruct( char *cit_str , citation_t *citation )
{
    int i=strlen(cit_str)-1,level=CIT_Z,tmp_i=0;
    char tmp_level[20];

    InitCit(citation);
    while(i>=0)
    {
       tmp_i = 0;
       while((cit_str[i] != '.') && i>=0 )
           tmp_level[tmp_i++] = cit_str[i--];
       tmp_level[tmp_i] = '\0';
       if( tmp_i > 1 )
          strrev(tmp_level);
       strcpy(CIT_VALUE(level),tmp_level);
       level--;
       i--;
    }
}
