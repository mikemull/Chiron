/*====================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\citlow.c 2.1 1994/02/13 19:08:08 mwm Exp mwm $
 *
 *   Low Level re-usable Citation manipulation functions.
 *
 *   last modified  12-11-92 GH
 *   $Log: citlow.c $
 * Revision 2.1  1994/02/13  19:08:08  mwm
 * Fixed bug in incrementing alphabetical citations.
 *
 * Revision 2.0  1993/09/18  16:26:46  mwm
 * Start of new version.
 *
 *
 *-------------------------------------------------------------------
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <graphics.h>
#include "tlg.h"
#include "tlgmem.h"
#include "text.h"
#include "bit.h"
#include "citlow.h"

enum left_vals    { LEFT_Z=8,LEFT_Y,LEFT_X,LEFT_W,LEFT_V,LEFT_UNUSED,
                    LEFT_ESC,LEFT_SPECIAL
                  };
enum right_vals   { RGHT_INC,RGHT_LIT1,RGHT_LIT2,RGHT_LIT3,RGHT_LIT4,
                    RGHT_LIT5,RGHT_LIT6,RGHT_LIT7,RGHT_7BIT,RGHT_7C,
                    RGHT_7STR,RGHT_14BIT,RGHT_14C,RGHT_14STR,RGHT_C,
                    RGHT_STR
                  };

/****** MACROS  ******/

#define SPECIAL_EOSTR   15
#define SPECIAL_EOB     14
#define SPECIAL_EOF     0
#define ESC_AUTH        0
#define ESC_WORK        1
#define ESC_ABBREV      2
#define LEVEL_LESS      -1
#define LEVEL_GTR       1
#define LEVEL_EQUAL     0

/* FILE GLOBALS */

int decode_result=CIT_OK;    /* results of decodes for all to see */

/*
** Local prototypes
*/
static void
IncrementByOne( char *citation_val );


/*-------------------------------------------------------------------
 *   StrUpdate               10.24.89  GH
 *
 *   Searches a string for non-numeric char and replaces it with
 *   a user specified char. str is assumed to be NULL terminated.
 *
 *   last modified:  12.11.92  GH
 *-------------------------------------------------------------------
 */
BOOL
StrUpdate( char *str, char c )
{
    BOOL result=FALSE;
    while( *str )
	{
       if( isalpha(*str) )
       {
          *str++ = c;
          result=TRUE;
       }
	}
    return(result);
}/*
END OF StrUpdate
*/


/*-------------------------------------------------------------------
 *   LoadCitBuffer              10.25.89  GH
 *
 *   Reads all bytes with sign bit set into a buffer. buffer is
 *   an array of BYTE of size CIT_BUFF_SIZE. LoadBuffer returns the
 *   number of bytes actually read into the buffer. fptr is a file
 *   pointer to either a .txt or .idt tlg formated file.
 *
 *   last modified : 12.14.92  GH
 *-------------------------------------------------------------------
 */
unsigned
LoadCitBuffer( BYTE *buffer, FILE *fptr )
{
	unsigned counter=0;

    while(HIBITSET(buffer[counter++]=getc(fptr)) && (counter<CIT_BUFF_SIZE))
    if( counter>=CIT_BUFF_SIZE )
       SYSTEMCRASH( EXIT_LOADBUFFER );
    ungetc( buffer[--counter],fptr);
    buffer[counter] = '\0';
    return(counter);
}/*
END OF LoadCitBuffer
*/


/*-------------------------------------------------------------------
 *   ResetCitation           12.16.92  GH
 *
 *   Resets all lower citation levels when a higher level has
 *   been changed.
 *-------------------------------------------------------------------
 */
void
ResetCitation( int level_index, citation_t *citation )
{
   int i=0;

   if( ( level_index < CIT_AUTHOR ) || ( level_index > CIT_Z ) )
       SYSTEMCRASH( EXIT_BADLEVELINDEX );

   if ( level_index >= CIT_V )
   {
      for ( i=level_index + 1; i<=CIT_Z; i++ )
         sprintf(CIT_VALUE(i),"%u",1);
   }
   else
   {
      for ( i=level_index + 1; i<=CIT_Z; i++ )
         sprintf(CIT_VALUE(i),"%s","");
   }
}/*
END OF ResetCitation
*/


