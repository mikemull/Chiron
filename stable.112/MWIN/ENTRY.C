/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\entry.c 2.0 1993/09/18 16:35:50 mwm Exp mwm $
 *
 *  Code for the Entry object.
 *
 *  $Log: entry.c $
 * Revision 2.0  1993/09/18  16:35:50  mwm
 * Start of new version.
 *
 * Revision 1.12  92/06/01  18:26:31  ROOT_DOS
 * Added stuff for non-fixed entries and triggers
 * 
 * Revision 1.11  92/06/01  17:55:27  ROOT_DOS
 * Added SetNewObject on displayText
 * 
 * Revision 1.10  92/04/12  18:04:54  ROOT_DOS
 * Don't know
 * 
 * Revision 1.9  92/04/05  15:59:54  ROOT_DOS
 * Changed SetValue to work on non-visible entries
 * Fixed entry switching
 * 
 * Revision 1.8  92/04/05  10:55:41  ROOT_DOS
 * Fixed left/right scrolling.
 * 
 * Revision 1.7  92/03/29  13:40:30  ROOT_DOS
 * Added code for non-fixed (pop-up) entries.
 * 
 * Revision 1.6  92/03/19  19:47:24  ROOT_DOS
 * Safety check
 * 
 * Revision 1.5  92/03/09  20:23:55  ROOT_DOS
 * Added focuson and focusoff procs
 * 
 * Revision 1.4  92/03/09  19:00:52  ROOT_DOS
 * Added numeric entry
 * 
 * Revision 1.3  91/12/01  14:14:52  ROOT_DOS
 * Changed keyproc arguments to add modifiers
 * 
 * Revision 1.2  91/09/05  18:09:45  ROOT_DOS
 * Added code to handle BETALETTER attributes.
 * 
 * Revision 1.1  91/09/04  18:40:01  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dos.h>
#include "tlg.h"
#include "win.h"
#include "button.h"
#include "list.h"
#include "entry.h"
#include "notice.h"
#include "editkey.h"
#include "tlgmem.h"
#include "inxfile.h"

/*
 *  Default Entry object
 */
Window EntryWindow =
{
        1,
	0,0,100,100,
        '0',
        allocEntry,
        setEntryAttributes,
        makeEntry,
        NULL,
        doText,
        E_AltProc,
        E_SwitchProc,
        EntryFocusOn,
        EntryFocusOff
};

Window *EntryObject = &EntryWindow;

/*
    Entry State statics and cursor shape
 */
static Window *save_focus_win=NULL;
static Handle save_focus_handle=NULL;
static Handle CurrentEntry=NULL;
static Handle CurrentSelect=NULL;
static char _cursor[2][14] = {
{ 0x7   ,0x0   ,0x7   ,0x0   ,0x0   ,0x0   ,0x0   ,0x0   ,
  0x0   ,0x0   ,0x18  ,0x66  ,0xffb3,0xffb3,},
{ 0x7   ,0x0   ,0x7   ,0x0   ,0x0   ,0x1   ,0x7   ,0x1f  ,
 0x3f  ,0x1f  ,0x7   ,0x1   ,0xffb3,0xffb3,}
};

Entry *d_entry;

#define ENTRY(a)        ((Entry *)(*TextHandle))->a
/*-------------------------------------------------------------------
 *  E_AltProc                06.25.90 MM
 *
 *  Function that is called when appropriate alt-key combination
 *  is pressed.
 *-------------------------------------------------------------------
 */
void
E_AltProc( Handle TextHandle )
{
   /*	(* (((Entry *)(*Ehnd))->button.ActProc) )(); */
        if( !ENTRY(fixed) && !ENTRY(window.visible) )
        {
                ENTRY(window.visible) = TRUE;
                makeEntry( TextHandle );
        }
        getFocus( &save_focus_win, &save_focus_handle );
	setFocus( &ENTRY(window), TextHandle );
	if( CurrentSelect != NULL )
		listDown( (Handle) &(((Entry *)
			  (*CurrentSelect))->list) );

        d_entry = (Entry *) (*TextHandle);
}/*
END of E_AltProc
*/

