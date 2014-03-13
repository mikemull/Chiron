/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\awlst.c 2.1 1993/11/21 16:26:42 mwm Exp mwm $
 *
 *    Code to manipulate the TLGAWLST.INX file.
 *    TLGAWLST.INX contains one 3-byte entry for every
 *    work serial number in the TLG.
 *
 *  $Log: awlst.c $
 * Revision 2.1  1993/11/21  16:26:42  mwm
 * Changed realloc to TLGrealloc.
 *
 * Revision 2.0  1993/08/31  20:52:13  mwm
 * start of new version
 *
 * Revision 1.5  92/04/06  08:35:58  ROOT_DOS
 * Added SetAWIndex function
 * Added some comments
 * 
 * Revision 1.4  92/03/10  19:43:49  ROOT_DOS
 * Made AWlist a single structure
 * Added function to create AWlist from author num list
 * 
 * Revision 1.3  92/01/29  18:59:27  ROOT_DOS
 * Changes regarding complete re-write of AW list code
 * 
 * Revision 1.2  92/01/18  13:20:42  ROOT_DOS
 * Moved SetAuthPointer,SetWorkPointer from makelist to awlst.
 * 
 * Revision 1.1  91/12/02  18:54:50  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */

#include "tlg.h"
#include "tlgmem.h"
#include "awlst.h"

static AWlist AW = { 0, 0, 0, 0, NULL };


/*-------------------------------------------------------------------
 *  CreateAWList             10.23.90 MM
 *
 *  Make the linked AW list.
 *------------------------------------------------------------------
 */
int
CreateAWList( unsigned *serial_list )
{
        int i,num_works=0;

        AW.total_match = 0;
        /*
        ** Get count of works
        */
        for( i=0; i<MAXSERIAL; i++)
        {
                if(serial_list[i])
                                num_works++;
        }

        if(num_works > 0 )
        {
                AW.AWlst = (AWnums *) TLGmalloc( num_works*sizeof(AWnums) );

                num_works = 0;
                for( i=0; i<MAXSERIAL; i++)
                {
                        if(serial_list[i])
                        {
                           AW.AWlst[num_works] = DecodeAWEntry(&AWLSTbuf[i*3]);
                           AW.AWlst[num_works].count = serial_list[i];
                           AW.total_match += serial_list[i];
                           num_works++;
                        }
                }
        }

        TLGfree(serial_list);
        AW.size = num_works;
        return(num_works);
}/*
END of CreateAWList */


/*-------------------------------------------------------------------
 *     CreateAWListFromAuth
 *
 *     Create the AW list from author numbers
 *
 *-------------------------------------------------------------------
 */
int
CreateAWListFromAuth( int *auth_nums, int auth_count )
{
    int num_works=0,i,j,auth;
    BYTE *tmp_ptr;

    AW.AWlst = (AWnums *) TLGmalloc( MAXSERIAL*sizeof(AWnums) );

    for(i=0; i<auth_count; i++ )
    {
        tmp_ptr = AWLSTbuf;
        for(j=0; j<MAXSERIAL ; j++ )
        {
           if( ( auth=GetAuthNum(tmp_ptr) )==auth_nums[i] )
           {
               AW.AWlst[num_works].AuthorNumber = auth;
               AW.AWlst[num_works].WorkNumber = GetWorkNum(tmp_ptr);
               AW.AWlst[num_works].count = 0;
               num_works++;
           }
           tmp_ptr += 3;
        }
    }
    Tcl_ValidateAllMemory(__FILE__,__LINE__);
    AW.AWlst = ( AWnums * ) TLGrealloc( AW.AWlst, num_works*sizeof(AWnums) );
    Tcl_ValidateAllMemory(__FILE__,__LINE__);

    AW.curr_index = AW.auth_index = 0;
    AW.size = num_works;
    return( num_works );
}/*
END of CreateAWListFromAuth */



/*-------------------------------------------------------------------
 *  CreateFullAWList         04.06.92 MM
 *
 *  Make an AW list with every possible entry
 *-------------------------------------------------------------------
 */
BOOL
CreateFullAWList( void )
{
        int i;
        BOOL result = TRUE;

        AW.AWlst = (AWnums *) TLGmalloc( MAXSERIAL*sizeof(AWnums) );

        if(AW.AWlst)
        {
                for( i=0; i<MAXSERIAL; i++)
                {
                        AW.AWlst[i] = DecodeAWEntry(&AWLSTbuf[i*3]);
                        AW.AWlst[i].count = 0;
                }
        }
        else
                result = FALSE;

        AW.size = MAXSERIAL;
        return(result);
}/*
END of CreateFullAWList */


/*-------------------------------------------------------------------
 *   GetAuthNum              10.19.89  GH
 *
 *   Function to extract the 14-bit author number from a
 *   TLGAWLST.INX record.  The author number exists in the
 *   high order 14-bits of the 3-byte entry.
 *
 *-------------------------------------------------------------------
 */

