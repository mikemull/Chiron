/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\wcnts.c 2.1 1993/11/21 16:40:56 mwm Exp mwm $
 *
 *   Code to manipulate the TLGWCNTS.INX file
 *   11.4.89
 *   $Log: wcnts.c $
 * Revision 2.1  1993/11/21  16:40:56  mwm
 * Changed for new word list structure.
 * Changed realloc to TLGrealloc.
 *
 * Revision 2.0  1993/09/15  20:33:03  mwm
 * Start of new version
 *
 * Revision 1.1  91/11/24  13:02:47  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include <alloc.h>
#include <string.h>
#include <graphics.h>
#include "tlg.h"
#include "tlgmem.h"
#include "inxfile.h"
#include "wcnts.h"
#include "bit.h"

/*-------------------------------------------------------------------
 *   GetWCEntry           10.09.89 GH
 *
 *   Returns a pointer to the appropriate word counts entry.
 *   11.4.89 MM
 *-------------------------------------------------------------------
 */
BYTE *
GetWCEntry( WORDLOC WordLoc, FILE *fptr )
{
	BYTE *WordCountsPtr,*BaseCountPtr;
	unsigned BlockSize=0,i=0;

        fseek( fptr, WordCountOffset(WordLoc.start_wlblock), 0);
        BlockSize = WordCountOffset(WordLoc.start_wlblock+1)
                   -WordCountOffset(WordLoc.start_wlblock);
	BaseCountPtr = WordCountsPtr = (BYTE *) TLGmalloc( BlockSize );
	CHECKREAD(WordCountsPtr,1,BlockSize,fptr);

	while( --WordLoc.WordNum )
        {
		if( HIBITSET(WordCountsPtr[1]) )
		     WordCountsPtr += (BitVal15(WordCountsPtr+1,1 )+1);
                else
		     WordCountsPtr += BitVal7(WordCountsPtr+1);
		WordCountsPtr += 2;
	}

	if( HIBITSET(WordCountsPtr[1]) )
		BlockSize = BitVal15(WordCountsPtr+1,1)+3;
	else
		BlockSize = BitVal7(WordCountsPtr+1)+2;

	for (i=0; i<BlockSize; i++)
		BaseCountPtr[i] = WordCountsPtr[i];

    BaseCountPtr= (BYTE *) TLGrealloc( BaseCountPtr, (size_t) BlockSize );
	return(BaseCountPtr);
}/*
END OF GetWCEntry
*/

/*-------------------------------------------------------------------
 *   DecodeWCEntry           10.09.89 GH
 *
 *   Function to decode the current entry in the WCNTS buffer.
 *   09.19.90 MM
 *-------------------------------------------------------------------
 */
UL
DecodeWCEntry( WORD *words[], unsigned *slist, int cnt )
{
        unsigned  count,listlen=0,serialnum=0;
	UL 	  NumWords=0;
        BYTE      *startptr;
        BYTE      *endptr,*bptr;
        extern    FILE *TLGwcnts;

        while(cnt--)
        {
                serialnum=0;
                startptr = GetWCEntry( words[cnt]->word_loc, TLGwcnts );
                bptr = (BYTE *) startptr;
                bptr++;
                if ( HIBITSET(*bptr) )
                {
                        listlen = BitVal15(bptr,1);
                        bptr += 2;
                }
                else
                {
                        listlen = BitVal7(bptr);
                        bptr++;
                }
                endptr = bptr + listlen;

                do
                {
                   count = BitVal2(bptr);

                   if ( HIBITSET(*bptr)  )
                   {
                        serialnum += BitVal13(bptr,1);
                        bptr += 2;
                   }
                   else
                   {
                        serialnum += BitVal5(bptr);
                        bptr++;
                   }

                   if ( count )
                        slist[serialnum-1] += count;
                   else if ( *bptr == 0 )
                   {
                        slist[serialnum-1] += BitVal16(bptr+1,1);
                        bptr += 3;
                   }
                   else if ( HIBITSET(*bptr) )
                   {
                        slist[serialnum-1] += BitVal15(bptr,1);
                        bptr += 2;
                   }
                   else
                   {
                        slist[serialnum-1] += BitVal7(bptr);
                        bptr++;
                   }
                   NumWords += (UL) slist[serialnum-1];
                } while ( bptr < endptr );
                TLGfree(startptr);

        }/*END of while*/

        return(NumWords);
}/*
END OF DecodeWCEntry
*/
