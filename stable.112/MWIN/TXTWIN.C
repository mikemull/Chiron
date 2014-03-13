
/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\txtwin.c 2.0 1993/09/18 16:32:45 mwm Exp mwm $
 *
 *  Code for the Text Window object
 *
 *  $Log: txtwin.c $
 * Revision 2.0  1993/09/18  16:32:45  mwm
 * Start of new version.
 *
 * Revision 1.15  92/06/01  17:53:56  ROOT_DOS
 * Move line index moving into work.c
 * Added horizontal scrolling
 * 
 * Revision 1.14  92/05/19  17:56:52  ROOT_DOS
 * Changes to scrolling
 * 
 * Revision 1.13  92/05/13  19:07:05  ROOT_DOS
 * Added logic to correctly position lines in fillScreen.
 * 
 * Revision 1.12  92/05/03  12:36:01  ROOT_DOS
 * Added call to SetNewObject
 * 
 * Revision 1.11  92/04/12  17:51:56  ROOT_DOS
 * Numerous changes to scrolling functions
 * 
 * Revision 1.10  92/04/02  14:21:31  ROOT_DOS
 * Fixed citation indexing bug
 * Removed SetNewCit function
 * 
 * Revision 1.9  92/03/09  20:24:13  ROOT_DOS
 * Added focuson and focusoff procs
 * 
 * Revision 1.8  92/03/08  19:35:32  ROOT_DOS
 * Added SetNewCit function
 * 
 * Revision 1.7  92/03/08  17:13:39  ROOT_DOS
 * Changed default structure so that R is alt key and nullProc is NULL
 * 
 * Revision 1.6  92/02/09  14:14:49  ROOT_DOS
 * Added border highlighting
 * 
 * Revision 1.5  91/12/01  16:40:33  ROOT_DOS
 * Added code to show next match on ctrl-N
 * 
 * Revision 1.4  91/12/01  14:15:54  ROOT_DOS
 * Changed keyproc arguments to add modifiers
 * 
 * Revision 1.3  91/12/01  13:32:46  ROOT_DOS
 * Removed work management routines out to work.c
 * 
 * Revision 1.2  91/09/05  18:10:01  ROOT_DOS
 * Added code to handle BETALETTER attributes.
 * 
 * Revision 1.1  91/09/04  17:35:44  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <ctype.h>
#include <dos.h>
#include "tlg.h"
#include "citlow.h"
#include "citation.h"
#include "win.h"
#include "button.h"
#include "txtwin.h"
#include "callback.h"
#include "editkey.h"
#include "notice.h"
#include "text.h"
#include "greek.h"
#include "tlgmem.h"
#include "work.h"
#include "chiron.h"
#include "entry.h"

/*
 *  Default Text Window Structure
 */

Window TxtWinWindow =
{
        1,
	0,0,100,100,
        'R',
        allocTxtWin,
        setTxtWinAttributes,
        makeTxtWin,
        NULL,
        getEditKey,
        T_AltProc,
        T_SwitchProc,
        TxtWinBorderOn,
        TxtWinBorderOff
};

Window *TxtWinObject = &TxtWinWindow;

#define TXTWIN(a)        ((TxtWin *)(*TextHandle))->a

/*--------------------------------------------------------------------
 *  T_AltProc		     08.01.90 MM
 *
 *  Function called when appropriate alt-key combination is typed.
 *-------------------------------------------------------------------
 */
void
T_AltProc( Handle TextHandle )
{
        TxtWinBorderOn( TextHandle );
        setFocus( &(((TxtWin *)(*TextHandle))->window), TextHandle );
}/*
END of T_AltProc
*/

/*--------------------------------------------------------------------
 *  T_SwitchProc                08.01.90 MM
 *
 *  Function called when appropriate alt-key combination is typed.
 *-------------------------------------------------------------------
 */
void
T_SwitchProc( Handle TextHandle )
{
        return;
}/*
END of T_AltProc
*/

/*-------------------------------------------------------------------
 *  allocTxtWin		     08.01.90 MM
 *
 *  Allocates memory for txtwin object and returns Handle.
 *-------------------------------------------------------------------
 */