/*-------------------------------------------------------------------
 *   IncrementByOne          01.05.93  GH
 *
 *   Function to increment the current specified citation level
 *   by one. The following rules are used :
 *
 *   number => number++     ( 3 => 4 ) number, no string
 *   char   => char++       ( b => c ) string, no number
 *********** The following two cases are not currently implemnted **
 *   char+number => char+number++     ( a50 => a51 ) string, no number
 *   number+char => number+char++     ( 50d => 50e ) number, string
 *-------------------------------------------------------------------
 */
void
IncrementByOne( char *citation_val )
{
    int numeric_portion;
    char *string_portion;
    ParseLevelValue( citation_val, &numeric_portion, &string_portion );
    if( numeric_portion && !(*string_portion) )
        sprintf(citation_val,"%u",++numeric_portion );
    else if ( (*string_portion) && ! numeric_portion )
    {
        if( strlen( string_portion ) > 1 )
           sprintf( citation_val,"%s","*ERROR*");
        else
           sprintf( citation_val,"%c",++(*string_portion));
    }
}/*
END of IncrementByOne */



/*-------------------------------------------------------------------
 *   DecodeLeftNibble        10.25.89  GH
 *
 *   Function to interpret the left nibble of a binary encoded
 *   citation type byte.  Returns an BYTE ( unsigned char ) value
 *   representing the level( a,b,c,v,w,x,y or z ) being updated.
 *
 *   last modified : 12.15.92  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeLeftNibble( BYTE left_nibble, BYTE right_nibble, int *level_index,
                  BYTE *buffer )
{
    BYTE *buf_ptr = buffer;

    switch( left_nibble )
    {
       case LEFT_Z       : *level_index = CIT_Z;
                           break;
       case LEFT_Y       : *level_index = CIT_Y;
                           break;
       case LEFT_X       : *level_index = CIT_X;
                           break;
       case LEFT_W       : *level_index = CIT_W;
                           break;
       case LEFT_V       : *level_index = CIT_V;
                           break;
       case LEFT_UNUSED  : break;
       case LEFT_ESC     : switch( BitVal7( buf_ptr ) )
                          {
                             case ESC_AUTH   : buf_ptr++;
                                               *level_index = CIT_AUTHOR;
                                               break;
                             case ESC_WORK   : buf_ptr++;
                                               *level_index = CIT_WORK;
                                               break;
                             case ESC_ABBREV : buf_ptr++;
                                               *level_index = CIT_ABBREV;
                                               break;
                             default         : SYSTEMCRASH( EXIT_LEFTESC );
                          }
                          break;
       case LEFT_SPECIAL : switch( right_nibble )
                           {
                              case SPECIAL_EOSTR : *level_index = CIT_EOSTR;
                                                   break;
                              case SPECIAL_EOB   : *level_index = CIT_EOBLOCK;
                                                   break ;
                              case SPECIAL_EOF   : *level_index = CIT_EOFILE;
                                                   break;
                              default : SYSTEMCRASH(EXIT_LEFTSPECIAL);
                           }
                           break;
       default           :   SYSTEMCRASH( EXIT_LEFTNIBBLE );
    }
    return ( buf_ptr );
}/*
END DecodeLeftNibble
*/



