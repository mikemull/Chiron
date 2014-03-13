/*========================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\notice.c 2.0 1993/09/18 16:34:32 mwm Exp mwm $
 *
 *   Notice window code.  Pops up information messages.
 *
 *   $Log: notice.c $
 * Revision 2.0  1993/09/18  16:34:32  mwm
 * Start of new version.
 *
 * 
 *------------------------------------------------------------------------
 */


#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "tlg.h"
#include "win.h"
#include "notice.h"
#include "tlgmem.h"

static void *ScreenBuffer;
static int x,y,NWidth,NHeight;
static int linenum=1;

unsigned char
makeNotice( char *Notice, int lines, int wait )
{
	int c;

        if(ScreenBuffer) noticeDown();
	if (wait) lines++;
	NWidth = (strlen(Notice)*8+20)/2;
	NHeight = (lines*12+10)/2;

	ScreenBuffer = getScreenRec( 320 - NWidth, 100-NHeight,
				     320 + NWidth, 100+NHeight );

	clearArea( 320-NWidth, 100-NHeight, 320 + NWidth, 100+NHeight );
	rectangle( 320-NWidth, 100-NHeight, 320+NWidth, 100+NHeight );
	outtextxy( 320-strlen(Notice)*8/2,100-NHeight+linenum*12, Notice );
	x = 320-NWidth;
	y = 100-NHeight;
        if(wait==WAIT)
	{
		addToNotice( "Type any key to continue" );
                WaitForKey();
	}
        else if( wait == YORN )
        {
                addToNotice( "Press Y for yes, N for no" );
		c=getch();
                if( (c == 'y') || (c == 'Y') )
                        return(TRUE);
		if(c==0) getch();
        }
        return(FALSE);
}

void
noticeDown( void )
{
        if(ScreenBuffer) putimage(x,y,ScreenBuffer,COPY_PUT);
	TLGfree( ScreenBuffer );
        ScreenBuffer = NULL;
	linenum = 1;
}

void
switchNotice( char *notice )
{
	noticeDown();
	makeNotice(notice,1,NOWAIT);
}

void
addToNotice( char *notice )
{
	linenum++;
	outtextxy( 320-strlen(notice)*8/2,100-NHeight+linenum*12, notice );
}

void
WaitForKey( void )
{
        int c;

        c=getch();
        if(c==0) getch();
}