/*-------------------------------------------------------------------
 *  E_SwitchProc             06.25.90 MM
 *
 *  Function called when this object is left for the next object.
 *-------------------------------------------------------------------
 */
void
E_SwitchProc( Handle TextHandle )
{
        if(!ENTRY(fixed))
        {
                EntryDown( TextHandle );
                /*
                **  Set the focus back so that the new thing
                **  wont set its focus back to a non-fixed entry
                */
                setFocus( save_focus_win, save_focus_handle );
        }
        else
                listDown( (Handle) &ENTRY(list) );
}/*
END of E_SwitchProc
*/


/*-------------------------------------------------------------------
 *  EntryFocusOn             03.25.92 MM
 *
 *  Function called when focus is set to the given entry
 *-------------------------------------------------------------------
 */
void
EntryFocusOn( Handle TextHandle )
{
        if(ENTRY(window.visible))
                buttonOn( ENTRY(button), SAVE);
}/*
END of EntryFocusOn */



/*-------------------------------------------------------------------
 *  EntryFocusOff            03.25.92 MM
 *
 *  Function called when this object is left for the next object.
 *-------------------------------------------------------------------
 */
void
EntryFocusOff( Handle TextHandle )
{
        if(ENTRY(window.visible))
                buttonOn( ENTRY(button), SAVE );
}/*
END of EntryFocusOff */



/*-------------------------------------------------------------------
 *  EntrySelect              06.25.90
 *
 *  Sets the current entry to the specified object
 *-------------------------------------------------------------------
 */
void
EntrySelect( Handle TextHandle )
{
        SetNewObject( &ENTRY(window), TextHandle );

        /*
        ** Cheesy debugging thang for TD
        */
        d_entry = ((Entry *)(*TextHandle));
}/*
END of EntrySelect
*/


/*-------------------------------------------------------------------
 *  EntryDown           03.25.92
 *
 *  Removed an unfixed entry from the screen
 *-------------------------------------------------------------------
 */
void
EntryDown( Handle TextHandle )
{
        putimage(ENTRY(window.left),ENTRY(window.top),
                 ENTRY(back_save), COPY_PUT );
        TLGfree(ENTRY(back_save));
        ENTRY(window.visible) = FALSE;
        if(ENTRY(trigger))
                (*ENTRY(trigger))();
}/*
END of EntryDown */


/*-------------------------------------------------------------------
 *  allocEntry               06.25.90 MM
 *
 *  Allocates an entry object and returns a Handle
 *-------------------------------------------------------------------
 */
Handle
allocEntry( void )
{
        Handle TextHandle;
	Handle bt,lt;
        int i;

	TextHandle  = (Handle) TLGmalloc(sizeof(Entry *));
	*TextHandle = TLGmalloc( sizeof(Entry) );
        ENTRY(window) = EntryWindow;
	bt = allocButton();
	ENTRY(button) = (Button *)(*bt);
	lt = allocList();
	ENTRY(list) = (List *)(*lt);
	ENTRY(list->list_parent_handle) = TextHandle;
	ENTRY(list->list_parent_core) = &ENTRY(window);
        ENTRY(buffpos) = ENTRY(curpos) = ENTRY(frame_start) = 0;
        ENTRY(cursor_pos) = 0;
        ENTRY(fixed) = TRUE;
        ENTRY(trigger) = NULL;

        for(i=0;i<ENTRY_MAX_BUFFER;i++)
                ENTRY(buffer)[i] = '\0';

        for(i=0;i<ENTRY_MAX_WIDTH;i++)
                ENTRY(btext)[i] = empty_betaletter;

        return( TextHandle );
}/*
END of allocEntry
*/

/*-------------------------------------------------------------------
 *  setEntryAttributes       06.25.90 MM
 *
 *  Sets default and specified entry attributes.
 *-------------------------------------------------------------------
 */
