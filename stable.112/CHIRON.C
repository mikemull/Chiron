/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\chiron.c 2.2 1994/02/13 18:52:10 mwm Exp mwm $                 05.19.90
 *
 *  Main file for CHIRON program.  This file contains the main
 *  routine, which builds the interface, then calls KeyLoop.
 *  The global interface objects are declared here also.
 *
 *      main(int argc,char **argv)
 *      int ExitProc(void)
 *      void Debug(char *fmt,...)
 *
 *  $Log: chiron.c $
 * Revision 2.2  1994/02/13  18:52:10  mwm
 * Macrofied default text saving file name
 *
 * Revision 2.1  1993/09/14  20:59:55  mwm
 * Start of new version
 *
 *
 * Revision 1.10  92/03/29  12:45:59  ROOT_DOS
 * added context entry.
 * 
 * Revision 1.9  92/03/15  15:30:52  ROOT_DOS
 * Added function key invocation of interface code and corresponding help
 * 
 * Revision 1.8  92/03/07  17:27:23  ROOT_DOS
 * More interface tweaking
 * 
 * Revision 1.7  92/03/02  17:44:11  ROOT_DOS
 * Changed search button to find, Get Text button to text
 * Added new menu options
 * 
 * Revision 1.6  92/02/24  17:03:15  ROOT_DOS
 * Added menu to help button.
 * 
 * Revision 1.5  92/02/09  14:17:14  ROOT_DOS
 * Added prompt to ExitProc.
 * 
 * Revision 1.4  91/12/02  18:36:14  ROOT_DOS
 * Minor changes to menu items
 * 
 * Revision 1.3  91/08/19  19:31:51  ROOT_DOS
 * Changed return type of ExitProc from void to int
 * 
 * Revision 1.2  91/08/19  19:15:17  ROOT_DOS
 * Added keywords, updated comments
 * 
 *
 *-------------------------------------------------------------------
 */
#include <dos.h>
#include "tlg.h"
#include "callback.h"
#include "info.h"
#include "win.h"
#include "chiron.h"
#include "button.h"
#include "list.h"
#include "entry.h"
#include "txtwin.h"
#include "notice.h"
#include "tlgmem.h"
#include "init.h"
#include "editkey.h"
#include "savetext.h"
#include "intr.h"

/* Global Interface Object Handles */
Handle help_button,search_button,text_button,info_button,quit_button,
       print_button;
Handle word_entry,author_entry,work_entry,cit_entry,context_entry,sfile_entry;
Handle txtwin;

BOOL debug_chiron=FALSE;
FILE *cdebug;

/*-------------------------------------------------------------------
 *  main                     05.19.90
 *
 *  The main routine.  Initializes, puts up interface, then calls
 *  KeyLoop to go into infinite loop to get keys.
 *
 *-------------------------------------------------------------------
 */