/*-------------------------------------------------------------------
 *   DecodeRightNibble       10.25.89  GH
 *
 *   Function to interpret the right nibble of the binary encoded
 *   code bytes and adjust the citation record.
 *
 *   last modified : 12.15.92  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeRightNibble( citation_t *citation, BYTE right_nibble,
                   int level_index, BYTE *buffer )
{
	 unsigned level_val=0;
     char level_str[CIT_MAX_STR],level_char='\0';
     BYTE *buf_ptr=buffer;
     int i=0;

	 switch( right_nibble )
     {
         case RGHT_INC   : IncrementByOne( CIT_VALUE(level_index) );
                           break;
         case RGHT_LIT1  :
         case RGHT_LIT2  :
         case RGHT_LIT3  :
         case RGHT_LIT4  :
         case RGHT_LIT5  :
         case RGHT_LIT6  :
         case RGHT_LIT7  : sprintf( CIT_VALUE( level_index ),"%u",
                                    ((unsigned) right_nibble) );
                           break;
         case RGHT_7BIT  : level_val = BitVal7(buf_ptr);
                           buf_ptr++;
                           sprintf( CIT_VALUE(level_index ), "%u", level_val);
                           break;
         case RGHT_7C    : level_val = BitVal7(buf_ptr);
                           buf_ptr++;
                           level_char = (BYTE) BitVal7(buf_ptr);
                           buf_ptr++;
                           sprintf( CIT_VALUE( level_index ),"%u%c",
                                    level_val,level_char );
                           break;
         case RGHT_7STR  : level_val = BitVal7(buf_ptr);
                           buf_ptr++;
                           while( *buf_ptr != 0xff )
                           {
                              level_str[i++] = (BYTE) BitVal7(buf_ptr);
                              buf_ptr++;
                              if( i >= CIT_MAX_STR )
                                 SYSTEMCRASH( EXIT_RGHTLONGSTR );
                           }
                           level_str[i] = '\0';
                           buf_ptr++;
                           sprintf( CIT_VALUE( level_index ),"%u%s",
                                    level_val,level_str );
                           break;
         case RGHT_14BIT : level_val = BitVal14(buf_ptr,1);
                           buf_ptr += 2;
                           sprintf( CIT_VALUE( level_index ),"%u",level_val );
                           break;
         case RGHT_14C   : level_val = BitVal14(buf_ptr,1);
                           buf_ptr += 2;
                           level_char = (BYTE) BitVal7(buf_ptr);
                           buf_ptr++;
                           sprintf( CIT_VALUE( level_index ),"%u%c",
                                    level_val,level_char );
                           break;
         case RGHT_14STR : level_val = BitVal14(buf_ptr,1);
                           buf_ptr += 2;
                           while ( *buf_ptr != 0xff )
                           {
                              level_str[i++] = (BYTE) BitVal7(buf_ptr);
                              buf_ptr++;
                              if( i >= CIT_MAX_STR )
                                 SYSTEMCRASH( EXIT_RGHTLONGSTR );
                           }
                           level_str[i] = '\0';
                           buf_ptr++;
                           sprintf( CIT_VALUE( level_index ),"%u%s",
                                    level_val,level_str);
                           break;
         case RGHT_C     : level_char = (BYTE) BitVal7(buf_ptr);
                           buf_ptr++;
                           if(!StrUpdate(CIT_VALUE(level_index),level_char))
                              SYSTEMCRASH( EXIT_STRUPDATE );
                           break;
         case RGHT_STR   : while ( *buf_ptr != 0xff )
                           {
                              level_str[i++] = ( BYTE ) BitVal7(buf_ptr);
                              buf_ptr++;
                              if( i >= CIT_MAX_STR )
                                 SYSTEMCRASH( EXIT_RGHTLONGSTR );
                           }
                           level_str[i] = '\0';
                           buf_ptr++;
                           sprintf( CIT_VALUE( level_index ),"%s",level_str );
                           break;
         default         : SYSTEMCRASH( EXIT_RGHTNIBBLE );
                           break;
     }
     ResetCitation( level_index, citation );
     return( buf_ptr );
}/*
END OF DecodeRightNibble
*/



/*-------------------------------------------------------------------
 *   DecodeBinCitBuf         02.20.90  GH
 *
 *   Function to decode a binary encoded citation contained in
 *   a buffer.
 *
 *   12.15.92  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeBinCitBuf( BYTE *buf_ptr, citation_t *citation )
{
    BYTE *internal_buf;
    BYTE left_nibble= (BYTE) 0,right_nibble= (BYTE) 0;
    int level_index=0;

    internal_buf = buf_ptr;
    while ( HIBITSET(*internal_buf) )
    {
       left_nibble = (BYTE)  (( *internal_buf & 0xf0 ) >> 4 );
       right_nibble = (BYTE) ( *internal_buf++ & 0x0f );
       internal_buf = DecodeLeftNibble(left_nibble, right_nibble,
                                       &level_index, internal_buf );
       if((level_index <= CIT_Z ) && (level_index >= CIT_AUTHOR ))
       {
          internal_buf = DecodeRightNibble( citation, right_nibble,
                                            level_index, internal_buf);
           decode_result = CIT_OK;
       }
       else
          decode_result = level_index; /*CIT_EOSTR,CIT_EOBLOCK,CIT_EOFILE*/
    }
    return( internal_buf );
}/*
END OF DecodeBinCitBuf
*/