Window *
setEntryAttributes( Handle TextHandle, va_list ap )
{
	Attrib attr;

	setCoreAttributes( &ENTRY(window), ap);
        (void) setListAttributes( (Handle) &ENTRY(list), ap );
        (void) setButtonAttributes( (Handle) &ENTRY(button) , ap );

	while((attr=va_arg(ap,Attrib)) != END_LIST)
	{
		switch(attr)
		{
                        case E_WIDTH:
                                ENTRY(width) = va_arg(ap,int);
				break;
			case E_TYPE:
                                ENTRY(txttype) = va_arg(ap,int);
				break;
			case B_KEY:
				ENTRY(window.AltKey)= va_arg(ap,int);
				break;
                        case E_VISIBLE:
                                ENTRY(window.visible) = va_arg(ap,BOOL);
                                if(ENTRY(window.visible))
                                {
                                      makeEntry(TextHandle);
                                      SetNewObject(&ENTRY(window),TextHandle);
                                      setFocus(&ENTRY(window),TextHandle);
                                }
                                break;
                        case E_FIXED:
                                ENTRY(fixed) = va_arg(ap,BOOL);
                                break;
                        case E_TRIGGER:
                                ENTRY(trigger) = va_arg(ap,PFV);
                                break;
			default:
				dropArg( &ap, attr );
				break;
		}
	}
        ENTRY(button->window.top) = ENTRY(window.top)+2;
        ENTRY(button->window.left) = ENTRY(window.left)+2;
        ENTRY(button->window.right) = ENTRY(button->window.left)+
                                       strlen(ENTRY(button->label))*8+4;
                               /* Magic number 8 is used here
                                  since we're using outtext
                                  and Borland fonts are only
                                  8 pixels per letter. */
        ENTRY(button->window.bottom) = ENTRY(button->window.top)+12;
        ENTRY(window.right)=ENTRY(button->window.right)+CHARWIDTH*ENTRY(width)
                            +2;
        ENTRY(window.bottom)=ENTRY(window.top) + LINEHEIGHT+3;

	return(&ENTRY(window));
}/*
END of setEntryAttributes
*/

/*-------------------------------------------------------------------
 *  makeEntry                06.25.90
 *
 *  Draws the entry object.
 *-------------------------------------------------------------------
 */
void
makeEntry( Handle TextHandle )
{
        char temp_value[ENTRY_MAX_BUFFER];


        if(!ENTRY(fixed))
        {
                ENTRY(back_save) = getScreenRec(ENTRY(window.left),
                                                ENTRY(window.top),
                                                ENTRY(window.right),
                                                ENTRY(window.bottom));
        }

        clearArea(ENTRY(window.left),ENTRY(window.top),
                  ENTRY(window.right),ENTRY(window.bottom));

	makeButton( (Handle) &ENTRY(button) );


        ENTRY(xpos) = ENTRY(button->window.right)+2;
	ENTRY(ypos) = ENTRY(window.top)+5;
	rectangle( ENTRY(window.left), ENTRY(window.top),
		   ENTRY(window.right), ENTRY(window.bottom));
        EntryCursorOn( TextHandle );

        if(ENTRY(curpos) != 0)
        {
                strncpy(temp_value,ENTRY(buffer),ENTRY_MAX_BUFFER-1);
                EntrySetValue(TextHandle, temp_value );
        }
}/*
END of makeEntry
*/

/*-------------------------------------------------------------------
 *  doText                   06.25.90 MM
 *
 *  Keystroke handler for Entry object.  Gets characters and
 *  either prints them or does the associated editing action.
 *-------------------------------------------------------------------
 */