int
main( int argc, char **argv )
{
        BOOL interface_test=FALSE;
        extern help(),ExitProc();
	extern void WordInfo();

        /*
        **  Turn on debugging if requested
        */
        if( (argc>1) && argv[1][1] == 'D')
        {
                debug_chiron = TRUE;
                if(!(cdebug=fopen("chiron.dbg","w")))
                {
                        fputs("Cant open debug file",stderr);
                        exit(1);
                }
        }

        if( (argc>1) && argv[1][1] == 'I')
                interface_test = TRUE;

        /*
        **  Set interrupt catcher
        signal( SIGINT, CatchSignal );
        */
        ctrlbrk( CatchSignal );

        ClockSetup();

	initWin();

        BindFuncKey( F1, WordListOption );
        BindFuncKey( F2, FindWorksWithPattern );
        BindFuncKey( F3, FindApproxAuthors );
        BindFuncKey( F4, GetTextForWord );
        BindFuncKey( F5, SetNextWork );
        BindFuncKey( F6, GotoNextMatch );
        BindFuncKey( F7, GoToCitation );
        BindFuncKey( F8, PrintThisCit );
        BindFuncKey( F9, SaveCurrentCit );



	help_button = createOb( NULL,ButtonObject,
				   W_X, 1,
				   W_Y, 1,
				   B_LABEL, "[H]elp",
				   B_KEY, 'H',
                                   B_FUNC, NULL,
                                   L_TYPE,LATIN,
                                   L_LIST,
                                        "Keyboard",
                                        "Patterns",
                                        "Commands",
                                        (char *) NULL,
                                   L_FUNC, HelpCallback,
				   END_LIST);

	search_button = createOb( NULL,ButtonObject,
                                   W_X, 60,
				   W_Y, 1,
                                   B_LABEL, "[F]ind",
                                   B_KEY, 'F',
                                   B_FUNC, NULL,
                                   L_TYPE,LATIN,
                                   L_LIST,
                                        "Works that contain pattern [F2]",
                                        "Pattern in word list       [F1]",
                                        "All Matches in All Text",
                                        "Authors                    [F3]",
                                        "Ignore Diacriticals",
                                        (char *) NULL,
                                   L_FUNC, GetWordList,
				   END_LIST);

	text_button =  createOb( NULL,ButtonObject,
                                   W_X, 119,
				   W_Y, 1,
                                   B_LABEL, "[T]ext",
                                   B_KEY, 'T',
                                   B_FUNC, NULL,
                                   L_LIST,
                                        "Get current work      [F4]",
                                        "Get next work in list [F5]",
                                        "Search",
                                        "Go To Citation        [F7]",
                                        "Set Context          Alt-N",
                                        (char *) NULL,
                                   L_FUNC, TextListCallback,
				   END_LIST);

	info_button =  createOb( NULL,ButtonObject,
                                   W_X, 178,
				   W_Y, 1,
				   B_LABEL, "[I]nfo",
				   B_KEY, 'I',
                                   B_FUNC, NULL,
                                   L_TYPE,LATIN,
                                   L_LIST,
                                        "Canon Info",
                                        "Citation Info",
                                        (char *)NULL,
                                   L_FUNC,Information,
				   END_LIST);

        print_button =  createOb( NULL,ButtonObject,
                                   W_X, 237,
				   W_Y, 1,
                                   B_LABEL, "[O]utput",
                                   B_KEY, 'O',
                                   B_FUNC, NULL,
                                   L_TYPE,LATIN,
                                   L_LIST,
                                        "Print Current Citation [F8]",
                                        "Print All Matches",
                                        "Print Canon Information",
                                        "Save Current Citation  [F9]",
                                        "Save All Matches",
                                        "Save Canon Info",
                                        (char *)NULL,
                                   L_FUNC,OutputCallback,
				   END_LIST);

	quit_button = createOb( NULL,ButtonObject,
                                   W_X, 312,
				   W_Y, 1,
				   B_LABEL, "[Q]uit",
				   B_KEY, 'Q',
                                   B_FUNC,ExitProc,
				   END_LIST);

        /*
        **  Put up bogus button to advertise chiron
        */
               (void)createOb( NULL,ButtonObject,
                                    W_X,371,
                                    W_Y,1,
                                    B_LABEL,"[      Chiron Version 1.1       ]",
                                    END_LIST );

	word_entry = createOb( NULL, EntryObject,
				  W_X,	1,
                                  W_Y,  15,
                                  B_LABEL, "[P]attern",
                                  B_KEY, 'P',
				  B_FUNC, NULL,
                                  L_FUNC, SetWord,
                                  L_TYPE,GREEK,
                                  E_WIDTH, 25,
				  E_TYPE,GREEK,
				  END_LIST);

	author_entry = createOb( NULL, EntryObject,
                                  W_X,  312,
				  W_Y,	15,
				  B_LABEL, "[A]uthor",
				  B_KEY, 'A',
				  B_FUNC, NULL,
				  L_FUNC, SetAuthor,
                                  E_WIDTH, 27,
				  E_TYPE, LATIN,
				  END_LIST);

	work_entry = createOb( NULL, EntryObject,
				  W_X,	1,
                                  W_Y,  33,
                                  B_LABEL, "[W]ork",
                                  B_KEY, 'W',
				  B_FUNC, NULL,
				  L_FUNC, SetWork,
                                  E_WIDTH, 36,
				  E_TYPE, LATIN,
				  END_LIST);

        cit_entry = createOb( NULL, EntryObject,
                                  W_X,  389,
                                  W_Y,  33,
                                  B_LABEL, "[C]it.",
                                  B_KEY, 'C',
				  B_FUNC, NULL,
                                  L_FUNC, NULL,
                                  E_WIDTH, 20,
				  E_TYPE, LATIN,
				  END_LIST);

        context_entry = createOb( NULL, EntryObject,
                                  W_X,  200,
                                  W_Y,  80,
                                  B_LABEL, "Co[n]text",
                                  B_KEY, 'N',
				  B_FUNC, NULL,
                                  L_FUNC, NULL,
                                  E_WIDTH, 15,
                                  E_VISIBLE,FALSE,
                                  E_FIXED,FALSE,
                                  E_TRIGGER,SetNewContext,
                                  E_TYPE, NUMERIC,
				  END_LIST);

        sfile_entry = createOb( NULL, EntryObject,
                                  W_X,  100,
                                  W_Y,  80,
                                  B_LABEL, "[S]ave Filename",
                                  B_KEY, 'S',
				  B_FUNC, NULL,
                                  L_FUNC, NULL,
                                  E_WIDTH, 30,
                                  E_VISIBLE,FALSE,
                                  E_FIXED,FALSE,
                                  E_TRIGGER,SaveTextTrigger,
                                  E_TYPE, LATIN,
				  END_LIST);


	txtwin = createOb( NULL, TxtWinObject,
				    W_X, 1,
                                    W_Y, 51,
                                    T_COLS,70,
				    T_LINES,11,
				    END_LIST);

        EntrySetValue( context_entry, "1" );
        SetNewContext();
        EntrySetValue( sfile_entry, "chrnsave.btx" );
        SetSaveFile( DEFAULT_SAVE_FILE );
        EntrySelect(word_entry);

        if(!interface_test)
                Initialize();

	keyLoop();
        return 1;
}/*
END of main
*/

/*-------------------------------------------------------------------
 *  ExitProc                 05.18.91 MM
 *
 *  Exit procedure called when Alt-Q is typed.   Frees all data
 *  structures (sanity check) and shuts off graphics.
 *  05.18.91 MM
 *-------------------------------------------------------------------
 */
int
ExitProc(void)
{
	extern int alloc_count;
        int do_quit=FALSE;

        do_quit = makeNotice("Do you really want to quit",2,YORN);
        noticeDown();

        if( do_quit )
        {
                FreeAll();
                closegraph();
                ClockShutdown();
                exit(0);
        }
        return 1;
}/*
END of ExitProc */


/*-------------------------------------------------------------------
 *  CatchSignal
 *
 *  Should trap ^c signal
 *-------------------------------------------------------------------
 */
int
CatchSignal( void )
{
/*        ExitProc(); */
        return(0);
}/*
END of Catch Signal */


/*-------------------------------------------------------------------
 *  Debug                    08.19.91 MM
 *
 *  Function to write debugging messages
 *-------------------------------------------------------------------
 */
void
Debug(char *fmt,...)
{
        va_list vars;

        if(debug_chiron)
        {
          va_start(vars,fmt);
          vfprintf(cdebug,fmt,vars);
          va_end(vars);
        }
}/*
END of Debug */