/*-------------------------------------------------------------------
 *   DecodeBinCit            10.23.89  GH
 *
 *   Function to read a binary encoded citation from either a
 *   .IDT or a .TXT file and adjust the citation structure
 *   passed to it to reflect the new citation. fptr must point
 *   to the beginning of a Binary Encoded Citation.  Returns
 *   CIT_OK or CIT_EOSTR, CIT_EOBLOCK, CIT_EOFILE.
 *
 *   last modified  12.16.92  GH
 *
 *-------------------------------------------------------------------
*/
int
DecodeBinCit( FILE *fptr, citation_t *citation )
{

    BYTE buffer[CIT_BUFF_SIZE], *buf_ptr;

    buf_ptr = buffer;
    LoadCitBuffer(buf_ptr,fptr);
    buf_ptr = DecodeBinCitBuf( buf_ptr, citation );
    return( decode_result );

}/*
END OF DecodeBinCit
*/



/*-------------------------------------------------------------------
 *   DecodeNewAuthWork  11.04.89  GH
 *
 *   Interprets Type Code 1 entries or Type 2 entries.
 *   Type Code 1 => New Author, Type Code 2 => New Work
 *   Previously DecodeType1or2.
 *
 *   last modified  01.18.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeNewAuthWork( BYTE *buffer, unsigned *len, unsigned *loc,
                   citation_t *citation )
{
    BYTE *buf_ptr=buffer+1;

    *len = BitVal16(buf_ptr,1);
    buf_ptr += 2;
    *loc = BitVal16(buf_ptr,1);
    buf_ptr += 2;
    buf_ptr = DecodeBinCitBuf( buf_ptr, citation );
    if( (decode_result == CIT_EOBLOCK) || (decode_result == CIT_EOFILE))
       return( (BYTE *) NULL );
    else
       return( buf_ptr );
}/*
END OF DecodeNewAuthWork
*/



/*-------------------------------------------------------------------
 *    DecodeNewSection            11.06.89  GH
 *
 *    Interprets Type Code 3 entries. Type Code 3 => New Section.
 *
 *    Previously DecodeType3
 *
 *    last modified  01.18.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeNewSection( BYTE *buffer, unsigned *sect_loc )
{
    BYTE    *buf_ptr = buffer+1;
    *sect_loc = BitVal16(buf_ptr,1);
    buf_ptr += 2;
    return( buf_ptr );

}/*
END OF DecodeNewSection
*/



/*-------------------------------------------------------------------
 *    DecodeNewFile          01.19.93  GH
 *
 *    Interprets Type Code 7 ( new file in combined idt ).
 *
 *    last modified  01.18.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeNewFile( BYTE *buffer, unsigned *cit_length, unsigned long *offset,
               unsigned *text_length, citation_t *citation )
{
    BYTE *buf_ptr=buffer+1;

    *cit_length = BitVal16(buf_ptr,1);
    buf_ptr += 2;
    memcpy(offset,buf_ptr,4);
    buf_ptr += 4;
    *text_length = BitVal16(buf_ptr,1);
    buf_ptr += 2;
    buf_ptr = DecodeBinCitBuf(buf_ptr, citation);
    if( (decode_result == CIT_EOBLOCK) || (decode_result == CIT_EOFILE))
       return( (BYTE *) NULL );
    else
       return( buf_ptr );
}/*
End of DecodeNewFile
*/