void
doText( Handle TextHandle, int letter, int mod )
{
        int diff;

        if ( !(letter & 0x00FF) ) /* edit key because no ascii value */
                EntryEditKey(TextHandle,letter);

        else if ( letter == CARRIAGE_RETURN )
	{
                if( !ENTRY(fixed) )
                        SetNewObject(save_focus_win,save_focus_handle);
                else if( ENTRY(list->ListSize)>0 )
                        EntryMakeList( TextHandle );
	}

	else if( letter == BACKSPACE )
		deleteLetter( TextHandle );

        else if( letter == ESCAPE )
                EntrySetValue(TextHandle,"");

        else if( isprint(letter & 0xFF) )
	{
                diff = ENTRY(curpos)-ENTRY(cursor_pos)-ENTRY(frame_start);
                while(diff--)
                        ShiftLeft(TextHandle);

		letter = letter & 0xFF;
                if( ENTRY(txttype) == GREEK )
                        addGreekLetter( TextHandle, letter );
                else if ( ENTRY(txttype) == LATIN )
                        addLatinLetter( TextHandle, letter );
                else /*NUMERIC*/
                        addNumber( TextHandle , letter );
	}
        else
                BEEP;
}/*
END of doText
*/


/*-------------------------------------------------------------------
 *  EntryEditKey             05.25.91 MM
 *
 *  Handle cursor keys
 *-------------------------------------------------------------------
 */
void
EntryEditKey(Handle TextHandle, int key)
{
        int diff=0;

        switch(key)
        {
                case LEFT_ARROW:
                ShiftRight(TextHandle);
                break;

                case RIGHT_ARROW:
                ShiftLeft(TextHandle);
                break;

                case HOME:
                diff = ENTRY(cursor_pos)+ENTRY(frame_start);
                while(diff--)
                        ShiftRight(TextHandle);
                break;

                case END:
                diff = ENTRY(curpos)-ENTRY(cursor_pos) ;
                while( diff-- )
                        ShiftLeft(TextHandle);
                break;
        }
}/*
END of EntryEditKey */


/*-------------------------------------------------------------------
 *  EntryGetValue                 06.25.90 MM
 *
 *  Returs the entry field value.
 *-------------------------------------------------------------------
 */
void
EntryGetValue( Handle TextHandle, void *value )
{
        strcpy((char *)value,ENTRY(buffer));
}/*
END of EntryGetValue
*/



/*-------------------------------------------------------------------
 *  EntrySetValue                 06.25.90 MM
 *
 *  Sets the entry field to the specified string.
 *-------------------------------------------------------------------
 */
void
EntrySetValue( Handle TextHandle, char *String )
{
        int i,diff;
        char new_entry[ENTRY_MAX_BUFFER];

        /*
        ** Get rid of leading and following spaces
        */
        while( isspace( *String ) )
                String++;
        strncpy(new_entry,String,ENTRY_MAX_BUFFER-1);
        for(i=strlen(new_entry)-1; isspace(new_entry[i]); i--)
                ; /* trim terminal blanks */
        new_entry[i+1] = '\0';

        while(ENTRY(curpos))
                ENTRY(btext[--ENTRY(curpos)]) =empty_betaletter;

        if(ENTRY(window.visible))
                clearArea( ENTRY(button->window.right)+2, ENTRY(window.top)+1,
                           ENTRY(window.right)-1, ENTRY(window.bottom)-1 );

        ENTRY(curpos) = 0;
        ENTRY(buffpos) =0;
        ENTRY(cursor_pos) =0;
        ENTRY(frame_start) = 0;
        ENTRY(xpos) = ENTRY(button->window.right)+2;
	ENTRY(ypos) = ENTRY(window.top)+5;

        while( ENTRY(buffpos)<strlen(new_entry) )
	{
                switch(ENTRY(txttype))
                {
                        case GREEK:
                                addGreekLetter(TextHandle,
                                               *(String+ENTRY(buffpos)) );
                                break;
                        case LATIN:
                                addLatinLetter(TextHandle,
                                               *(String+ENTRY(buffpos)) );
                                break;
                        case NUMERIC:
                                addNumber(TextHandle,
                                               *(String+ENTRY(buffpos)) );
                                break;
                }
        }
        diff = ENTRY(cursor_pos)+ENTRY(frame_start);
        while(diff--)
            ShiftRight(TextHandle);

        EntryCursorOn(TextHandle);
}/*
END of EntrySetValue
*/



