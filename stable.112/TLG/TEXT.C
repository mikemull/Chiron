/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\text.c 2.1 1993/11/21 16:17:22 mwm Exp mwm $
 *
 *  Routines for dealing with betatext in text and idt files.
 *
 *  $Log: text.c $
 * Revision 2.1  1993/11/21  16:17:22  mwm
 * removed obsolete SearchTextFile
 *
 * Revision 2.0  1993/09/15  20:34:11  mwm
 * Start of new version
 *
 * Revision 1.3  92/04/19  12:17:00  ROOT_DOS
 * numerous changes
 * 
 * Revision 1.2  91/09/05  18:08:21  ROOT_DOS
 * Added code to handle BETALETTER attributes.
 * 
 * Revision 1.1  91/09/05  17:26:08  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include <alloc.h>
#include <string.h>
#include <ctype.h>
#include "tlg.h"
#include "text.h"
#include "strsrch.h"
#include "tlgmem.h"
#include "inxfile.h"
#include "file.h"


/*-------------------------------------------------------------------
 *  WordIsInLine             05.25.91 MM
 *
 *  Returns true if word is found in line
 *-------------------------------------------------------------------
 */
BOOL
WordIsInLine( char *Word, char *Line )
{
        int count=0;
        char csave;

        SKIPHIGH(Line);
        TEXTCOUNT(Line,count);
        /*
        ** KLUDGE ALERT KLUDGE ALERT This is a BAD thing to do
        */
        csave=Line[count+1];
        Line[count+1] = '/0';

        if(MatchPattern( (BYTE *)Word, (BYTE *)Line, count ))
        {
                Debug("%s\n",Line);
                Line[count+1]=csave;
                return(TRUE);
        }
        Line[count+1]=csave;
        return(FALSE);
}/*
END of WordIsInLine */


/*---------------------------------------------------------
 *    IndexBlock             04-03-91
 *
 *    This routine indexs a block of text pointing one
 *    element of the array line_index at the beginning of
 *    each text line and one element of cit_index at the
 *    beginning of each citation entry.
 *
 *    last modified : 04-19-91
 *---------------------------------------------------------
 */
int
IndexBlock( BYTE *block, char *line_index[], char *cit_index[] )
{
	int line_count = 0;
        BYTE *beg_block=block;

        while( ((block-beg_block)<8192) && (*block != 0) )
	{
            cit_index[line_count] = (char *) block;
            SKIPHIGH(block);
            line_index[line_count] = (char *) block;
            while( !( HIBITSET( *block )) && ( *block ) )
               block++;
	    line_count++;
	}
        return( line_count );
}/*
END of IndexBlock */


/*-------------------------------------------------------------------'
 *    MassageWord            02.07.90  GH
 *
 *    Function to remove all unwanted chars from a Word and to
 *    replace or removed chars based on following rules.
 *    Grave Accent -> Acute Accent ( \ -> / )
 *    Final accent removed from word with two accents
 *    Internal breathing marks removed
 *
 *    last modified  02.24.90  MM
 *-------------------------------------------------------------------
 */

void
MassageWord( char *Word, char *MassagedWord )
{
        int Windex,MWindex=0,AccentCount=0,GrkCharCount=0,CharNum,WordLen;
        char C;

	WordLen = strlen(Word);
        for ( Windex = 0; Windex < WordLen ; Windex++ )
	{
           C = toupper(Word[Windex]);
           if( LetterVal(C) )
	   {
		GrkCharCount++;
                MassagedWord[MWindex++] = C;
	   }
	   else if ( CharNum = DiaVal( Word[Windex] ) )
	   {
		switch( CharNum )
		{
                  case 1 : GrkCharCount++;
                           MassagedWord[MWindex++] = C;
			   break;
                  case 2 :
                  case 3 : if( GrkCharCount <= 1 )
                              MassagedWord[MWindex++] = C;
			   break;
                  case 4 :
                  case 5 : if( !AccentCount++ )
                              MassagedWord[MWindex++] = C;
			   break;
                  case 6 : if( !AccentCount++ )
			      MassagedWord[MWindex++] = '/';
			   break;
                  case 7 :
                  case 8 :
                  case 9 : MassagedWord[MWindex++] = C;
			   break;
		}
	   }
	   else
		MassagedWord[MWindex++] = C;
	}
	MassagedWord[MWindex] = (char) 0x00;
}/*
END OF MassageWord
*/