/*-------------------------------------------------------------------
 *   DecodeSectionStartCit   01.19.93  GH
 *   DecodeSectionEndCit     01.19.93  GH
 *   DecodeBlockEndCit       01.19.93  GH
 *
 *   These three routines are provided for consistancy of
 *   interface. They just call DecodeBinCitBuf directly
 *
 *   last modified 01.19.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeSectionStartCit( BYTE *buffer, citation_t *citation )
{
    BYTE *buf_ptr=buffer+1;

    buf_ptr = DecodeBinCitBuf(buf_ptr,citation);
    if((decode_result == CIT_EOBLOCK) || (decode_result == CIT_EOFILE))
       return( (BYTE *) NULL );
    else
       return( buf_ptr );
}

BYTE *
DecodeSectionEndCit( BYTE *buffer, citation_t *citation )
{
    BYTE *buf_ptr=buffer+1;

    buf_ptr = DecodeBinCitBuf(buf_ptr,citation);
    if((decode_result == CIT_EOBLOCK) || (decode_result == CIT_EOFILE))
       return( (BYTE *) NULL );
    else
       return( buf_ptr );
}

BYTE *
DecodeBlockEndCit( BYTE *buffer, citation_t *citation )
{
    BYTE *buf_ptr=buffer+1;

    buf_ptr = DecodeBinCitBuf(buf_ptr,citation);
    if((decode_result == CIT_EOBLOCK) || (decode_result == CIT_EOFILE))
       return( (BYTE *) NULL );
    else
       return( buf_ptr );
}
   


/*-------------------------------------------------------------------
 *   DecodeExceptionStart    01.19.93  GH
 *
 *   Decode type-code 11, start exception citation.
 *
 *   last modified 01.19.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeExceptionStart( BYTE *buffer, unsigned *block, citation_t *citation )
{
    BYTE *buf_ptr=buffer+1;

    *block = BitVal16(buf_ptr,1);
    buf_ptr += 2;
    buf_ptr = DecodeBinCitBuf( buf_ptr, citation );
    if((decode_result == CIT_EOBLOCK) || (decode_result == CIT_EOFILE))
       return( (BYTE *) NULL );
    else
       return( buf_ptr );
}

/*-------------------------------------------------------------------
 *   DecodeExceptionEnd      01.19.93  GH
 *
 *   Decode type-code 12, end exception citation.
 *
 *   last modified 01.19.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeExceptionEnd( BYTE *buffer, citation_t *citation )
{
    BYTE *buf_ptr=buffer+1;

    buf_ptr = DecodeBinCitBuf( buf_ptr, citation );
    if((decode_result == CIT_EOBLOCK) || (decode_result == CIT_EOFILE))
       return( (BYTE *) NULL );
    else
       return( buf_ptr );
}

/*-------------------------------------------------------------------
 *   DecodeAuthWorkDesc  11.04.89  GH
 *
 *   Interprets Type Code 16 entries. Type Code 16 => author/work
 *   description.
 *     level_id == 0 => author description
 *     level_id == 1 => work description
 *   Previously DecodeType16
 *
 *   last modified 01.18.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeAuthWorkDesc( BYTE *buffer, citation_t *citation)
{
    BYTE level_id, desc_len, *buf_ptr=buffer+1;
    int i=0;

    level_id = *buf_ptr;
    buf_ptr++;
    desc_len = *buf_ptr;
    buf_ptr++;
    for( i=0; i<desc_len; i++ )
    {
       if( i > CIT_MAX_STR )
          SYSTEMCRASH(EXIT_AWDESCLEN );
       CIT_DESC(level_id)[i] = *buf_ptr;
       buf_ptr++;
    }
    CIT_DESC(level_id)[i] = '\0';
    if( *buf_ptr == 0x00 )
       return( ( BYTE * ) NULL );
    else
       return( buf_ptr );
}/*
END OF DecodeAuthWorkDesc
*/