/*-------------------------------------------------------------------
 *  EntryPrintChar           06.17.92 MM
 *
 *  Prints a character at the current cursor position
 *-------------------------------------------------------------------
*/
void
EntryPrintChar( Handle TextHandle, BETALETTER letter )
{
    if(ENTRY(window.visible))
    {
        PrintGreekChar(
                  ENTRY(button->window.right)+CHARWIDTH*ENTRY(cursor_pos)+2,
                  ENTRY(window.top)+5,
                  letter
                  );
        ENTRY(cursor_pos)++;
    }
}/*
END of EntryPrintChar */



/*-------------------------------------------------------------------
 *  EntryMakeList            06.25.90 MM
 *
 *  Draws list associated with entry (if any).
 *-------------------------------------------------------------------
 */
void
EntryMakeList( Handle TextHandle )
{
        EntrySelect( TextHandle );

	ENTRY(list->window.left)= ENTRY(window.left);
	ENTRY(list->window.top)= ENTRY(window.bottom)+1;
	ENTRY(list->window.right)= ENTRY(window.right);
        ENTRY(list->window.bottom)=ENTRY(window.bottom)+
                                   LINEHEIGHT*(ENTRY(list->ListSize)>=10 ? 10 :
                                                  ENTRY(list->ListSize))+1;

        makeList( (Handle) &ENTRY(list) );
}/*
END of EntryMakeList
*/

/*-------------------------------------------------------------------
 *  EntrySetList             06.25.90 MM
 *
 *  Sets the Entry list to tthe specified list of strings.
 *-------------------------------------------------------------------
 */
void
EntrySetList( Handle TextHandle, char **sList, int ListSize )
{
	setList( (Handle) &ENTRY(list), sList, ListSize );
	CurrentSelect = TextHandle;
}/*
END of EntrySetList
*/

/*-------------------------------------------------------------------
 *  addGreekLetter           06.25.90 MM
 *
 *  Add a greek letter to the entry.
 *-------------------------------------------------------------------
 */
void
addGreekLetter( Handle TextHandle, int letter )
{
        BETALETTER *bp;

        if ( DiaVal(letter) && (ENTRY(buffpos) == 0) )
        {
               BEEP;
               return;
        }

        if(ISOPER(letter) || ISESCAPE(letter) )
	{
		addLatinLetter(TextHandle, letter);
		return;
	}
	else if(ISSIGMA(letter))
		ENTRY(buffer)[ENTRY(buffpos)] = 'S';
	else
		ENTRY(buffer)[ENTRY(buffpos)]=(char) letter;

        ENTRY(buffer)[++ENTRY(buffpos)]=(char) '\0';
        EntryCursorOff( TextHandle );

        if(!DiaVal(letter))
        {
                if( ENTRY(cursor_pos) == ENTRY(width)-1 )
		{
                      MoveLeft( TextHandle );
		}
                AddToBetaLetter(letter, &(ENTRY(btext)[ENTRY(curpos)]));

                EntryPrintChar(TextHandle, ENTRY(btext)[ENTRY(curpos)] );

                ENTRY(curpos)++;
        }
        else
        {
                ENTRY(cursor_pos)--;
                bp = &ENTRY(btext)[ENTRY(curpos)-1];

                bp->attrib = XOR_PUT;
                EntryPrintChar(TextHandle,*bp);
                bp->attrib = OR_PUT;

                AddToBetaLetter(letter, &(ENTRY(btext)[ENTRY(curpos)-1]));

                ENTRY(cursor_pos)--;
                EntryPrintChar(TextHandle,*bp);
        }
        EntryCursorOn( TextHandle );
}/*
END of addGreekLetter
*/

