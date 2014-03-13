/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\author.c 2.0 1993/09/15 20:36:09 mwm Exp mwm $
 *
 *  Functions related to the currently selected author.
 *  $Log: author.c $
 * Revision 2.0  1993/09/15  20:36:09  mwm
 * Start of new version.
 *
 * Revision 1.1  92/04/12  16:16:47  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include "tlg.h"
#include "author.h"
#include "file.h"

static Author CurrAuth;

/*-------------------------------------------------------------------
 *  NewAuthor                03.15.92 MM
 *
 *  Set up for a new author.  This includes opening the authors IDT
 *  and Text files..
 *-------------------------------------------------------------------
 */
void
NewAuthor( int num )
{
	char file[40];

        CloseAuthor();
        CurrAuth.number = num;
        sprintf(file,"TLG%04d.IDT", num);
        if( (CurrAuth.idtfile = OpenTLG(file)) == NULL )
	{
		fprintf(stderr,"Can't open idt file %s",file);
	}

        sprintf(file,"TLG%04d.TXT", num);
        if( (CurrAuth.txtfile = OpenTLG(file)) == NULL )
	{
		fprintf(stderr,"Can't open text file %s",file);
	}
}/*
END of NewAuthor */


/*-------------------------------------------------------------------
 *  CloseAuthor              03.15.92 MM
 *
 *  Close the IDT and Text files for the current author.
 *-------------------------------------------------------------------
 */
void
CloseAuthor( void )
{
        if(CurrAuth.idtfile) fclose(CurrAuth.idtfile);
        if(CurrAuth.txtfile) fclose(CurrAuth.txtfile);
        CurrAuth.txtfile=NULL;
        CurrAuth.idtfile=NULL;
}/*
END of CloseAuthor */



FILE *IdtFile() { return CurrAuth.idtfile; }
FILE *TextFile() { return CurrAuth.txtfile; }

