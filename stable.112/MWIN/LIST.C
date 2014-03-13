/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\list.c 2.0 1993/09/18 16:31:47 mwm Exp mwm $
 *
 *  Code for list object
 *
 *  $Log: list.c $
 * Revision 2.0  1993/09/18  16:31:47  mwm
 * Start of new version.
 *
 * Revision 1.7  92/04/12  14:45:00  ROOT_DOS
 * Moved structure init stuff.
 * 
 * Revision 1.6  92/03/19  20:08:03  ROOT_DOS
 * Safety check
 * 
 * Revision 1.5  92/03/09  20:24:29  ROOT_DOS
 * Added focuson and focusoff procs
 * 
 * Revision 1.4  92/03/09  18:19:02  ROOT_DOS
 * Added code to get and reset focus.
 * 
 * Revision 1.3  91/12/01  14:16:18  ROOT_DOS
 * Changed keyproc arguments to add modifiers
 * 
 * Revision 1.2  91/12/01  13:37:29  ROOT_DOS
 * Added Change ListItem
 * 
 *-------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "tlg.h"
#include "win.h"
#include "list.h"
#include "notice.h"
#include "editkey.h"
#include "tlgmem.h"
#include "greek.h"

/*
   Default List object
 */
Window ListWindow =
{
        1,
	0,0,100,100,
	'0',
        allocList,
        setListAttributes,
        makeList,
        NULL,
        handleEditKey,
        L_AltProc,
        NULL,
        NULL,
        NULL
};

Window *ListObject = &ListWindow;

#define LIST(a) (((List *)(*Lhnd))->a)
#define PRINTITEM(ypos,string) \
{ \
        if(LIST(Type) == LATIN) \
                outtextxy(LIST(window.left)+5,(ypos),(string)); \
        else \
                PrintGreekString(LIST(window.left)+5,(ypos),(string)); \
}

static Handle temp_handle;
static Window *temp_win;

static List *dlist;
                
/*-------------------------------------------------------------------
 *  L_AltProc
 *
 *  Function called when correct alt-key combo is typed.
 *-------------------------------------------------------------------
 */
void
L_AltProc( Handle Lhnd )
{
	return;
}/*
END of L_AltProc
*/

/*-------------------------------------------------------------------
 *  allocList                06.25.90 MM
 *
 *  Function to allocate List object and return Handle.
 *-------------------------------------------------------------------
 */
Handle
allocList( void )
{
	Handle Lhnd;

	Lhnd  = (Handle) TLGmalloc(sizeof(List *));
	*Lhnd = TLGmalloc( sizeof(List) );
        LIST(window) = ListWindow;
	LIST(screenMap)= NULL;
        LIST(list) = NULL;
	LIST(list_parent_handle)=NULL;
	LIST(list_parent_core)=NULL;
        LIST(ActProc) = NULL;
        LIST(ListSize) = LIST(Selection) = LIST(row) = 0;
        LIST(Type)=LATIN;

	return(Lhnd);
}/*
END of allocList
*/

/*-------------------------------------------------------------------
 *  setListAttributes        06.25.90 MM
 *
 *  Set default and specified list attributes.
 *-------------------------------------------------------------------
 */
Window *
setListAttributes( Handle Lhnd, va_list ap )
{
	Attrib attr;

	setCoreAttributes( &LIST(window), ap );

	while((attr=va_arg(ap,Attrib)) != END_LIST)
	{
		switch(attr)
		{
                        case L_KEY:
                                LIST(window.AltKey) = va_arg(ap,int);
				break;
                        case L_FUNC:
                                LIST(ActProc)=va_arg(ap,PFVIC);
				break;
                        case L_TYPE:
                                LIST(Type) = va_arg(ap,int);
				break;
                        case L_LIST:
                                LIST(list) = GenerateList( Lhnd,&ap );
                                break;
			default:
				dropArg( &ap, attr );
				break;
		}
	}
        return(&LIST(window));
}/*
END of setListAttributes
*/

/*-------------------------------------------------------------------
 *  makeList                 06.25.90 MM
 *
 *  Makes the list object.
 *-------------------------------------------------------------------
 */
void
makeList( Handle Lhnd )
{
        dlist = ((List *)(*Lhnd));
        LIST(x1)=LIST(window.left);
        LIST(y1)=LIST(window.top);
        LIST(x2)=LIST(window.right);
        LIST(y2)=LIST(window.bottom);
        getFocus( &temp_win, &temp_handle );
        setFocus( &LIST(window), Lhnd );
	LIST(screenMap)= getScreenRec(LIST(x1),LIST(y1),LIST(x2),LIST(y2));
        DrawList(Lhnd);
}/*
END of makeList
*/


/*-------------------------------------------------------------------
 *  DrawList                 1.16.91 MM
 *
 *  Draws the list.
 *-------------------------------------------------------------------
 */
