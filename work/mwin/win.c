/*========================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\win.c 2.0 1993/09/18 16:30:38 mwm Exp mwm $
 *
 *   Main routines for TLG interface
 *
 *   $Log: win.c $
 * Revision 2.0  1993/09/18  16:30:38  mwm
 * Start of new version.
 *
 *
 * Revision 1.4  92/03/12  20:33:17  ROOT_DOS
 * Added FocusOn and FocusOff calls to setfocus
 * 
 * Revision 1.3  92/03/09  18:23:49  ROOT_DOS
 * Added getfocus routine; removed nullProc
 * 
 * Revision 1.2  91/12/01  14:14:09  ROOT_DOS
 * Changed keyproc arguments to add modifiers
 * 
 * Revision 1.1  91/12/01  13:38:54  ROOT_DOS
 * Initial revision
 * 
 *------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <bios.h>
#include "tlg.h"
#include "win.h"
#include "tlgmem.h"
#include "entry.h"
#include "editkey.h"

/*
 *   Window globals
 */
static PFH AltProcs[26];	/*Function pointer for Alt-key callbacks*/
static Handle  AltHandles[26];  /*Handle for object associated with key */
static Window  *AltCores[26];   /*Pointer to core structures            */
static Handle CurrentHandle;    /*Handle for currently active object    */
static Handle CurrentEntryH;    /*Handle for currently focused object   */
static Window *CurrentEntryW;   /*Core for currently focused object     */
static Window *CurrentCore;     /*Core for currently active object      */

static PFV FuncKeyProcs[NUM_FUNC_KEYS];

static Window defWindow ={ 10, 10, 110, 110 };
static int _scancodes[26+NUM_FUNC_KEYS] =
                            { 0x1E00, 0x3000, 0x2E00, 0x2000, 0x1200, 0x2100,
			      0x2200, 0x2300, 0x1700, 0x2400, 0x2500, 0x2600,
			      0x3200, 0x3100, 0x1800, 0x1900, 0x1000, 0x1300,
			      0x1F00, 0x1400, 0x1600, 0x2F00, 0x1100, 0x2D00,
                              0x1500, 0x2C00, F1_KEY, F2_KEY, F3_KEY, F4_KEY,
                              F5_KEY, F6_KEY, F7_KEY, F8_KEY, F9_KEY, F10_KEY
                            };


/*-------------------------------------------------------------------------
 *   initWin                 01.23.90 MM
 *
 *   initializes the BGI and the window system
 *
 *-------------------------------------------------------------------------
 */
void
initWin()
{
        int gd=CGA,gm=CGAHI,i;

        registerbgidriver(CGA_driver);

        initgraph(&gd,&gm,"");
	setfillstyle(EMPTY_FILL,0);
	for(i=0; i<26; i++)
	{
		AltHandles[i] = (Handle) NULL;
		AltCores[i] = (Window *) NULL;
	}
        for(i=0;i<NUM_FUNC_KEYS;i++)
                FuncKeyProcs[i] = NULL;

}/*
END of initWin
*/

/*---------------------------------------------------------------------------
 *   createOb                01.23.90 MM
 *
 *   Does everthing necessary to create an object of the specified type.
 *   Returns a 'handle', that is a pointer to an object pointer.
 *
 *---------------------------------------------------------------------------
 */
Handle
createOb( Handle parent, Window *newOb, ... )
{
	va_list	ap;
	Handle retHandle;

        va_start(ap,newOb);
	retHandle=(*newOb->allocOb)();
        newOb=(*newOb->setAttProc)(retHandle, ap);
        if(newOb->visible)
                (*newOb->makeProc)(retHandle);
	AltProcs[(newOb->AltKey)-'A'] = newOb->AltProc;
	AltCores[(newOb->AltKey)-'A'] = newOb;
	AltHandles[(newOb->AltKey)-'A'] = retHandle;
	CurrentCore = CurrentEntryW = newOb;
	CurrentHandle = CurrentEntryH = retHandle;
	va_end(ap);
	return( retHandle );
}/*
END of createOb
*/


