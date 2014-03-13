/*=========================================================
 *
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\epson_fx.c 2.0 1993/09/18 16:51:00 mwm Exp mwm $
 *
 *   Print Driver routines for Epson FX-80 and compatable
 *   Printers.
 *
 *   Last Modified : 03-12-91   GH
 *=========================================================
 */

#define EPSON_FX

#include <bios.h>
#include <dos.h>
#include "tlg.h"
#include "tlgmem.h"
#include "greek.h"
#include "print.h"
#include "epson_fx.h"

#define FX_BYTES 13
#define FX_CHAR_CNT 83
#define FX_BS 8
#define FX_NL 10
#define FX_CR 13
#define FX_COMP  15
#define FX_UNCOMP  18

#define DELAY   1000

char *FX_font_buf;
static int port_id=0;

/*---------------------------------------------------------
 *   FX_Setup                03-13-91
 *
 *   Routine called once when printer is selected to
 *   perform set-up for other printer functions
 *   Reads in font file for later downloading and
 *   sets left margin to 5.
 *
 *   Last modified  : 03-13-91
 *---------------------------------------------------------
 */
int
FX_Setup()
{
    FILE *font_file;
    int items_read=0;

    FX_font_buf = ( char * ) TLGmalloc( 1300 * sizeof(char));
    font_file = fopen("epson_fx.fxf","rb");
    items_read = fread(FX_font_buf,FX_BYTES,FX_CHAR_CNT,font_file);
    fclose(font_file);
    return( ( ( items_read == FX_CHAR_CNT ) ? 0 : items_read ) );
}

/*---------------------------------------------------------
 *   FX_PrintStr             03-12-91
 *
 *   Outputs the string to the printer attached to port
 *   port_id.
 *
 *   last modified           03-11-91
 *---------------------------------------------------------
 */
int
FX_PrintStr( char *str, int len )
{
   int i,result;

   for( i = 0; i < len; i++ )
   {
      if( str[i] == '&' )
         FX_SelectFont(latin);
      else if( str[i] == '$' )
         FX_SelectFont(greek);
      else
      {
         result = FX_PrintChar(str[i]);

	 if (( result != 0x10 ) && ( result != 0x40 ) )
	    return ( result );
      }
   }
   return( 0 );
}

/*---------------------------------------------------------
 *   FX_FlushPrinter         03-12-91
 *
 *   Flushes the Epson Printer buffer by issuing a
 *   0 line vertical tab
 *
 *   last modified           03-11-91
 *---------------------------------------------------------
 */
int
FX_FlushPrinter()
{
   static char FX_flush_str[] = {27,102,1,0};
   return( FX_PrintStr(FX_flush_str,4) );
}

/*---------------------------------------------------------
 *   FX_FormFeed             04-23-91
 *
 *   Cause printer to feed to top of form
 *
 *---------------------------------------------------------
 */
int
FX_FormFeed()
{
   static char FX_ff_str[] = {12,0};

   return( FX_PrintStr( FX_ff_str,2 ) );
}



/*---------------------------------------------------------
 *   FX_LineFeed             04-23-91
 *
 *   Cause printer to issue a line feed
 *
 *---------------------------------------------------------
 */
int
FX_LineFeed()
{
   static char FX_lf_str[] = {13,10,0};

   return( FX_PrintStr( FX_lf_str,3 ) );
}



/*---------------------------------------------------------
 *   FX_HomePrinter          03-12-91
 *
 *   Moves the print head to left side of page by issuing
 *   a CR LF character sequence.  Buffer contents will be
 *   printed
 *
 *   last modified           03-11-91
 *---------------------------------------------------------
 */
int
FX_HomePrinter()
{
   static char FX_home_str[] = {13,10};
   return( FX_PrintStr(FX_home_str,2) );
}

