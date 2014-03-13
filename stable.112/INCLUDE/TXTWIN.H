/*
**  $Header: C:\MIKE\TLG\CHIRON\RCS\txtwin.h 2.0 1993/09/18 16:44:14 mwm Exp mwm $
*/

#ifndef txtwin_h
#define txtwin_h

#include "citlow.h"

#define HORZ_SCROLL_INC     20

#define TATTR(size,ordinal)     ATTR((size),(60+ordinal))
typedef enum {  T_BUFF  =   TATTR(Wcpsize,1),
                T_COLS  =   TATTR(Wintsize,2),
                T_LINES =   TATTR(Wintsize,3),
                T_SEARCHPATT =   TATTR(Wcpsize,4)
            } T_Attrib;

struct _txtwin
{
	Window window;
    char * buffer_pos;
	int     rows,cols,cur_row,cur_col;
    BYTE    line_start_offset;
	unsigned xpos,ypos;
        citation_t   citation ;
    char searchpatt[40];
};

typedef struct _txtwin   TxtWin;
typedef TxtWin       **TxtWinOb;

extern Window *TxtWinObject;

Handle allocTxtWin( void );
Window *setTxtWinAttributes( Handle, va_list );
void *GetTxtWinAttributes( Handle, Attrib);
void makeTxtWin( Handle );
void getEditKey( Handle, int, int );
void T_AltProc( Handle );
void T_SwitchProc( Handle );
void displayText( Handle );
void fillScreen( Handle );
void writeline( int index, Handle );
void scrollUp( Handle, int );
void scrollDown( Handle, int );
void highLight( Handle );
void TxtWinBorderOn( Handle );
void TxtWinBorderOff( Handle );
void reverseVideo( Handle );
#endif