void
SetAttributes( Handle object, ... )
{
	va_list	ap;
        Window twin;

        va_start(ap,object);
        twin=((Entry *)(*object))->window;
        (*twin.setAttProc)(object,ap);
        va_end(ap);
}


/*--------------------------------------------------------------------------
 *  setCoreAttributes        01.23.90 MM
 *
 *  Sets the attributes of the object 'core' -- in this case the window
 *  dimensions.
 *
 *-------------------------------------------------------------------------
 */
void
setCoreAttributes( Window *WPtr, va_list ap )
{
	Attrib attr;

	while((attr=va_arg(ap,Attrib)) != END_LIST)
	{
		switch(attr)
		{
			case W_X:
				WPtr->left = va_arg(ap,int);
				break;
			case W_Y:
				WPtr->top = va_arg(ap,int);
				break;
			case W_WIDTH:
				WPtr->right = va_arg(ap,int)+WPtr->left;
				break;
			case W_HEIGHT:
				WPtr->bottom = va_arg(ap,int)+WPtr->top;
				break;
			default:
				dropArg( &ap, attr );
				break;
		}
	}
	CurrentCore = WPtr;
}/*
END of setCoreAttributes
*/

/*-------------------------------------------------------------------------
 *  keyLoop                  01.23.90 MM
 *
 *  The main 'event loop'.  Now we just have key events.  If an Alt-key the
 *  appropriate action is taken, otherwise the key is sent to the currently
 *  focused object.
 *
 *-------------------------------------------------------------------------
 */
void
keyLoop()
{
	int key,mod,index;

	while(1)
	{
		while( bioskey(1) == 0);

		key = bioskey(0);
		mod = bioskey(2);

		if ( mod & 0x0008 )
		{
			index = letterOfScancode( key );
			if( AltCores[index] != NULL )
			{
                                SetNewObject( AltCores[index],
                                             AltHandles[index] );
			}
		}
                else if ( (key >= F1_KEY) && (key <= F10_KEY) )
                {
			index = letterOfScancode( key );
                        if( FuncKeyProcs[index] )
                                (*FuncKeyProcs[index])();
                }
		else
		{
                        if(CurrentEntryW->keyproc)
                            (*CurrentEntryW->keyproc)( CurrentEntryH, key, mod);
		}
	}
}/*
END of keyLoop
*/


/*-------------------------------------------------------------------------
 *  SetNewObject             03.28.92 MM
 *
 *  Set a new object to current;
 *-------------------------------------------------------------------------
 */
void
SetNewObject( Window *new_core, Handle new_handle )
{
        if(CurrentCore->SwitchProc)
                (*CurrentCore->SwitchProc)(CurrentHandle);
        CurrentCore = new_core;
        CurrentHandle = new_handle;
        if(CurrentCore->AltProc)
                (*CurrentCore->AltProc)(CurrentHandle);
}/*
END of SetNewObject */

/*-------------------------------------------------------------------
 *  getScreenRec                06.09.91 Mm
 *
 *  Retrieves a area of frame buffer
 *-------------------------------------------------------------------
 */
void *
getScreenRec( int left, int top, int right, int bottom )
{
	static void *buffer;

        getimage(left,top,right,bottom,
		 buffer = TLGmalloc( imagesize(left,top,right,bottom)) );

	return( buffer );
}/*
END of getScreenRec */


/*----------------------------------------------------------------------------
 *  ScrollWindow                06.09.91 MM
 *
 *  Scroll the window by the indicated number of lines (+ = down, -= up)
 *----------------------------------------------------------------------------
 */
 void ScrollWindow( Window window, int lines )
 {

        void *screenbuffer=
        getScreenRec(window.left+1,
                     window.top+1+((lines<0) ? (lines*LINEHEIGHT*-1) : 0),
                     window.right-1,
                     window.bottom-1-((lines>0) ? (lines*LINEHEIGHT) : 0) );

        clearWindow( &window );

        putimage(window.left+1,
                 window.top+1+((lines>0) ? LINEHEIGHT*lines : 0),
                 screenbuffer,XOR_PUT);

        TLGfree(screenbuffer);
}/*
END of ScrollWindow */


