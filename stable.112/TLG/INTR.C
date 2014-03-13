/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\intr.c 2.0 1993/09/18 16:15:31 mwm Exp mwm $
 *
 *   $Log: intr.c $
 * Revision 2.0  1993/09/18  16:15:31  mwm
 * Start of new version
 *
 *===================================================================
 */

#include <stdio.h>
#include <dos.h>
#include <graphics.h>
#include "tlg.h"
#include "editkey.h"
#include "intr.h"

/* MACROS */

#define CLOCK_INTR    0x1C
#define TIC_CNT       18
#define C_BRK_INTR    0x1B
#define ICON_XPOS     400
#define ICON_YPOS     3

/* STATICS */

static BOOL timer;
static int  nicon=0;

static timer_icon[2][14] = {
{
0xf   ,0x8   ,0x0   ,0xf03f,0x6018,0xc00c,0x8007,0xc00f,
0xe01f,0xf03f,0x0   ,0x2a2e,0xfe16,0x8acd,},
{
0xf   ,0x8   ,0x0   ,0xf03f,0xe01f,0xc00f,0x8007,0xc00c,
0x6018,0xf03f,0x0   ,0x2c16,0xfe16,0x55ca,}
};


/* PRIVATE PROTOTYPES */

void interrupt ClockHandler();
void interrupt (*old_clock)( void );

/*-------------------------------------------------------------------
 *  ClockToggle
 *
 *  Toggles the installed Clock handler on or off
 *-------------------------------------------------------------------
 */
void
ClockToggle( BOOL val )
{
    if( val )
       timer = TRUE;
    else
    {
       nicon = (nicon == 0) ? 1 : 0 ;
       putimage( ICON_XPOS, ICON_YPOS, timer_icon[nicon], XOR_PUT );
       timer = FALSE;
    }
}

/*-------------------------------------------------------------------
 *  ClockSetup
 *
 *  Installs the custom Clock handler
 *-------------------------------------------------------------------
 */
void
ClockSetup()
{
    old_clock = getvect(CLOCK_INTR);
    setvect(CLOCK_INTR,ClockHandler);
}

/*-------------------------------------------------------------------
 *  ClockShutdown
 *
 *  Un-Installs the custom Clock handler
 *-------------------------------------------------------------------
 */
void
ClockShutdown()
{
    setvect(CLOCK_INTR,old_clock);
}

/*-------------------------------------------------------------------
 *  ClockHandler
 *
 *  The custom Clock handler
 *-------------------------------------------------------------------
 */
void interrupt
ClockHandler()
{
    static int tic;

    tic = ( tic > TIC_CNT ) ? 0 : tic+1;
    if( timer )
    {
       if( !tic )
       {
        /* visual feed back goes here */
        putimage(ICON_XPOS,ICON_YPOS,timer_icon[nicon], COPY_PUT);
        nicon = (nicon == 0) ? 1 : 0 ;

       }
    }
    old_clock();
}


/*-------------------------------------------------------------------
 *  CheckForEscape
 *
 *  Returns true if an ESCAPE has been typed
 *-------------------------------------------------------------------
 */
BOOL
CheckForEscape( void )
{
        int key;
        BOOL stat = FALSE;

        while(bioskey(1))
        {
                key = bioskey(0);
                if(key == ESCAPE)
                {
                        stat = TRUE;
                        break;
                }
        }

        return(stat);
}
                        