/*-------------------------------------------------------------------
 *  addLatinLetter           06.25.90 MM
 *
 *  Add Latin (Phoenician) letter to  entry.
 *-------------------------------------------------------------------
 */
void
addLatinLetter( Handle TextHandle, int letter )
{
	ENTRY(buffer)[ENTRY(buffpos)]=(char) letter;
	ENTRY(buffer)[++ENTRY(buffpos)]=(char) '\0';

        EntryCursorOff(TextHandle);
        if( ENTRY(cursor_pos) == (ENTRY(width)-1) )
        {
                MoveLeft( TextHandle );
        }
        ENTRY(btext)[ENTRY(curpos)]=empty_betaletter;
        ENTRY(btext)[ENTRY(curpos)].dc_num = LCHAR;
        ENTRY(btext)[ENTRY(curpos)].dc_codes[0] = letter;
        EntryPrintChar(TextHandle,ENTRY(btext)[ENTRY(curpos)]);
        EntryCursorOn(TextHandle);
	ENTRY(curpos)++;
}/*
END of addLatinLetter
*/

/*-------------------------------------------------------------------
 *  AddNumber                02.04.92 MM
 *
 *  Add number to numeric field
 *-------------------------------------------------------------------
 */
void
addNumber( Handle TextHandle, int letter )
{
    /*    if(isdigit(letter)) */
                addLatinLetter( TextHandle, letter );
}/*
END of AddNumber */


/*-------------------------------------------------------------------
 *  deleteLetter             06.25.90 MM
 *
 *  Does all the things necessary to effect a backspace.
 *-------------------------------------------------------------------
 */
void
deleteLetter( Handle TextHandle )
{
        int xpos,diff;

        if( ENTRY(curpos) > 0 )
	{
                diff = ENTRY(curpos)-ENTRY(cursor_pos)-ENTRY(frame_start);
                while(diff--)
                        ShiftLeft(TextHandle);

                if(ENTRY(txttype) == GREEK)
                    while( DiaVal( ENTRY(buffer)[ENTRY(buffpos)-1] ) )
                         ENTRY(buffpos)--;

                ENTRY(buffpos)--;
		ENTRY(buffer)[ENTRY(buffpos)]=(char) '\0';
                ENTRY(curpos)--;
                ENTRY(btext)[ENTRY(curpos)] = empty_betaletter;


                EntryCursorOff(TextHandle);

                ENTRY(cursor_pos)--;
                xpos=ENTRY(cursor_pos)*CHARWIDTH+ENTRY(button->window.right)+2;
                bar(xpos,ENTRY(window.top)+1,
                    xpos+CHARWIDTH,ENTRY(window.top)+LINEHEIGHT-1);

                if(ENTRY(frame_start) > 0)
                {
                    MoveRight(TextHandle);

                    ENTRY(cursor_pos) = 0;
                    EntryPrintChar( TextHandle,
                                    ENTRY(btext)[ENTRY(frame_start)] );
                    ENTRY(cursor_pos) = ENTRY(curpos) - ENTRY(frame_start);
                }

                EntryCursorOn( TextHandle );
	}
}/*
END of deleteLetter
*/

/*-------------------------------------------------------------------
 *  ShiftLeft               10.17.90 MM
 *
 *  Shift the entry text field to the left
 *-------------------------------------------------------------------
 */
void
ShiftLeft( Handle TextHandle )
{
        EntryCursorOff(TextHandle);
        if(ENTRY(cursor_pos) < (ENTRY(width)-1) )
        {
            ENTRY(cursor_pos)++;
        }
        else if((ENTRY(frame_start)+ENTRY(width)-1) <= ENTRY(curpos))
        {
            MoveLeft(TextHandle);

            EntryPrintChar( TextHandle,
                            ENTRY(btext)[ENTRY(frame_start)+ENTRY(width)-2]);
        }
        EntryCursorOn(TextHandle);
}/*
END of ShiftLeft */