/*-------------------------------------------------------------------
 *  HighLightLine                08.01.90 MM
 *
 *  Highlights the current text line
 *-------------------------------------------------------------------
 */
void
HighLightLine( Window window, int line )
{
        void *bitmap=getScreenRec(window.left+1,
                            window.top+LINEHEIGHT*line+1,
                            window.right-1,
                            window.top+LINEHEIGHT*(line+1) );

        putimage(window.left+1,window.top+LINEHEIGHT*line+1,
		 bitmap, NOT_PUT );

	TLGfree(bitmap);
}/*
END of HighLightLine
*/


/*----------------------------------------------------------------------------
 *   clearWindow                01.23.90 MM
 *
 *   Clears the specified window.
 *
 *----------------------------------------------------------------------------
 */
void
clearWindow( Window *window )
{
	bar(window->left+1,window->top+1,window->right-1,window->bottom-1);
}/*
END of clearWindow
*/

/*----------------------------------------------------------------------------
 *   clearArea                01.23.90 MM
 *
 *   Clears the specified area.
 *
 *----------------------------------------------------------------------------
 */
void clearArea( int left, int top, int right, int bottom )
{
	bar(left,top,right,bottom);
}/*
END of clearArea
*/

/*----------------------------------------------------------------------------
 *   setFocus                01.23.90 MM
 *
 *   Sets the 'focus' to the given object.  This means it gets key events.
 *
 *----------------------------------------------------------------------------
 */
void
setFocus( Window *WPtr, Handle hnd )
{
        if(CurrentEntryW->FocusOffProc)
                (*CurrentEntryW->FocusOffProc)(CurrentEntryH);
	CurrentEntryW = WPtr;
	CurrentEntryH = hnd;
        if(CurrentEntryW->FocusOnProc)
                (*CurrentEntryW->FocusOnProc)(CurrentEntryH);
}/*
END of setFocus
*/
/*----------------------------------------------------------------------------
 *   getFocus                03.04.92 MM
 *
 *   Return the window * and handle of the current focus
 *----------------------------------------------------------------------------
 */
void
getFocus( Window **WPtr, Handle *hnd )
{
        *WPtr = CurrentEntryW;
        *hnd = CurrentEntryH;
}/*
END of getFocus
*/

/*---------------------------------------------------------------------------
 *   dropArg                 01.23.90 MM
 *
 *   Updates the va_list by dropping the arg.  Used in attribute set
 *   functions.  May go away soon.
 *
 *---------------------------------------------------------------------------
 */
void dropArg( va_list *app, Attrib att )
{
        char *cp;
        int size = ATTSIZE(att);

        if (size>100)
        {
                cp = (char *) (*app);
                while(*cp++);
                *app = (va_list *) cp;
        }
        else
                (char *)(*app) += size;
}/*
END of dropArg
*/

/*--------------------------------------------------------------------------
 *   letterOfScancode        01.23.90 MM
 *
 *   Returns the letter corresponding to the given scancode.
 *
 *--------------------------------------------------------------------------
 */
int
letterOfScancode( int key )
{
	int i;

        for(i=0;i<(26+NUM_FUNC_KEYS);i++)
	{
		if ( _scancodes[i] == key )
                        return( (i < 26) ? i : (i-26) );
	}
	return(0);
}/*
END of letterOfScancode
*/



/*-------------------------------------------------------------------
 *  BindFuncKey              03.12.92 MM
 *
 *  Binds function key to a procedure
 *-------------------------------------------------------------------
 */
void
BindFuncKey( int FKey, PFV function )
{
        FuncKeyProcs[FKey] = function;
}/*
END of BindFuncKey */