/*---------------------------------------------------------
 *   FX_InitPrinter          03-12-91
 *
 *   Initializes the printer, clearing all buffers.
 *
 *   last modified           03-11-91
 *---------------------------------------------------------
 */
int
FX_InitPrinter()
{
   static char FX_init_str[] = {27,64};     /* init printer */
   static char FX_lm_str[] = {27,108,5,0};  /* set left margin */
   FX_PrintStr(FX_init_str,2);
   return( FX_PrintStr(FX_lm_str,4) );
}

/*---------------------------------------------------------
 *   FX_SelectFont           03-12-91
 *
 *   Selects the indicated printer font and
 *   downloads it ( if necessary )
 *
 *   last modified           03-13-91
 *---------------------------------------------------------
 */
int
FX_SelectFont( fonts font_selection)
{
   int i,j;
   static char FX_latin_str[] = {27,37,0,0};

   switch ( font_selection )
   {
      case greek : for(i=0;i<FX_CHAR_CNT;i++)
		      {
                         FX_PrintChar(27);
                         FX_PrintChar(38);
                         FX_PrintChar(0);
                         FX_PrintChar(FX_font_buf[i*FX_BYTES]);
                         FX_PrintChar(FX_font_buf[i*FX_BYTES]);
			 for(j=1;j<FX_BYTES;j++)
			 {
                             FX_PrintChar(FX_font_buf[(i*FX_BYTES)+j]);
			 }
		      }
                      FX_PrintChar(27);
                      FX_PrintChar(37);
                      FX_PrintChar(1);
                      FX_PrintChar(0);
		      break;

      case latin : FX_PrintStr(FX_latin_str,4);
		      break;

      default  :      return( 1 );
   }
   return( 0 );
}

/*---------------------------------------------------------
 *   FX_XlateBetaLine        03-12-91
 *
 *   Translates a line of Betacode characters to a line
 *   suitable for printing with FX_PrintStr.  Control
 *   codes are embedded.
 *
 *   last modified           04-02-91
 *---------------------------------------------------------
 */
int
FX_XlateBetaLine( BETALETTER *beta_line, char *print_line,
		  int print_line_len )
{
    int beta_count = 0,p_index=0,i=0;

    for( beta_count=0;beta_count<print_line_len;beta_count++)
    {
	print_line[p_index++] = beta_line[beta_count].charcode;
        if( (beta_line[beta_count].dc_num > 0) &&
            (beta_line[beta_count].dc_num != LCHAR) )
	{
	   for( i=0;i<beta_line[beta_count].dc_num;i++)
	   {
	      if( beta_line[beta_count].dc_codes[i] == '|' )
	      {
		 print_line[p_index++] = 27;
		 print_line[p_index++] = 83;
		 print_line[p_index++] = 1;
		 print_line[p_index++] = FX_COMP;
		 print_line[p_index++] = '|';
		 print_line[p_index++] = FX_UNCOMP;
		 print_line[p_index++] = 27;
		 print_line[p_index++] = 84;
	      }
	      else
              {
		 print_line[p_index++] = FX_BS;
		 print_line[p_index++] = beta_line[beta_count].dc_codes[i];
              }
	   }
	}
	/*
	else
	   p_index++;
	*/
    }
    print_line[p_index++] = FX_NL;
    print_line[p_index++] = FX_CR;
    print_line[p_index] = NULL;
    return 1;
}


/*---------------------------------------------------------
 *   FX_PrintChar        07-15-92 MM
 *
 *   Print a character and then wait for an ACK.
 *---------------------------------------------------------
 */
FX_PrintChar( char c )
{
   int i,result,delay;
   union REGS inreg,outreg;

/*
   delay = DELAY;
   result = biosprint(0,c,port_id);
   while(delay-- && !(result & 0x40) )
        result = biosprint(2,0,port_id);
*/
   inreg.h.ah = 0x05;
   inreg.h.dl = c;

   intdos(&inreg,&outreg);

   return ( result=0x40 );
}
