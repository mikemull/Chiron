/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\button.c 2.0 1993/09/18 16:31:09 mwm Exp mwm $
 *
 *  Contains the code for the BUTTON object
 *
 *  $Log: button.c $
 * Revision 2.0  1993/09/18  16:31:09  mwm
 * Start of new version.
 *
 * Revision 1.4  92/03/19  20:08:28  ROOT_DOS
 * Safety check
 * 
 * Revision 1.3  92/03/09  20:23:28  ROOT_DOS
 * Added focuson and focusoff procs
 * 
 * Revision 1.2  92/03/09  19:02:16  ROOT_DOS
 * Added switchproc to take down list.
 * 
 * Revision 1.1  91/12/01  13:35:55  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <math.h>
#include "tlg.h"
#include "win.h"
#include "button.h"
#include "notice.h"
#include "tlgmem.h"

/*
 *  Default Button Core
 */
Window ButtonWindow =
{
        TRUE,
	0,0,100,100,
	'0',
        allocButton,
	setButtonAttributes,
	makeButton,
        NULL,
        NULL,
	B_AltProc,
        B_SwitchProc,
        NULL,
        NULL
};

Window *ButtonObject = &ButtonWindow;

static Button *CurrentButton;
static unsigned char ButtonIsOn=0;

#define BUTTON(a) (((Button *)(*Bhnd))->a)

/*-------------------------------------------------------------------
 *  B_AltProc                04.17.90
 *
 *  Function called when alt-key combination is typed for a button
 *  object.
 *-------------------------------------------------------------------
 */
void
B_AltProc( Handle Bhnd )
{
	long int i;

	buttonOn( (Button *)*Bhnd, NOSAVE );
        if(BUTTON(ActProc) != NULL)
                (*BUTTON(ActProc))();
        if(BUTTON(list->list)!=NULL)
                ButtonMakeList( Bhnd );
	buttonOn( (Button *)*Bhnd, NOSAVE );
}/*
END of B_AltProc
*/

/*-------------------------------------------------------------------
 *  B_SwitchProc             06.25.90 MM
 *
 *  Function called when this object is left for the next object.
 *-------------------------------------------------------------------
 */
void
B_SwitchProc( Handle Bhnd )
{
        listDown( (Handle) &BUTTON(list) );
}/*
END of E_SwitchProc
*/


/*-------------------------------------------------------------------
 *  allocButton              04.17.90  MM
 *
 *  Function that allocates the memory for the object and returns
 *  a handle.
 *-------------------------------------------------------------------
 */
Handle
allocButton( void )
{
	Handle Bhnd;
        Handle lt;

	Bhnd  = (Handle) TLGmalloc(sizeof(Button *));
	*Bhnd = TLGmalloc( sizeof(Button) );
        BUTTON(window) = ButtonWindow;
	lt = allocList();
        BUTTON(list) = (List *)(*lt);
        BUTTON(list->list_parent_handle) = Bhnd;
        BUTTON(list->list_parent_core) = &BUTTON(window);
        return(Bhnd);
}/*
END of allocButton
*/


/*-------------------------------------------------------------------
 *  setButtonAttributes      6.25.90
 *
 *  Sets default and specified attributes.
 *-------------------------------------------------------------------
 */
Window *
setButtonAttributes( Handle Bhnd, va_list ap )
{
	Attrib attr;

	setCoreAttributes( &BUTTON(window), ap );
        (void) setListAttributes( (Handle) &BUTTON(list), ap );

	while((attr=va_arg(ap,Attrib)) != END_LIST)
	{
		switch(attr)
		{
			case B_LABEL:
				BUTTON(label) = va_arg(ap,char *);
				break;
			case B_KEY:
				BUTTON(window.AltKey) = va_arg(ap,int);
				break;
			case B_FUNC:
				BUTTON(ActProc) = va_arg(ap,PFI);
				break;
			default:
				dropArg( &ap, attr );
				break;
		}
	}

        BUTTON(window.right) = BUTTON(window.left)+
                               strlen(BUTTON(label))*8+4;
                               /* Magic number 8 is used here
                                  since we're using outtext
                                  and Borland fonts are only
                                  8 pixels per letter. */
	BUTTON(window.bottom) = BUTTON(window.top)+12;

	return(&BUTTON(window));
}/*
END of setButtonAttributes
*/


/*-------------------------------------------------------------------
 *  makeButton               06.25.90 MM
 *
 *  Puts button on graphics screen.
 *-------------------------------------------------------------------
 */
void
makeButton( Handle Bhnd )
{
	rectangle( BUTTON(window.left), BUTTON(window.top),
		   BUTTON(window.right), BUTTON(window.bottom) );

        outtextxy( BUTTON(window.left)+2, BUTTON(window.top)+3,
		   BUTTON(label) );
}/*
END of makeButton
*/


void
ButtonMakeList( Handle Bhnd )
{
        int i,a,maxlen=0;

        for (i=0;i<BUTTON(list)->ListSize;i++)
        {
                /* maxlen = max(maxlen,strlen(BUTTON(list)->list[i]));
                */
                a = strlen(BUTTON(list)->list[i]);
                maxlen = (maxlen > a) ? maxlen : a ;
        }

        BUTTON(list->window.left)= BUTTON(window.left);
        BUTTON(list->window.top)= BUTTON(window.bottom)+1;
        BUTTON(list->window.right)= BUTTON(window.left)+maxlen*CHARWIDTH;
        BUTTON(list->window.bottom)= BUTTON(window.bottom)+
                                        LINEHEIGHT*BUTTON(list->ListSize)+1;

        makeList( (Handle) &BUTTON(list) );
}/*
END of BUTTONMakeList
*/
/*-------------------------------------------------------------------
 *  buttonOn                 06.25.90
 *
 *  Strange function to highlight button based upon save argument.
 *-------------------------------------------------------------------
 */
#define W(a)	BPtr->window.a
void buttonOn( Button *BPtr, ButtonStat save )
{
	void far *bitmap;

	bitmap=getScreenRec( W(left), W(top), W(right), W(bottom) );
	putimage( W(left), W(top), bitmap, NOT_PUT );
	TLGfree( bitmap );
	if(save==SAVE)
	{
		CurrentButton = BPtr;
		ButtonIsOn = 1;
	}
	else if (save==UNSAVE)
		ButtonIsOn = 0;

}/*
END of buttonOn
*/


/*-------------------------------------------------------------------
 *  currentButton            6.25.90 MM
 *
 *  Returns pointer to currently active button.
 *-------------------------------------------------------------------
 */
Button *
currentButton( void )
{
	return(CurrentButton);
}/*
END of currentButton
*/


/*-------------------------------------------------------------------
 *  buttonIsOn               06.25.90 MM
 *
 *  Function that checks to see if the button is highlighted. Stupid.
 *-------------------------------------------------------------------
 */
unsigned char
buttonIsOn( void )
{
	return( ButtonIsOn );
}/*
END of buttonIsOn
*/