Handle
allocTxtWin( void )
{
        int i;
        Handle TextHandle;

	TextHandle  = (Handle) TLGmalloc(sizeof(TxtWin *));
	*TextHandle = TLGmalloc( sizeof(TxtWin) );
        TXTWIN(window) = TxtWinWindow;
        TXTWIN(line_start_offset) = 0;

        return(TextHandle);
}/*
END of allocTxtWin
*/

/*-------------------------------------------------------------------
 *  setTxtWinAttributes	     08.01.90 MM
 *
 *  Function to set attributes of text window object.
 *-------------------------------------------------------------------
 */
Window *
setTxtWinAttributes( Handle TextHandle, va_list ap )
{
	Attrib attr;
        char **tb,*tp,*tp2;

        setCoreAttributes( &TXTWIN(window), ap);

	while((attr=va_arg(ap,Attrib)) != END_LIST)
	{
		switch(attr)
		{
                        case T_BUFF:
				tb = va_arg(ap,char **);
                                break;
			case T_COLS:
                                TXTWIN(cols) = va_arg(ap,int);
                                break;
			case T_LINES:
                                TXTWIN(rows) = va_arg(ap,int);
                                break;
                        case T_SEARCHPATT:
                                tp = va_arg(ap,char *);
                                tp2 = TXTWIN(searchpatt);
                                while(*tp)
                                {
                                    *tp2 = toupper(*tp);
                                    tp++;tp2++;
                                }
                                *tp2 = '\0';
                                break;
			default:
				dropArg( &ap, attr );
				break;
		}
	}
	return(&TXTWIN(window));
}/*
END of setTxtWinAttributes
*/
void *
GetTxtWinAttributes( Handle TextHandle, Attrib attr )
{
        switch(attr)
        {
                case T_BUFF:
                        break;
        }
        return(NULL);
}

/*-------------------------------------------------------------------
 *  makeTxtWin		     08.01.90 MM
 *
 *  Puts TextWindow on Screen
 *-------------------------------------------------------------------
 */
void
makeTxtWin( Handle TextHandle )
{
	int i;
        TXTWIN(xpos) = TXTWIN(window.left)+3;
	TXTWIN(ypos) = TXTWIN(window.top);
        TXTWIN(cur_row) = TXTWIN(cur_col) = 0;

	TXTWIN(window.right) = 	TXTWIN(window.left)+
                                TXTWIN(cols)*CHARWIDTH;
	TXTWIN(window.bottom) = TXTWIN(window.top)+
                                TXTWIN(rows)*LINEHEIGHT+1;

        setlinestyle(USERBIT_LINE,0xEEEE,NORM_WIDTH);
	rectangle( TXTWIN(window.left), TXTWIN(window.top),
                   TXTWIN(window.right), TXTWIN(window.bottom));
        setlinestyle(SOLID_LINE,0,NORM_WIDTH);

	setFocus( &(TXTWIN(window)), TextHandle );
}/*
END of makeTxtWin
*/

/*-------------------------------------------------------------------
 *  getEditKey		     08.01.90 MM
 *
 *  The keyproc for Text Window.  Gets the edit key and does something.
 *-------------------------------------------------------------------
 */
void
getEditKey( Handle TextHandle, int key, int mod )
{
        if(Work.is_open)
        {
          switch(key)
          {
        Tcl_ValidateAllMemory(__FILE__,__LINE__);
          case DOWN_ARROW:
		scrollUp( TextHandle, 1);
		break;
          case UP_ARROW:
		scrollDown( TextHandle, 1 );
                break;
          case RIGHT_ARROW:
                if((TXTWIN(line_start_offset)+HORZ_SCROLL_INC)<TXTWIN(cols))
                        TXTWIN(line_start_offset) += HORZ_SCROLL_INC;
                fillScreen( TextHandle );
                break;
          case LEFT_ARROW:
                if( (TXTWIN(line_start_offset) - HORZ_SCROLL_INC ) >= 0 )
                        TXTWIN(line_start_offset) -= HORZ_SCROLL_INC;
                else
                        TXTWIN(line_start_offset) = 0;
                fillScreen( TextHandle );
                break;
          case HOME:
                TXTWIN(line_start_offset) = 0;
                fillScreen(TextHandle);
                break;
          case PG_DOWN:
                scrollUp(TextHandle,5);
                break;
          case PG_UP:
                scrollDown(TextHandle,5);
                break;
          default:
		break;
          }
          /*
          ** This is a ctrl-N dude
          */
          if( ((key & 0xFF00) == 0x3100) && (mod & 0x04) )
          {
                if(NextMatch())
                        displayText( TextHandle );
                else
                {
                        BEEP;
                        makeNotice("No more matches in this work",2,WAIT);
                        noticeDown();
                }
          }

          SetNewCit();
        }
        else
          BEEP;
}/*
END of getEditKey
*/

