/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\inxfile.c 2.0 1993/09/15 20:31:58 mwm Exp mwm $
 *
 *   These are utilities routines for dealing with the index (.INX)
 *   files TLGWLINX.INX (word list index) and TLGWCINX.INX (word
 *   counts index.  See TLG documentation for more detail.
 *
 *   $Log: inxfile.c $
 * Revision 2.0  1993/09/15  20:31:58  mwm
 * Start of new version
 *
 *
 *-------------------------------------------------------------------
 */

#include <ctype.h>
#include <string.h>
#include "tlg.h"
#include "inxfile.h"

/*-------------------------------------------------------------------
 *   LetterVal               9.24.89 MM
 *
 *   Returns the value (1-26) associated with a Greek letter in
 *   Betacode.
 *
 *-------------------------------------------------------------------
 */

unsigned
LetterVal( char Letter )
{
        static char letters[]="'ABGDEVZHQIKLMNCOPRSTUFXYW";
        int i;

	Letter = toupper(Letter);
        for( i=0; i<26; i++)
                if (Letter == letters[i])
                        return(i+1);

        return(0);
}/*
END of LetterVal
*/

/*-------------------------------------------------------------------
 *   DiaVal                  10.15.89 MM
 *
 *   Returns the betacode value for a given diacritical
 *
 *-------------------------------------------------------------------
 */
unsigned
DiaVal( char C )
{
        static char diacrits[] = "|)(/=\\+";
	int i=0;

        for( i=0; i<7 ; i++ )
	{
		if( diacrits[i] == C )
                    return (i+1);
	}
	return 0;
}/*
END of DiaVal
*/

/*-------------------------------------------------------------------
 *   WordVal                 9.24.89 MM
 *
 *   Computes the number of bytes to sum (see SumBytes) in the WLINX
 *   file to find block offset of word.  Value is based on first two
 *   letters of Word
 *
 *-------------------------------------------------------------------
 */

unsigned
WordVal( char *Word )
{
        int FirstLetter=0,SecondLetter=0,i=0;

        while( (FirstLetter==0) && (i<strlen(Word)) )
                FirstLetter = LetterVal( Word[i++] );

        while( (SecondLetter==0) && (i<strlen(Word)) )
                SecondLetter = LetterVal( Word[i++] );

        return( ((FirstLetter -1)*27 ) + SecondLetter );
}/*
END of WordVal
*/



/*-------------------------------------------------------------------
 *   SumBytes                9.24.89 MM
 *
 *   Sums the bytes in the WLINX file (contained in core in the array
 *   LINXbuf).  This gives the block offset of the word in the word
 *   list and the place to look in WCINX for the count offset.
 *
 *-------------------------------------------------------------------
 */

unsigned
SumBytes( unsigned count )
{
        unsigned i,Sum=0;

        for(i=0; i<=count; i++)
                Sum += (unsigned) LINXbuf[i];

        return(Sum);
}/*
END of SumBytes
*/


/*-------------------------------------------------------------------
 *   WordCountOffset         9.24.89 MM
 *
 *   Returns byte offset into WCNTS based on BlockNumber from
 *   SumBytes
 *
 *-------------------------------------------------------------------
 */

unsigned long
WordCountOffset( unsigned BlockNumber )
{
        return( CINXbuf[BlockNumber] );
}/*
END of WordCountOffset
*/

