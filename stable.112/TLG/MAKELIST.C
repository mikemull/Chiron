/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\makelist.c 2.0 1993/09/18 16:22:07 mwm Exp mwm $
 *
 *  (Previously findword.c)
 *  Make various lists including AW list, author name list, work name
 *  list, and filtered work name list.  The AW list is a linked list
 *  but all the rest are simple char * arrays.  See also ll.c and
 *  wcnts.c
 *  09.18.90 MM
 *  $Log: makelist.c $
 * Revision 2.0  1993/09/18  16:22:07  mwm
 * Start of new version.
 *
 * Revision 1.3  92/01/29  18:58:39  ROOT_DOS
 * Changes regarding complete re-write of AW list code
 * 
 * Revision 1.2  92/01/18  13:22:17  ROOT_DOS
 * Moved SetAuthPointer,SetWorkPointer from makelist to awlst.
 * 
 * Revision 1.1  91/12/02  18:54:26  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include <string.h>
#include "tlg.h"
#include "win.h"
#include "chiron.h"
#include "makelist.h"
#include "awlst.h"
#include "citlow.h"
#include "citation.h"
#include "tlgmem.h"
#include "wlist.h"
#include "entry.h"
#include "callback.h"
#include "atab.h"

/*-------------------------------------------------------------------
 *  createAuthorNameList     09.10.90  MM
 *
 *  Make list of author names from author numbers in AW list.
 *-------------------------------------------------------------------
 */
unsigned
createAuthorNameList( char *Authors[] )
{
   unsigned i,awindex;

   i= awindex =0;

   do
   {
      Authors[i] = (char *) TLGmalloc(81);
      (void)ATABNumbertoName( AWAuthor(awindex), Authors[i] );
      while( AWAuthor(awindex) == AWAuthor(awindex+1) )
         awindex++;
      awindex++;
      i++;
   }while( awindex < AWSize() );

   Authors[i] = NULL;
   return(i);
}/*
END of createAuthorNameList
*/

/*-------------------------------------------------------------------
 *    GetWorks               11.13.89  GH
 *
 *    Function to read the specified IDT and return two arrays of
 *    pointers, 1 to works names and 1 to block locations.
 *
 *-------------------------------------------------------------------
 */
unsigned
GetWorks( FILE *idt, char *wn[] , int wl[], int wnum[] )
{
    citation_t citation_info,*citation;
	unsigned len=0,loc=0,workcount=0;
	BYTE typecode;
    BYTE *idt_buffer,*p_buffer,*currloc;
	char *temp1,*temp2;
	int index=0;

    citation = &citation_info;
    idt_buffer = CitationLoadFromIdt( idt );

    p_buffer = FindNextTypeCodeinBuf(idt_buffer,2);
    typecode = *p_buffer;
	while ( ( typecode != 0x01 ) && ( typecode != 0x00 ) )
	{
	    temp1 = (char *)TLGmalloc(41);
	    workcount++;
        currloc = p_buffer;
        p_buffer = DecodeNewAuthWork(p_buffer,&len,&loc,&citation_info);
        p_buffer = FindNextTypeCodeinBuf(p_buffer,16);
        p_buffer = DecodeAuthWorkDesc( p_buffer,&citation_info);
            wn[index] = strncpy(temp1,CIT_DESC( CIT_WORK ),40);
	    wl[index] = loc;
        wnum[index] = atoi(CIT_VALUE( CIT_WORK ));
        p_buffer = currloc + len;
        typecode = *p_buffer;
        index++;
	}
    wn[index]=NULL;
    TLGfree( idt_buffer );
	return workcount;
}/*
END OF GetWorks
*/

/*-------------------------------------------------------------------
 *  FilterWorkList	     09.17.90 MM
 *
 *  Take the list of all works and produce a list of those in
 *  which the word occurs by matching the work numbers against
 *  the AW list.
 *  09.17.90 MM
 *-------------------------------------------------------------------
 */
unsigned
FilterWorkList( int num_works, int work_nums[],
		char **all_works, char **filtered_list,
                int *all_blocks, int *filtered_blocks,int *filtered_end,
		int *all_wnums,  int *filtered_wnums )
{
        register i,num=0,authindex,currauth;

        authindex = AuthorAWIndex();
        currauth = AWAuthor(authindex);

        while( AWAuthor(authindex) == currauth )
	{
		for(i=0;i<num_works;i++)
		{
                        if(  AWWork(authindex) == work_nums[i] )
			{
				*filtered_list++ = all_works[i];
				*filtered_blocks++ = all_blocks[i];
                                *filtered_end++ = (i<(num_works-1))
                                                        ? all_blocks[i+1]
                                                        : (-1) ;
				*filtered_wnums++ = all_wnums[i];
				num++;
			}
		}
                authindex++;
	}
	*filtered_list = NULL;
	return(num);
}/*
END of FilterWordList
*/

/*-------------------------------------------------------------------
 *  freeNameList             09.10.90 MM
 *
 *  Frees elements of a name list
 *-------------------------------------------------------------------
 */
void
freeNameList( char **List )
{
	while( *List )
        {
		TLGfree( *List );
		List++;
	}
}/*
END of freeNameList */

