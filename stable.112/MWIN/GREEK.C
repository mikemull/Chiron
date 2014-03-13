/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\greek.c 2.0 1993/09/18 16:36:47 mwm Exp mwm $
 *
 *  This file contains code to write Greek characters, including
 *  diacriticals.
 *
 *     void PrintGreekString( int xp, int yp, char *beta_string )
 *     int PrintGreekChar( int cpX, int cpY, BETALETTER bchar, int type )
 *     char *GetBetaLetter( char *buffer, BETALETTER *blp )
 *     void AddToBetaLetter( char c, BETALETTER *blp )
 *     char *GetBetaLine( char *gbl_buf, BETALETTER *betaline,
 *                        int print_line_len,int *char_count )
 *
 *  $Log: greek.c $
 * Revision 2.0  1993/09/18  16:36:47  mwm
 * Start of new version.
 *
 * Revision 1.5  92/05/30  15:15:13  ROOT_DOS
 * Changed code to handle escapes
 * 
 * Revision 1.4  92/05/30  13:00:06  ROOT_DOS
 * Added kludge to handle highlighting bug
 * Added code to handle iota subscripts
 * 
 * Revision 1.3  91/12/02  19:19:16  ROOT_DOS
 * Fixed character highlighting for multiple matches
 * 
 * Revision 1.2  91/09/05  18:09:12  ROOT_DOS
 * Added code to handle BETALETTER attributes.
 * 
 * Revision 1.1  91/09/04  17:29:18  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include <stdio.h>
#include <graphics.h>
#include <ctype.h>
#include "tlg.h"
#include "greek.h"
#include "win.h"
#include "strsrch.h"
#include "inxfile.h"
#include "work.h"
#include "g.fnt"

BETALETTER empty_betaletter = { SPACE,0,0,0,0,OR_PUT };

static BOOL latin_mode=FALSE;

/*-------------------------------------------------------------------
 *  PrintGreekString         05.19.91 MM
 *
 *  Prints a null or high-bit-set terminated string of betatext in
 *  Greek.
 *    05.19.91 MM
 *-------------------------------------------------------------------
 */
void
PrintGreekString( int xp, int yp, char *beta_string )
{
        BETALETTER outstring[80];
        int count,i=0;

        for(i=0;i<80;i++)
                outstring[i]=empty_betaletter;
        GetBetaLine( beta_string, outstring, 80, &count);
        i=0;
        while(i<count)
                xp+=PrintGreekChar(xp,yp,outstring[i++]);
}/*
END of PrintGreekString  */


/*-------------------------------------------------------------------
 *  PrintGreekChar           10.17.90 MM
 *
 *  Print the given betaletter at X,Y.
 *-------------------------------------------------------------------
 */
int
PrintGreekChar( int cpX, int cpY, BETALETTER bchar)
{
        int i;

        if(bchar.dc_num != LCHAR )
        {
           putimage( cpX, cpY, (void far *)&Greek[bchar.charcode][0],
                     bchar.attrib );
           for(i=0;i<bchar.dc_num;i++)
           {
              if( bchar.dc_codes[i] == IOTASUBCODE )
              {
                putimage( cpX, cpY+1, (void far *) &Greek[bchar.dc_codes[i]][0],
                          XOR_PUT );
              }
              else
              {
                putimage( cpX, cpY-3, (void far *) &Greek[bchar.dc_codes[i]][0],
                          XOR_PUT );
              }
           }
        }
        else
                outtextxy(cpX,cpY,bchar.dc_codes);

        return( CHARWIDTH );
}/*
END of PrintGreekChar
*/

/*-------------------------------------------------------------------
 *  GetBetaLetter            10.17.90 MM
 *
 *  Given a buffer pointer, construct a betaletter from the sequence
 *  of characters.
 *
 *  last modified : 04.06.91  GH
 *-------------------------------------------------------------------
 */
