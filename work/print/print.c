/*=========================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\print.c 2.0 1993/09/18 16:50:32 mwm Exp mwm $
 *
 *   Print routines.
 *
 *   last modified : 03-11-91
 *   $Log: print.c $
 * Revision 2.0  1993/09/18  16:50:32  mwm
 * Start of new version.
 *
 *
 *=========================================================
 */
#include <stdio.h>
#include <bios.h>
#include <time.h>
#include <string.h>
#include "tlg.h"
#include "text.h"
#include "greek.h"
#include "canon.h"
#include "citlow.h"
#include "citation.h"
#include "work.h"
#include "print.h"
#include "work.h"
#include "tlgmem.h"
#ifdef EPSON_FX
#include "epson_fx.h"
#endif

#define BLOCK_8_K 	( 1024 * 8 )
#define PAGE_LEN 60
#define LINE_WID 80

static page_line;
static PrintStruc prin_funcs;

/*---------------------------------------------------------
 *    SelectPrinter          03-11-91
 *
 *    This routine assigns the printer function pointers
 *    to the approriate printer routines based on the value
 *    of printer.
 *
 *    last modified : 03-13-91
 *---------------------------------------------------------
 */
void
SelectPrinter( prn_devs prn )
{

    switch ( prn )
    {
      case epson_fx :  prin_funcs.Setup = FX_Setup;
                       prin_funcs.PrintStr = FX_PrintStr;
                       prin_funcs.InitPrinter = FX_InitPrinter;
                       prin_funcs.HomePrinter = FX_HomePrinter;
                       prin_funcs.SelectFont = FX_SelectFont;
                       prin_funcs.FlushPrinter = FX_FlushPrinter;
                       prin_funcs.FormFeed = FX_FormFeed;
                       prin_funcs.LineFeed = FX_LineFeed;
                       prin_funcs.XlateBetaLine = FX_XlateBetaLine;
                       (prin_funcs.Setup)();
		       break;
      default :        break;
    }
}


/*---------------------------------------------------------
 *   InitializePrinter
 *
 *   Send printer init string and select font.
 *---------------------------------------------------------
 */
void
InitializePrinter( fonts font )
{
        (prin_funcs.InitPrinter)();
        (prin_funcs.SelectFont)(font);
        (prin_funcs.FlushPrinter)();
}

/*---------------------------------------------------------
 *   PrintBlock              05.03.91
 *
 *   Print a Block.
 *
 *   last modified :  05.03.91  GH
 *---------------------------------------------------------
 */
void
PrintBlock( char *block )
{
    static char *line_index[1000], *cit_index[1000];
    char *buf_ptr, print_buf[300];
	BETALETTER beta_buf[100];
	int line_count=0,line_num=0,bc_cnt=0;

        line_count = IndexBlock((BYTE *)block,line_index,cit_index);
        for( line_num=0; line_num<5; line_num++ )
	{
        if( ! ( page_line % PAGE_LEN ) )
	    {
	       (prin_funcs.FormFeed)();
               page_line = 0;
	    }
	    buf_ptr = line_index[line_num];
	    while( !HIBITSET( *buf_ptr ) )
	    {
	       buf_ptr = GetBetaLine(buf_ptr,beta_buf,LINE_WID,&bc_cnt);
	       (prin_funcs.XlateBetaLine)(beta_buf,print_buf,bc_cnt);
	       (prin_funcs.PrintStr)(print_buf,strlen(print_buf));
               page_line++;
	    }
	 }
}


/*---------------------------------------------------------
 *   PrintContext            04-29-91 GH
 *
 *   Print a context from a block
 *   PrintContext currently does not span blocks.
 *
 *   last modified  :  05-07-91
 *---------------------------------------------------------
 */
void
PrintContext( char *line_index[], int line_count,
              int line_number, int context)
{
	int beg_line, end_line, bc_cnt, i;
	char *line_ptr, print_buf[300];
	BETALETTER beta_buf[100];

        /*
        ** Print citation first
        */
        CitStrfromStruc( WorkCitStruct(), print_buf );
        (prin_funcs.SelectFont)( latin );
        (prin_funcs.PrintStr)( print_buf, strlen(print_buf));
        (prin_funcs.LineFeed)();
        (prin_funcs.LineFeed)();
        (prin_funcs.SelectFont)( greek );
        page_line+=2;

	beg_line = (line_number - context) > 0 ? (line_number - context)
					       : 0;
	end_line = (line_number + context) > line_count
					       ? line_count
					       : line_number + context;
	for( i=beg_line; i <= end_line; i++ )
	{
           if( ( page_line + ( 2 * context ) > PAGE_LEN ) )
	   {
	      (prin_funcs.FormFeed)();
              page_line = 0;
	   }

	   line_ptr = line_index[i];
	   do
	   {
	      line_ptr = GetBetaLine(line_ptr,beta_buf,LINE_WID,&bc_cnt);
	      (prin_funcs.XlateBetaLine)(beta_buf,print_buf,bc_cnt);
	      (prin_funcs.PrintStr)(print_buf,strlen(print_buf));
              page_line++;
	   }while( !HIBITSET( *line_ptr ) && !(i==end_line));
    }
    (prin_funcs.LineFeed)();
    (prin_funcs.PrintStr)( "__________", 10);
    (prin_funcs.LineFeed)();
    (prin_funcs.LineFeed)();
    (prin_funcs.LineFeed)();
    page_line += 4;

}



/*-------------------------------------------------------------------
 *   PingPrinter
    port == 0 ===> lpt1
    port == 1 ===> lpt2
    etc ..
 *-------------------------------------------------------------------
*/
BOOL
PingPrinter( int port )
{
    int status;

    status = biosprint(2,0,port);

    if( (status & 0x01) || (status & 0x08) ||
        (status & 0x20) || ( status == 0 ) || (port == -1) )
        return( FALSE );                      /* no printer */
    else
        return( TRUE );
}/*
END of PingPrinter */



/*-------------------------------------------------------------------
 *    PrintCanon
 *
 *    Function to print the canon bibliography on the currently
 *    selected print device.  This is here because prin_funcs
 *    is a static and is not visible in canon.c
 *-------------------------------------------------------------------
 */
BOOL
PrintCanon( void )
{
    char print_buf[300];
    int i=0;
    char **canon_biblio;
    BOOL printok = TRUE;

    CanBibInit();
    canon_biblio = CanBibGetBiblio( Work.author_num, Work.work_num );
    (prin_funcs.SelectFont)(latin);
    (prin_funcs.PrintStr)("\n\n******************************\n\n",34 );
    (prin_funcs.PrintStr)("  TLG Canon Bibliography\n",25 );
    i=0;
    while( canon_biblio[i] != NULL )
    {
       sprintf(print_buf,"%s\n",canon_biblio[i]);
       (prin_funcs.PrintStr)(print_buf,strlen(print_buf));
       i++;
    }

   (prin_funcs.LineFeed)();
   (prin_funcs.PrintStr)("--------------",14);
   (prin_funcs.LineFeed)();
   (prin_funcs.FlushPrinter)();
   CanBibFree();
   return( printok );
}