/*-------------------------------------------------------------------
 *   DecodeLowLevDesc 11.04.89  GH
 *
 *   Interprets Type Code 17 entries. Type Code 17 => level
 *   v through z descriptions.
 *     level_id == 0 =>
 *   Formerly DecodeLowLevDescTypeBuf
 *
 *   last modified 01.18.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeLowLevDesc( BYTE *buffer, citation_t *citation)
{
    BYTE level_id, desc_len, *buf_ptr=buffer+1;
    int i=0;

    level_id = 7 - *buf_ptr;
    buf_ptr++;
    desc_len = *buf_ptr;
    buf_ptr++;
    for( i=0; i<desc_len; i++ )
    {
       if( i > CIT_MAX_STR )
          SYSTEMCRASH(EXIT_LLDESCLEN );
       CIT_DESC(level_id)[i] = *buf_ptr;
       buf_ptr++;
    }
    CIT_DESC(level_id)[i] = '\0';
    if( *buf_ptr == 0x00 )
       return( ( BYTE * ) NULL );
    else
       return( buf_ptr );
}/*
END OF DecodeLowLevDesc
*/



/*-------------------------------------------------------------------
 *   DecodeCombinedHeader 11.04.89  GH
 *
 *   Decodes a combined header entry, type_code == 31
 *
 *   last modified 01.18.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
DecodeCombinedHeader( BYTE *buffer )
{
    BYTE *buf_ptr = buffer + 4;
    if( *buf_ptr == 0x00 )
       return( ( BYTE * ) NULL );
    else
       return( buf_ptr );
}



/*-------------------------------------------------------------------
 *   DecodeUnknownType13 05.31.93  GH
 *
 *   Decodes the type code 13 which appeared without documentation
 *   on the 'D' disk.  I don't know what this represents but it
 *   appears to always be 5 bytes long.... type_code == 13
 *
 *
 *-------------------------------------------------------------------
 */
BYTE *
DecodeUnknownType13 ( BYTE *buffer )
{
    BYTE *buf_ptr = buffer + 3;
    while(HIBITSET( *buf_ptr ) ) buf_ptr++;
    if( *buf_ptr == TYPE_EOF )
       return( ( BYTE * ) NULL );
    else
       return( buf_ptr );
}



/*-------------------------------------------------------------------
 *   InitCit                 11.09.89  GH
 *
 *   Initializes a citation. All members of the citation have the first
 *   value set to NULL .
 *
 *   last modified   01.18.93  GH
 *-------------------------------------------------------------------
 */
void
InitCit( citation_t *citation)
{

        int i=0;

        for(i=0; i< CIT_MAX_LEVELS; i++ )
        {
            CIT_DESC(i)[0] = '\0';
            CIT_VALUE(i)[0] = '\0';
        }
}/*
END OF InitCit
*/



/*-------------------------------------------------------------------
 *   GetFullCitFromBuf       11.07.89  GH
 *
 *   Function to read the full citation information from the
 *   beginning of a IDT or TXT 8K block buffer.
 *
 *   last modified  01.19.93  GH
 *-------------------------------------------------------------------
 */
BYTE *
GetFullCitFromBuf( BYTE *buffer,  citation_t *citation )
{
    BYTE *buf_ptr = buffer;
    unsigned dummy=0;
    unsigned long ldummy=0L;

    while( ( *buf_ptr != TYPE_NEWSECTION ) && *buf_ptr )
	{
       switch ( *buf_ptr )
       {
         case TYPE_NEWAUTH :
         case TYPE_NEWWORK : buf_ptr=DecodeNewAuthWork(buf_ptr,&dummy,
                                                       &dummy,citation);
                             break;
         case TYPE_NEWSECTION : buf_ptr=DecodeNewSection(buf_ptr,&dummy);
                                break;
         case TYPE_NEWFILE : buf_ptr=DecodeNewFile(buf_ptr,&dummy,&ldummy,
                                                   &dummy,citation);
                             break;
         case TYPE_BEGCIT  : buf_ptr=DecodeSectionStartCit( buf_ptr,citation);
                             break;
         case TYPE_ENDCIT  : buf_ptr=DecodeSectionEndCit( buf_ptr, citation );
                             break;
         case TYPE_LASTCIT : buf_ptr=DecodeBlockEndCit( buf_ptr, citation );
                             break;
         case TYPE_BEGEXCEPT : buf_ptr=DecodeExceptionStart(buf_ptr, &dummy,
                                                            citation );
                               break;
         case TYPE_ENDEXCEPT : buf_ptr=DecodeExceptionEnd(buf_ptr, citation );
                               break;
         case TYPE_AWDESC : buf_ptr=DecodeAuthWorkDesc(buf_ptr,citation);
                             break;
         case TYPE_LLDESC  : buf_ptr=DecodeLowLevDesc(buf_ptr,citation);
                             break;
         case TYPE_HEADER  : buf_ptr=DecodeCombinedHeader(buf_ptr );
                             break;
         case UNKNOWN_13   : buf_ptr = DecodeUnknownType13 ( buf_ptr );
                             break;
         default           : SYSTEMCRASH(EXIT_BADTYPECODE);
                             break;
       }
	}
    return( buf_ptr );
}/*
END OF GetFullCitfromBuf
*/