char *
GetBetaLetter( char *buffer, BETALETTER *blp )
{
	BYTE c;
        int charcode,i,num_match;
        BYTE  *q;
        BYTE  *r;
        BYTE  *s;

        q = (BYTE *) buffer;
        num_match = WorkMatchCount();
	*blp = empty_betaletter;

        for(i=0;i<num_match;i++)
        {
                r = (BYTE *) WorkMatchBegin(i);
                s = (BYTE *) WorkMatchEnd(i);

                /*
                ** KLUDGE This should never occur but it does because of
                ** half-assed pc-segment shit
                */
                if( r>s) break;

                if( (q >= r) && (q < s) )
                {
                        blp->attrib = NOT_PUT;
                        break;
                }
        }

	while(ISESCAPE(*buffer))
	{
		if( *buffer == '&' )
                  latin_mode = TRUE;
		else if ( *buffer == '$' )
                  latin_mode = FALSE;

                blp->dc_num = LCHAR; blp->dc_codes[0] = *buffer;
                buffer++;
                return(buffer);
        }

	if(*buffer == '*')
	{
		buffer++;
		while( charcode = DiaVal( *buffer ) )
		{
		   AddToBetaLetter(*buffer,blp);
		   buffer++;
		}
		c = *buffer;
	}
        else
        {
                if (isalpha(*buffer) && !latin_mode)
                        c= tolower(*buffer);
                else
                        c= *buffer;
        }

	if( (*buffer=='S') && !(DiaVal(*(buffer+1)) || LetterVal(*(buffer+1))))
	{
		if( c == 'S' )
		   c = 'J';
		else
		   c = 'j';
	}

	AddToBetaLetter(c,blp);
        buffer++;
        while( charcode = DiaVal( *buffer ) )
        {
                AddToBetaLetter(*buffer,blp);
                buffer++;
        }

        if(HIBITSET(*(buffer+1))) latin_mode = FALSE;
        return(buffer);
}/*
END of GetBetaLetter
*/

/*-------------------------------------------------------------------
 *  AddToBetaLetter          10.17.90 MM
 *
 *  Add the chracter to the betaletter.
 *
 *  last modified : 04.02.91 GH
 *-------------------------------------------------------------------
 */
void
AddToBetaLetter( char c, BETALETTER *blp )
{

        if(latin_mode || isdigit(c) )
        {
                blp->dc_num = LCHAR;
                blp->dc_codes[0] = c;
        }
        else if (!DiaVal(c))
        {
		blp->charcode = c;
                blp->dc_num = 0;
        }
        else
	{
	   if( blp->dc_num == 0 )
	      blp->dc_codes[blp->dc_num++] = c;
	   else
	   {
	      if( blp->dc_codes[(blp->dc_num)-1] == '(' )
	      {
		 switch ( c )
		 {
		    case '\\' : blp->dc_codes[(blp->dc_num)-1] = '{';
				break;
		    case '/'  : blp->dc_codes[(blp->dc_num)-1] = '[';
				break;
		    case '='  : blp->dc_codes[(blp->dc_num)-1] = '<';
				break;
		 }
	      }
	      else if ( blp->dc_codes[(blp->dc_num)-1] == ')' )
	      {
		 switch ( c )
		 {
		    case '\\' : blp->dc_codes[(blp->dc_num)-1] = '}';
				break;
		    case '/'  : blp->dc_codes[(blp->dc_num)-1] = ']';
				break;
		    case '='  : blp->dc_codes[(blp->dc_num)-1] = '>';
				break;
		 }
	      }
	      else
		 blp->dc_codes[blp->dc_num++] = c;
	   }
	}
	return;
}/*
END of AddToBetaLetter
*/

/*-------------------------------------------------------------------
 *  GetBetaLine              03.18.90 GH
 *
 *  Given a buffer pointer, construct a betaletter line from the
 *  sequence of characters.
 *
 *  Last Modified : 03.25.91 GH
 *-------------------------------------------------------------------
 */
char *
GetBetaLine( char *gbl_buf, BETALETTER *betaline, int print_line_len,
	    int *char_count )
{
    *char_count = 0;

    while((*char_count < print_line_len) && ( !HIBITSET(*gbl_buf)) &&
	  (*gbl_buf))
    {
	    gbl_buf = GetBetaLetter(gbl_buf,&betaline[*char_count]);
	    (*char_count)++;
    }
    return(gbl_buf);

}/*
END of GetBetaLine
*/

