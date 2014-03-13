/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\win.h 2.0 1993/09/18 16:44:12 mwm Exp mwm $
*/

#ifndef m_win_h
#define m_win_h

#include <stdarg.h>
#include <graphics.h>

#define NUMATT	16
#define LINEHEIGHT    13
#define CHARWIDTH     9

typedef void **Handle;
typedef int (*PFI)();
typedef void (*PFV)();
typedef void (*PFH)( Handle );
#define Wintsize    2
#define WBoolsize   2
#define WPFIsize    4
#define WPFVsize    4
#define WPFHsize    4
#define Wcharsize   1
#define Wcpsize     4
#define Wcplist     101
#define ATTR( _size_, _ordinal_ )       ((_size_<<8) | _ordinal_)
#define ATTSIZE( _attr_ )             ( ((_attr_ & 0xFF00) >> 8) )

enum {GREEK,LATIN,NUMERIC};

typedef enum {  END_LIST    =0,
                W_X         =ATTR(Wintsize,1),
                W_Y         =ATTR(Wintsize,2),
                W_WIDTH     =ATTR(Wintsize,3),
                W_HEIGHT    =ATTR(Wintsize,4)
} Attrib;

struct _window
{
    BOOL visible;
	int left,top,right,bottom;
	int	AltKey;
	Handle  (*allocOb)();
	struct _window *(*setAttProc)( Handle, va_list );
	PFH     makeProc;
    PFH     killProc;
    void    (*keyproc)( Handle, int, int );
	PFH	AltProc;
	PFH	SwitchProc;
    PFH FocusOnProc;
    PFH FocusOffProc;
};
typedef struct _window 		Window;

struct _child
{
	Handle Object;
	struct _child *nextChild;
};
typedef struct _child		Child;

void initWin( void );
Handle createOb( Handle, Window *, ... );
void SetAttributes( Handle, ... );
void setCoreAttributes( Window *, va_list );
void keyLoop( void );
void SetNewObject( Window *, Handle );
void *getScreenRec( int, int, int ,int );
void ScrollWindow( Window, int );
void HighLightLine( Window, int );
void clearWindow( Window * );
void clearArea( int, int, int, int );
void setFocus( Window * , Handle );
void getFocus( Window ** , Handle * );
void setAltProc(  int, PFI );
int letterOfScancode( int );
void dropArg( va_list *, Attrib );
void BindFuncKey( int key, PFV func );
#endif