/*-------------------------------------------------------------------
 *  displayText		     08.01.90 MM
 *
 *  The routine to set up the text for displaying. Shows the first
 *  screen.
 *-------------------------------------------------------------------
 */
void
displayText( Handle TextHandle )
{
        int i;

        SetNewObject( &(TXTWIN(window)), TextHandle );
        TxtWinBorderOn( TextHandle );

        fillScreen( TextHandle );
        SetNewCit();
}/*
END of displayText
*/

/*-------------------------------------------------------------------
 *  fillScreen		     08.01.90 MM
 *
 *  Fills the screen using the text buffer and the line index.
 *-------------------------------------------------------------------
 */
void
fillScreen( Handle TextHandle )
{
        int linenum=0,CurrentLine,FirstLine;
        BOOL work_beg = WorkBegFound();

	clearWindow( &(TXTWIN(window)) );
	TXTWIN(ypos) = TXTWIN(window.top)+4;

        FirstLine = WorkLineIndex();
        if( work_beg && !WorkIsSearchMode() )
                CurrentLine = FirstLine;
        else
                CurrentLine = FirstLine > 5 ? FirstLine-5: 0;

        while( (linenum++ < TXTWIN(rows)) && WorkTextLine(CurrentLine) )
	{
                writeline( CurrentLine++, TextHandle );
	}

        if( work_beg && !WorkIsSearchMode() )
                TXTWIN(cur_row) = 0;
        else
                TXTWIN(cur_row) = FirstLine >5 ? 5 : FirstLine ;

        TXTWIN(ypos) = TXTWIN(window.top)+(TXTWIN(cur_row)+1)*LINEHEIGHT+4;
        HighLightLine( TXTWIN(window), TXTWIN(cur_row) );
}/*
END of fillScreen
*/

/*-------------------------------------------------------------------
 *  writeLine		     08.01.90 MM
 *
 *  Writes one line from the buffer to the TextWindow.  Updates the
 *  buffer and the screenposition.
 *-------------------------------------------------------------------
 */
void
writeline( int index, Handle TextHandle )
{
        int column=0;
        BETALETTER bl;
        char *buffer = WorkTextLine(index);

        SKIPHIGH(buffer);

        if((buffer+TXTWIN(line_start_offset))<WorkCitLine(index+1) )
        {
            buffer+=TXTWIN(line_start_offset);
            TXTWIN(xpos) = TXTWIN(window.left);
            while( !HIBITSET(*buffer) )
            {
               bl = empty_betaletter;
               buffer = GetBetaLetter( buffer, &bl );
               if(column<TXTWIN(cols))
                  TXTWIN(xpos)+=PrintGreekChar(TXTWIN(xpos),TXTWIN(ypos),bl);
               column++;
            }
            SKIPTEXT(buffer);
	}

        TXTWIN(ypos)+=LINEHEIGHT;

}/*
END of writeLine
*/

/*-------------------------------------------------------------------
 *  scrollUp                 08.01.90 MM
 *
 *  Moves the line down one.  The text may move "up".
 *-------------------------------------------------------------------
 */