/*-------------------------------------------------------------------
 *    FindNextTypeCodeinIDT      11.15.89  GH
 *
 *    This function searches the specified IDTable beginning at
 *    the current file position for the first occurrance of the
 *    indicated TypeCode
 *
 *    last modified : 09.17.90 GH.
 *-------------------------------------------------------------------
 */
BOOL
FindNextTypeCodeinIDT( FILE *idt, int type_code )
{
	int  tc=0,i=0;
    BYTE *buffer;

    buffer = ( BYTE * ) TLGmalloc( CIT_BUFF_SIZE );
    while ( ( tc = fgetc(idt) )  !=  type_code )
	{
	    switch( tc )
	    {
           case TYPE_EOF        : return( FALSE );
           case TYPE_NEWAUTH    :
           case TYPE_NEWWORK    : fread(buffer,2,2,idt);
                                  LoadCitBuffer(buffer,idt);
                                  break;
           case TYPE_NEWSECTION : fread(buffer,2,1,idt);
                                  break;
           case TYPE_BEGCIT     :
           case TYPE_ENDCIT     :
           case TYPE_LASTCIT    : LoadCitBuffer(buffer,idt);
                                  break;
           case TYPE_BEGEXCEPT  : fread(buffer,2,1,idt);
                                  break;
           case TYPE_ENDEXCEPT  : fread(buffer,2,1,idt);
                                  break;
           case TYPE_AWDESC     :
           case TYPE_LLDESC     : fread(buffer,2,1,idt);
                                  for ( i=0; i < buffer[1]; i++ )
                                    fgetc(idt);
                                  break;
           case UNKNOWN_13      : fread( buffer,5,1,idt );
                                  break;
           default              : if( tc != TYPE_HEADER )
                                    SYSTEMCRASH(EXIT_BADTYPECODE);
                                  break;
        }
	}
	ungetc(tc,idt);
    TLGfree( buffer );
    return( TRUE );
}
/*
END OF  FindFirstTypeCodeinIDT
*/



/*-------------------------------------------------------------------
 *    FindNextTypeCodeinBuf      11.15.89  GH
 *
 *    This function searches the IDTable in memory beginning at
 *    the current buffer position for the first occurrance of the
 *    indicated TypeCode.  Returns the updated buffer pointer if
 *    found, returns ( BYTE * )NULL if not found.
 *
 *    last modified : 09.17.90 GH.
 *-------------------------------------------------------------------
 */
