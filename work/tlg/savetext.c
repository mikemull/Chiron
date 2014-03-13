/*===================================================================
 * $Header: C:\MIKE\TLG\CHIRON\RCS\savetext.c 2.1 1994/02/13 19:02:40 mwm Exp mwm $
 *
 * Routines to save betatext to a file.
 *
 * $Log: savetext.c $
 * Revision 2.1  1994/02/13  19:02:40  mwm
 * Fixed bug that was causing file to be reopened on every save.
 *
 * Revision 2.0  1993/09/15  20:15:13  mwm
 * Start of new version
 *
 * Revision 1.1  92/04/06  18:20:28  ROOT_DOS
 * Initial revision
 * 
 * 
 *-------------------------------------------------------------------
 */
#include "tlg.h"
#include "win.h"
#include "work.h"
#include "notice.h"
#include "citation.h"
#include "savetext.h"

static char save_filename[128] = {0} ;
static FILE *sfp;
static BOOL new_file;

/*-------------------------------------------------------------------
 *  SetSaveFile              04.02.92 MM
 *
 *  Set the file name a open file for text save file.
 *------------------------------------------------------------------
*/
void
SetSaveFile( char *filename )
{
    new_file = FALSE;

    if(strlen(filename) == 0)
        strcpy( filename, DEFAULT_SAVE_FILE );

    if ( strcmp( filename, save_filename ) != 0 )
    {
        new_file = TRUE;

        strcpy(save_filename,filename);
    }

    return;
}/*
END of SetSaveFile */



/*-------------------------------------------------------------------
 *  SaveCurrentCit           04.01.92 MM
 *
 *  Write the current citation +/- context lines of betatext to the
 *  indicated file
 *------------------------------------------------------------------
 */
void
SaveCurrentCit( void )
{
        int i,start_line,end_line;
        char *tp;
        char citstr[80];

        if( sfp == NULL)
            sfp = fopen( save_filename, "w" );
        else if (new_file)
        {
            fclose(sfp);
            sfp = fopen( save_filename, "w" );
            new_file = FALSE;
        }

        if( sfp != NULL )
        {
                CitStrfromStruc( WorkCitStruct(), citstr );
                fputs(citstr,sfp);
                fputc('\n',sfp);

                start_line = (Work.line_index >= Work.context) ?
                             (Work.line_index - Work.context) :
                             0;

                end_line = (Work.line_index + Work.context) < Work.line_max ?
                           (Work.line_index + Work.context) :
                           Work.line_max;

                for(i=start_line;
                    i<=end_line;
                    i++)
                {
                        tp = Work.linestart[i];
                        while(!HIBITSET(*tp))
                                fputc((int) *tp++, sfp);

                        fputc('\n',sfp);
                }
                fputs("--------------------------",sfp);
                fputc('\n',sfp);
        }
        fflush(sfp);
}/*
END of SaveCurrentCit */