void
scrollUp( Handle TextHandle, int lines )
{
       int i,delta,lshift;

       if(!WorkMoveLineIndex( lines ))
           return;

       HighLightLine(TXTWIN(window),TXTWIN(cur_row));
       if( (TXTWIN(cur_row)+lines) >= TXTWIN(rows) )
       {
               delta = (TXTWIN(rows)-TXTWIN(cur_row));
               ScrollWindow( TXTWIN(window), -lines );
	       TXTWIN(xpos) = TXTWIN(window.left);
               TXTWIN(ypos) -= (LINEHEIGHT*(lines-delta+1));

               for(i=-(lines-delta);i<delta;i++)
                   writeline(WorkLineIndex()+i,TextHandle);

               TXTWIN(ypos) -= LINEHEIGHT*(delta-1);
       }
       else
       {
               TXTWIN(cur_row)+=lines;
               TXTWIN(ypos) += LINEHEIGHT*lines;
       }
       HighLightLine(TXTWIN(window),TXTWIN(cur_row));
}/*
END of scrollUp
*/

/*-------------------------------------------------------------------
 *  scrollDown   	     08.01.90 MM
 *
 *  Move the line up, so the text moves down.
 *-------------------------------------------------------------------
 */
void
scrollDown( Handle TextHandle, int lines )
{
       int i,delta,lshift;

       if( !WorkMoveLineIndex( -lines ) )
            return;

       HighLightLine(TXTWIN(window),TXTWIN(cur_row));
       if( TXTWIN(cur_row)-lines < 0 )
       {
               delta=(lines-TXTWIN(cur_row));
               ScrollWindow(TXTWIN(window),lines);
	       TXTWIN(xpos) = TXTWIN(window.left);
	       TXTWIN(ypos) = TXTWIN(window.top)+4;

               for(i= -(lines-delta); i<delta ; i++)
                    writeline(WorkLineIndex()+i,TextHandle);

               TXTWIN(ypos) -= (LINEHEIGHT*(delta-1));
       }
       else
       {
               TXTWIN(cur_row)-=lines;
               TXTWIN(ypos) -= LINEHEIGHT*lines;
       }
       HighLightLine(TXTWIN(window),TXTWIN(cur_row));
}/*
END of scrollDown
*/

/*-------------------------------------------------------------------
 *  TxtWinBorderOn           02.09.91 MM
 *
 *  Turns on text window border.
 *-------------------------------------------------------------------
 */
void
TxtWinBorderOn( Handle TextHandle )
{
        setlinestyle(SOLID_LINE,0,NORM_WIDTH);
	rectangle( TXTWIN(window.left), TXTWIN(window.top),
                   TXTWIN(window.right), TXTWIN(window.bottom));
}/*
END of TxtWinBorderOn */


/*-------------------------------------------------------------------
 *  TxtWinBorderOff           02.09.91 MM
 *
 *  Turns on text window border.
 *-------------------------------------------------------------------
 */
void
TxtWinBorderOff( Handle TextHandle )
{
        setwritemode( XOR_PUT );
        setlinestyle(SOLID_LINE,0,NORM_WIDTH);
	rectangle( TXTWIN(window.left), TXTWIN(window.top),
                   TXTWIN(window.right), TXTWIN(window.bottom));
        setwritemode( COPY_PUT );
        setlinestyle(USERBIT_LINE,0xAAAA,NORM_WIDTH);
	rectangle( TXTWIN(window.left), TXTWIN(window.top),
                   TXTWIN(window.right), TXTWIN(window.bottom));
        setlinestyle(SOLID_LINE,0,NORM_WIDTH);
}/*
END of TxtWinBorderOff */


/*-------------------------------------------------------------------
 *  reverseVideo	     08.01.90 MM
 *
 *  Reverses the TextWindow colors.  Not currently used.
 *-------------------------------------------------------------------
 */
void reverseVideo( Handle TextHandle )
{
	void *bitmap;

        /*
	bitmap=getScreenRec(TXTWIN(window.left)+1,TXTWIN(window.top)+1,
			    TXTWIN(window.right)-1,TXTWIN(window.bottom)-1);
	putimage(TXTWIN(window.left)+1,TXTWIN(window.top)+1,bitmap,NOT_PUT);
	TLGfree(bitmap);
        */
}/*
END of reverseVideo
*/
