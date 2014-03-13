/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\entry.h 2.0 1993/09/18 16:44:10 mwm Exp mwm $
*/

#ifndef entry_h
#define entry_h

#include "greek.h"
#include "button.h"
#include "list.h"

#define EATTR(size,ordinal)     ATTR( (size), (40+ordinal))
typedef enum {  E_WIDTH =   EATTR(Wintsize,1),
                E_TYPE  =   EATTR(Wintsize,2),
                E_VISIBLE = EATTR(WBoolsize,3),
                E_FIXED =   EATTR(WBoolsize,4),
                E_TRIGGER = EATTR(WPFVsize,5)
            }E_Attrib;

#define ENTRY_MAX_BUFFER    128
#define ENTRY_MAX_WIDTH     80

struct _entry
{
    Window  window;
	Button *button;
	List   *list;
    char    buffer[ENTRY_MAX_BUFFER];
	int     buffpos;
    BETALETTER btext[ENTRY_MAX_WIDTH];
    int     width;
    int     curpos;
    int     cursor_pos;
    int     frame_start;
	unsigned xpos,ypos;
	unsigned txttype;
    BOOL fixed;
    void *back_save;
    PFV trigger;
};
typedef struct _entry	Entry;
typedef Entry		**EntryOb;

extern Window *EntryObject;

Handle allocEntry( void );
Window *setEntryAttributes( Handle, va_list );
void makeEntry( Handle );
void EntrySelect( Handle );
void EntryGetValue( Handle, void * );
void EntrySetValue( Handle, char * );
void EntrySetList( Handle, char **, int );
void EntryMakeList( Handle );
void doText( Handle, int, int );
void EntryEditKey( Handle, int );
void EntryDown( Handle );
void addGreekLetter( Handle,int);
void addLatinLetter( Handle,int);
void addNumber( Handle,int);
void deleteLetter( Handle );
void E_AltProc( Handle );
void E_SwitchProc( Handle );
void EntryFocusOn( Handle );
void EntryFocusOff( Handle );
void drawCursor( Handle );
void entryListDown( Handle );
void nextSelection( Handle );
void lastSelection( Handle );
void highLightSelection( Handle );
void MoveLeft( Handle );
void MoveRight( Handle );
void ShiftLeft( Handle );
void ShiftRight( Handle );
void EntryCursorOn( Handle );
void EntryCursorOff( Handle );
#endif
