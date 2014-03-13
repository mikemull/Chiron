/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\button.h 2.0 1993/09/18 16:44:11 mwm Exp mwm $
*/

#ifndef button_h
#define button_h

#include "list.h"
#define BATTR( size, ordinal )  ATTR( (size), (20+ordinal) )
typedef enum {  B_LABEL     =BATTR(Wcpsize,1),
                B_KEY       =BATTR(Wintsize,2),
                B_FUNC      =BATTR(WPFIsize,3)
        } B_Attrib;

struct _button
{
	Window	window;
    List *  list;
	char *	label;
	PFI	ActProc;
};
typedef struct _button	Button;
typedef Button		**ButtonOb;
typedef enum {NOSAVE,SAVE,UNSAVE} ButtonStat;

extern Window *ButtonObject;

Handle allocButton( void );
Window *setButtonAttributes( Handle,va_list );
void makeButton( Handle );
void B_AltProc( Handle );
void B_SwitchProc( Handle );
void ButtonMakeList( Handle );
void buttonOn( Button *, ButtonStat );
Button *currentButton( void );
unsigned char buttonIsOn( void );
#endif
