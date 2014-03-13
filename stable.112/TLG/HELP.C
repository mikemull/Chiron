
/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\help.c 2.0 1993/09/18 16:25:05 mwm Exp mwm $
 *
 *  Help functions.
 *
 *  $Log: help.c $
 * Revision 2.0  1993/09/18  16:25:05  mwm
 * Start of new version.
 *
 *
 *-------------------------------------------------------------------
 */

#include <stdio.h>
#include <alloc.h>
#include <string.h>
#include "tlg.h"
#include "win.h"
#include "help.h"
#include "notice.h"
#include "greek.h"

long m0,m1,m2,m3,m4;
enum { KEYBOARD, PATTERN, COMMAND };

#ifdef DEBUG_CODE

void help( int d ) {};
void KeyboardHelp( void ) {};
void PatternHelp( void ) {};
void CommandHelp( void ) {};

#else


/*-------------------------------------------------------------------
 *  help                     02.10.92 MM
 *
 *  Help function.  Displays help screens based on help_type
 *-------------------------------------------------------------------
 */
void
help( int help_type )
{
        switch( help_type )
        {
                case KEYBOARD:
                        KeyboardHelp();
                        break;
                case PATTERN:
                        PatternHelp();
                        break;
                case COMMAND:
                        CommandHelp();
                        break;
        }
}/*
END of help */


/*-------------------------------------------------------------------
 *  KeyboardHelp                02.10.92 MM
 *
 *  Displays Latin-Greek keyboard mapping.
 *-------------------------------------------------------------------
 */
 void
 KeyboardHelp( void )
 {
        int i,j;
        char ls[2];
        int x,y;
        static char *keyline[] = { "()=/\\",
                                   "qwertyuiop",
                                   "asdfghjkl",
                                   "zxcvbnm"
                                 };

        ls[1] = '\0';

        makeNotice( "<      Greek Letter Keyboard Assignments      >",14,NOWAIT );

        for(j = 0; j< 4; j++)
        {
            y = 40+((j*2)*LINEHEIGHT)+(j*5);
            for(i = 0; i< strlen(keyline[j]); i++ )
            {
              ls[0] = keyline[j][i];
              x = 200+(2*i*CHARWIDTH);
              outtextxy( x, y, ls );
              PrintGreekString( x, y+LINEHEIGHT,ls);
            }
        }

        WaitForKey();

	noticeDown();
}/*
END of KeyboardHelp */


/*-------------------------------------------------------------------
 *  PatternHelp                02.10.92 MM
 *
 *  Displays info about regular expressions and combinations
 *-------------------------------------------------------------------
 */
void
PatternHelp( void )
{
        int i;

        makeNotice( "<               Searching for Patterns            >",
                   14,NOWAIT);
        addToNotice("");
        addToNotice(" .  matches any letter                             ");
        addToNotice("    For example AB.C matches ABDC, ABEC, ABFC, etc.");
        addToNotice("");
        addToNotice(" #  matches 0 or more occurences of the previous   ");
        addToNotice("    character.  For example ABC# matches AB, ABC,  ");
        addToNotice("    ABCC, ABCCC, etc.                              ");
        addToNotice("");
        addToNotice(".#  matches any sequence of characters.            ");
        addToNotice("    For example, AB.# matches AB, ABXY, ABVQD, etc.");
        addToNotice("");
        addToNotice("&   Pattern1 and Pattern2. For example AB & CD     ");
        addToNotice("%   Pattern1 or Pattern2.  For example WX % YZ     ");

        WaitForKey();

	noticeDown();
}/*
END of PatternHelp  */



/*-------------------------------------------------------------------
 *  CommandHelp                02.10.92 MM
 *
 *  Shows info about function key mappings
 *-------------------------------------------------------------------
 */
void
CommandHelp( void )
{
        makeNotice( "                 Command Keys                   ",
                   13,NOWAIT );
        addToNotice("");
        addToNotice("      F1     Find All Matches in Word List      ");
        addToNotice("      F2     Find All Works That Contain Pattern");
        addToNotice("      F3     Find Authors                       ");
        addToNotice("      F4     Get Text For Work                  ");
        addToNotice("      F5     Get Text For Next Work             ");
        addToNotice("      F6     Go To Next Match                   ");
        addToNotice("      F7     Go To Citation                     ");
        addToNotice("      F8     Print Text at Current Citation     ");
        addToNotice("      F9     Save Text at Current Citation      ");
        addToNotice("      F10    ------------------------------     ");
        addToNotice(" Press any key to continue                      ");

        WaitForKey();
        noticeDown();
}/*
END of CommandHelp */

#endif /* DEBUG */