/*-------------------------------------------------------------------
 *  ShiftRight              10.17.90 MM
 *
 *  Shift the entry text field to the right
 *-------------------------------------------------------------------
 */
void
ShiftRight( Handle TextHandle )
{
        EntryCursorOff(TextHandle);
        if(ENTRY(cursor_pos) > 0 )
        {
            ENTRY(cursor_pos)--;
        }
        else if(ENTRY(frame_start) > 0)
        {
            MoveRight(TextHandle);

            EntryPrintChar( TextHandle, ENTRY(btext)[ENTRY(frame_start)] );
            ENTRY(cursor_pos)--;
        }
        EntryCursorOn(TextHandle);
}/*
END of ShiftRight */


void
MoveLeft( Handle TextHandle )
{
    void *buffer;

    ENTRY(frame_start)++;

    ENTRY(cursor_pos)--;

    if(ENTRY(window.visible))
    {
       buffer = getScreenRec( ENTRY(button->window.right)+2+CHARWIDTH,
                               ENTRY(window.top)+1,
                               ENTRY(window.right)-1-CHARWIDTH,
                               ENTRY(window.bottom)-1 );

       clearArea( ENTRY(button->window.right)+2, ENTRY(window.top)+1,
                  ENTRY(window.right)-1-CHARWIDTH, ENTRY(window.bottom)-1 );

       putimage(ENTRY(button->window.right)+2,ENTRY(window.top)+1,buffer,
                COPY_PUT);

    }
}


void
MoveRight( Handle TextHandle )
{
      void *buffer;

      ENTRY(frame_start)--;
      if(ENTRY(window.visible))
      {
         buffer = getScreenRec( ENTRY(button->window.right)+2,
                                ENTRY(window.top)+1,
                               ENTRY(window.right)-1-(2*CHARWIDTH),
                               ENTRY(window.bottom)-1 );

         clearArea( ENTRY(button->window.right)+2, ENTRY(window.top)+1,
                    ENTRY(window.right)-1, ENTRY(window.bottom)-1 );

         putimage(ENTRY(button->window.right)+2+CHARWIDTH,
                  ENTRY(window.top)+1,buffer,
                     COPY_PUT);
      }
}



/*------------------------------------------------------------------
 *  EntryCursorOn            06.18.92 MM
 *
 *  Draws the cursor at the cursor position.
 *------------------------------------------------------------------
 */
void
EntryCursorOn( Handle TextHandle )
{
   int ncurs = (ENTRY(cursor_pos) == (ENTRY(width)-1)) ? 1 : 0 ;

   if(ENTRY(window.visible))
        putimage( ENTRY(cursor_pos)*CHARWIDTH + ENTRY(button->window.right)+2,
                  ENTRY(ypos)+2, _cursor[ncurs],OR_PUT );
}/*
END of EntryCursorOn */


/*------------------------------------------------------------------
 *  EntryCursorOff           06.18.92 MM
 *
 *  Removes the cursor at the cursor position.
 *------------------------------------------------------------------
 */
void
EntryCursorOff( Handle TextHandle )
{
    int ncurs = (ENTRY(cursor_pos) == (ENTRY(width)-1)) ? 1 : 0 ;

    if(ENTRY(window.visible))
        putimage( ENTRY(cursor_pos)*CHARWIDTH + ENTRY(button->window.right)+2,
                  ENTRY(ypos)+2, _cursor[ncurs], XOR_PUT );
}/*
END of EntryCursorOn */


/*------------------------------------------------------------------
 *  drawCursor               06.25.90 MM
 *
 *  Draws the cursor on the screen.
 *------------------------------------------------------------------
 */
void
drawCursor( Handle TextHandle )
{
   int ncurs = (ENTRY(cursor_pos) == (ENTRY(width)-1)) ? 1 : 0 ;


        putimage( ENTRY(cursor_pos)*CHARWIDTH + ENTRY(button->window.right)+2,
                  ENTRY(ypos)+2, _cursor[ncurs], XOR_PUT );
}/*
END of drawCursor
*/