void
DrawList( Handle Lhnd )
{
	int i,j=0,ty;
	char mess[35];

	clearArea( LIST(x1),LIST(y1),LIST(x2),LIST(y2) );
	rectangle( LIST(x1),LIST(y1),LIST(x2),LIST(y2) );

        ty= LIST(window.top)+3;

        if(LIST(Selection) < 10)
                LIST(row) = LIST(Selection);

        for(i= (LIST(Selection)-LIST(row)) ;
            i< (LIST(Selection)+(9-LIST(row))+1) ;
            i++, ty+=LINEHEIGHT)
	{
		while( j<35 ) {mess[j]='\0';j++;} j=0;
		strncpy(mess,LIST(list[i]),34);
                PRINTITEM(ty,mess)
                if( i==(LIST(ListSize)-1) ) break;
	}
        HighLightLine(LIST(window),LIST(row));
}/*
END of DrawList
*/

/*-------------------------------------------------------------------
 *  handleEditKey            06.25.90 MM
 *
 *  Key handler for list object.  Deals only with edit keys.
 *-------------------------------------------------------------------
 */
void
handleEditKey( Handle Lhnd, int key, int mod )
{
	switch (key)
	{
	case DOWN_ARROW:
                nextSelection(Lhnd);
		break;
	case UP_ARROW:
                lastSelection(Lhnd);
		break;
        case PG_DOWN:
                if( (LIST(Selection)+10) < LIST(ListSize) )
                {
                        LIST(Selection)+=10;
                        DrawList(Lhnd);
                }
                break;
        case PG_UP:
                if( (LIST(Selection)-10) >= 0 )
                {
                        LIST(Selection)-=10;
                        DrawList(Lhnd);
                }
                break;
	case ESCAPE:
		listDown( Lhnd );
		break;
	case CARRIAGE_RETURN:
		listDown( Lhnd );
		(*LIST(ActProc))(
				LIST(Selection),
				LIST(list[LIST(Selection)])
				);
		break;
	}
}/*
END of handleEditKey
*/

/*-------------------------------------------------------------------
 *  nextSelection            06.25.90 MM
 *
 *  Moves to next selection down in list.
 *-------------------------------------------------------------------
 */
void
nextSelection( Handle Lhnd )
{
        if(LIST(Selection) == (LIST(ListSize)-1) )
                BEEP;
        else
        {
           HighLightLine(LIST(window),LIST(row));
           LIST(Selection)++;
           if( LIST(row) < (LIST(ListSize) > 10 ? 9 : LIST(ListSize)-1))
           {
                LIST(row)++;
                HighLightLine(LIST(window),LIST(row));
           }
           else
           {
                ScrollWindow(LIST(window),-1);
                PRINTITEM(LIST(window.top)+3+9*LINEHEIGHT,
                          LIST(list[LIST(Selection)]) )
                HighLightLine(LIST(window),LIST(row));
           }

        }
}/*
END of nextSelection
*/

/*-------------------------------------------------------------------
 *  lastSelection            06.25.90 MM
 *
 *  Moves to previous selection in list.
 *-------------------------------------------------------------------
 */
void
lastSelection( Handle Lhnd )
{
        if(LIST(Selection) == 0)
                BEEP;
        else
        {
                HighLightLine(LIST(window),LIST(row));
                LIST(Selection)--;

                if( LIST(row) >0 )
                {
                        LIST(row)--;
                        HighLightLine(LIST(window),LIST(row));
                }
                else
                {
                        ScrollWindow(LIST(window),1);
                        PRINTITEM(LIST(window.top)+3,
                                LIST(list[LIST(Selection)]) )
                        HighLightLine(LIST(window),LIST(row));
                }
        }
}/*
END of lastSelection
*/

/*-------------------------------------------------------------------
 *  listDown                 06.25.90 MM
 *
 *  Takes down list.
 *-------------------------------------------------------------------
 */
void listDown( Handle Lhnd )
{
	if( LIST(screenMap) != NULL )
	{
		putimage( LIST(x1),LIST(y1), LIST(screenMap), COPY_PUT );
		TLGfree(LIST(screenMap));
                setFocus(temp_win,temp_handle);
	}
	LIST(screenMap)=NULL;
}/*
END of listDown
*/

/*-------------------------------------------------------------------
 *  setList                  06.25.90 MM
 *
 *  Sets list to specified array of stings.
 *-------------------------------------------------------------------
 */
void
setList( Handle Lhnd, char **sList, int ListSize )
{
        LIST(Selection)=LIST(row)=0;
	LIST(ListSize)= ListSize;
	LIST(list)= sList;
}/*
END of setList
*/

void
ChangeListItem( Handle Lhnd, int num_item, char *new_item)
{
        strcpy(LIST(list[num_item]),new_item);
}

char **
GenerateList( Handle Lhnd, va_list *ap )
{
        int i=0;
        char *item;
        char **list;
        list = (char **) TLGmalloc(10*sizeof(char *));

        while( (item = va_arg((*ap),char *)) != NULL )
        {
                list[i] = (char *) TLGmalloc(40);
                strncpy(list[i],item,40);
                i++;
        }
        LIST(ListSize) = i;
        LIST(Selection) = 0;

        return(list);
}