BYTE *
FindNextTypeCodeinBuf( BYTE *buffer, int type_code )
{
    BYTE *buf_ptr=buffer;
    citation_t *citation;
    unsigned dummy;
    unsigned long ldummy;

    citation = ( citation_t * ) TLGmalloc( sizeof( citation_t ) );

    while( *buf_ptr != type_code )
    {
       switch ( *buf_ptr )
       {
         case TYPE_EOF        : TLGfree( citation );
                                return( ( BYTE * )NULL );
         case TYPE_NEWAUTH    :
         case TYPE_NEWWORK    : buf_ptr=DecodeNewAuthWork(buf_ptr,&dummy,
                                                          &dummy,citation);
                                break;
         case TYPE_NEWSECTION : buf_ptr=DecodeNewSection(buf_ptr,&dummy);
                                break;
         case TYPE_NEWFILE    : buf_ptr=DecodeNewFile(buf_ptr,&dummy,&ldummy,
                                                      &dummy,citation);
                                break;
         case TYPE_BEGCIT     : buf_ptr=DecodeSectionStartCit( buf_ptr,citation);
                                break;
         case TYPE_ENDCIT     : buf_ptr=DecodeSectionEndCit( buf_ptr, citation );
                                break;
         case TYPE_LASTCIT    : buf_ptr=DecodeBlockEndCit( buf_ptr, citation );
                                break;
         case TYPE_BEGEXCEPT  : buf_ptr=DecodeExceptionStart(buf_ptr, &dummy,
                                                             citation );
                                break;
         case TYPE_ENDEXCEPT  : buf_ptr=DecodeExceptionEnd(buf_ptr, citation );
                                break;
         case TYPE_AWDESC     : buf_ptr=DecodeAuthWorkDesc(buf_ptr,citation);
                                break;
         case TYPE_LLDESC     : buf_ptr=DecodeLowLevDesc(buf_ptr,citation);
                                break;
         case TYPE_HEADER     : buf_ptr=DecodeCombinedHeader(buf_ptr );
                                break;
         case UNKNOWN_13      : buf_ptr = DecodeUnknownType13( buf_ptr );
                                break;
         default              : SYSTEMCRASH(EXIT_BADTYPECODE);
                                break;
       }

	}
    TLGfree( citation );
    return( buf_ptr );
}
/*
END OF  FindNextTypeCodeinBuf
*/

/*-------------------------------------------------------------------
 *    ParseLevelValue        11.21.89  GH
 *
 *    Takes a pointer to a string and then parses the string
 *    returning a numeric and an ASCII string portion.
 *    a level value beginning with a char is all string regardless
 *    of any additional digits, a value beggining with a digit
 *    has a numeric portion and may have a string portion as well.
 *
 *    last modified  01.21.93  GH
 *-------------------------------------------------------------------
 */
void
ParseLevelValue( char *string, int *num_port, char **str_port )
{
    *str_port = string;
    *num_port = 0;
    if( isdigit( **str_port ) )
	   *num_port = (int) strtol(string,str_port,10);
}/*
END OF ParseLevelValue
*/



/*-------------------------------------------------------------------
 *    LvlCmp                 11.29.89 GH
 *
 *    Function to compare two citation levels and return an
 *    integer value indicating the relationship of the two levels
 *    level1 < level2 returns   <0
 *    level1 = level2 returns  ==0
 *    level1 > level2 returns   >0
 *
 *    last modified 11.29.89  GH
 *-------------------------------------------------------------------
 */
int
LvlCmp( char *string1, char *string2 )
{
    char *string1_str,*string2_str;
    int  string1_num=0,string2_num=0;

    ParseLevelValue(string1,&string1_num,&string1_str);
    ParseLevelValue(string2,&string2_num,&string2_str);
    if( string1_num == string2_num )
       return( strcmp( string1_str,string2_str ));
    else if( string1_num < string2_num )
       return LEVEL_LESS;
	else
       return LEVEL_GTR;
}/*
END OF LvlCmp
*/



/*-------------------------------------------------------------------
 *   CompareCit              12-7-89  GH
 *
 *   Function to compare two citations.
 *   citation1 < citation2 returns <0
 *   citation1 == citation2 returns ==0
 *   citation 1 > citation2 returns >0
 *
 *   last modified  01.25.93  GH
 *-------------------------------------------------------------------
 */
int
CompareCit( citation_t *citation1, citation_t *citation2 )
{
    int i=0,comp_res=0;

    for ( i=0; i<=CIT_MAX_LEVELS; i++ )
	{
        if( strlen( (*citation1)[i].level_val ) &&
            strlen( (*citation2)[i].level_val ) )
        {
           comp_res = LvlCmp( ((*citation1)[i].level_val),
                              ((*citation2)[i].level_val) );
           if( comp_res ) return( comp_res );
        }
	}
    return( 0 );
}/*
END OF COMPARECIT
*/
