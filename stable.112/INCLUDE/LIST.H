/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\list.h 2.0 1993/09/18 16:43:35 mwm Exp mwm $
*/

#ifndef list_h
#define list_h

#define LATTR(size,ordinal)     ATTR((size),(80+ordinal))
typedef enum{   L_KEY   =   LATTR(Wintsize,1),
                L_FUNC  =   LATTR(WPFIsize,2),
                L_LIST  =   LATTR(Wcplist,3),
                L_TYPE  =   LATTR(Wintsize,4)
            }L_Attrib;

typedef void (*PFVIC)( int, char *) ;

struct _list
{
	Window	window;
	char **list;
	int ListSize;
    int Type;
	Handle list_parent_handle;
	Window *list_parent_core;
	void	(*ActProc)( int, char * );
	void *screenMap;
	int x1,y1,x2,y2;
	int Selection;
    int row;
};
typedef struct _list  List;
typedef List      **ListOb;

extern Window *ListObject;

Handle allocList( void );
Window *setListAttributes( Handle,va_list );
void makeList( Handle );
void DrawList( Handle );
void setList( Handle, char **, int );
void L_AltProc( Handle );
char **GenerateList( Handle, va_list * );
void ChangeListItem( Handle, int, char *);
void listDown( Handle );
void handleEditKey( Handle, int, int );
void nextSelection( Handle );
void lastSelection( Handle );
void highLightSelection( Handle );
#endif