unsigned
GetAuthNum( BYTE *Value )
{
	return (((((((unsigned)Value[0] << 8 ) & 0xff00 ) |
		    ((unsigned)Value[1] & 0x00ff ))
		      & 0xfffc ) >> 2 ) & 0x3fff);
}/*
END OF GetAuthNum */


/*-------------------------------------------------------------------
 *   GetWorkNum              10.19.89  GH
 *
 *   Function to extract the 10-bit work number from a TLGAWLST.INX
 *   record. The Work Number exists in the low order 10-bits of
 *   the record.
 *
 *-------------------------------------------------------------------
 */

unsigned
GetWorkNum( BYTE *Value )
{
	return ( ( ((((unsigned)Value[1] ) << 8 ) & 0xff00 ) |
		(((unsigned)Value[2] ) & 0x00ff ) ) & 0x03ff );
}/*
END OF GetWorkNum */



/*-------------------------------------------------------------------
 *   DecodeAWEntry           10.19.89  GH
 *
 *   Function to take a 3-byte entry from the AWLST file and return
 *   an author number and a work number. The function expects a
 *   pointer to a BYTE buffer containing the entry to be decoded
 *   as its argument.  DecodeAWEntry returns a structure which
 *   contains the Author and Work number extracted from the
 *   indicated entry.
 *
 *-------------------------------------------------------------------
 */
AWnums
DecodeAWEntry( BYTE *entry )
{
	AWnums structvar;

	structvar.AuthorNumber = GetAuthNum(entry);
	structvar.WorkNumber = GetWorkNum(entry);
	return structvar;
}/*
END OF DecodeAWEntry */



/*-------------------------------------------------------------------
 *  SetAuthIndex           09.10.90 MM
 *
 *  Sets the current and author indices to the indicated author
 *-------------------------------------------------------------------
 */
void
SetAuthorIndex( int author )
{
        int i=0;

	while(author--)
	{
           while( (AW.AWlst[i].AuthorNumber) == (AW.AWlst[i+1].AuthorNumber) )
                i++;
           ++i;
	}

        AW.curr_index = AW.auth_index = i;
}/*
END of SetAuthPointer */



/*-------------------------------------------------------------------
 *  SetWorkIndex           09.23.90 MM
 *
 *  Sets the index to the selected work
 *  09.23.90 MM
 *-------------------------------------------------------------------
 */
void
SetWorkIndex( int work_num )
{
        int i = AW.auth_index;

        while( AW.AWlst[i].WorkNumber != work_num )
                i++;

        AW.curr_index = i;
}/*
END of SetWorkIndex */


/*-------------------------------------------------------------------
 *  SetAWIndex
 *
 *  Sets the index to the indicated number if possible
 *  09.23.90 MM
 *-------------------------------------------------------------------
 */
BOOL
SetAWIndex( int new_index )
{
        BOOL good_index = TRUE;

        if( (new_index < AW.size) && (new_index >= 0) )
        {
                AW.curr_index = new_index;
                if(AW.AWlst[new_index].AuthorNumber !=
                   AW.AWlst[AW.auth_index].AuthorNumber )
                   AW.auth_index = new_index;
        }
        else
                good_index = FALSE;

        return(good_index);
}/*
END of SetAWIndex */



/*-------------------------------------------------------------------
 *  SetMatchTotal
 *-------------------------------------------------------------------
 */
void
SetMatchTotal( unsigned total )
{
        AW.total_match = total;
}/*
END of SetMatchTotal */


/*-------------------------------------------------------------------
 *  MatchTotal
 *-------------------------------------------------------------------
 */
UL
MatchTotal( void )
{
        return( AW.total_match );
}/*
END of MatchTotal */



/*-------------------------------------------------------------------
 *  CurrentAWIndex
 *-------------------------------------------------------------------
 */
int
CurrentAWIndex( void )
{
        return( AW.curr_index );
}/*
END of CurrentAWIndex */


/*-------------------------------------------------------------------
 *  AuthorAWIndex
 *-------------------------------------------------------------------
 */
int
AuthorAWIndex( void )
{
        return( AW.auth_index );
}/*
END of AuthorAWIndex */


/*-------------------------------------------------------------------
 *   AWAuthor
 *-------------------------------------------------------------------
 */
unsigned
AWAuthor( unsigned index )
{
        return( AW.AWlst[index].AuthorNumber );
}/*
END of AWAuthor */


/*-------------------------------------------------------------------
 * AWWork
 *-------------------------------------------------------------------
 */
unsigned
AWWork( unsigned index )
{
        return( AW.AWlst[index].WorkNumber );
}/*
END of AWWork */


/*-------------------------------------------------------------------
 *  AWSize
 *-------------------------------------------------------------------
 */
unsigned
AWSize(void)
{
        return(AW.size);
}/*
END of AWSize */


/*-------------------------------------------------------------------
 *  FreeAWList               03.10.92 MM
 *
 *  Frees up the AWList memory
 *-------------------------------------------------------------------
 */
void
FreeAWList( void )
{
        if(AW.AWlst != NULL)
                TLGfree(AW.AWlst);

        AW.AWlst = NULL;
}/*
END of FreeAWList */

