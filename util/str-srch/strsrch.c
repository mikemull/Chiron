/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\strsrch.c 2.0 1993/09/18 16:27:24 mwm Exp mwm $
 *
 *   Generic String and Pattern Search routines.
 *
 *   Generic routine to search for a pattern in hunk of text. Routine
 *   uses simple heuristics to determine which search algorithm
 *   to invoke for a given pattern.
 *
 *   BYTE *MatchPattern( BYTE *pattern, BYTE *text, int text_len,
 *                       BYTE *ignore_set)
 *   int  ChangeIgnoreSet( void )
 *
 *   $Log: strsrch.c $
 * Revision 2.0  1993/09/18  16:27:24  mwm
 * Start of new version.
 *
 * Revision 1.5  92/04/16  18:00:18  ROOT_DOS
 * clean up for c++
 * 
 * Revision 1.4  92/02/19  18:47:14  ROOT_DOS
 * added loop to set end_of_patt correctly
 * 
 * Revision 1.3  92/02/18  19:47:37  ROOT_DOS
 * Made ignore set a static within strsrch and added ChangeIngoreSet
 * function.
 * 
 * Revision 1.2  91/09/05  18:09:34  ROOT_DOS
 * Added code to handle BETALETTER attributes.
 * 
 * Revision 1.1  91/09/03  19:31:34  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include "tlg.h"
#include "strsrch.h"
#include "regex.h"

/* The following are accessed by the actual search
 * routines ( BM_Srch(), BYG_Srch(), etc ) as
 * externs
 */
BYTE *beg_of_patt, *end_of_patt;
/**/
char betachars[] = "'ABGDEVZHQIKLMNCOPRSTUFXYW";
char diacrit[] = "()|+/\\=";
/*****/
static char *igset[2] = { diacrit, "\\/=" };
static int     searchtype=IGNORE;


/*-------------------------------------------------------------------
 *   MatchPattern                 04.24.90  GH
 *
 *   Catch-all routine for pattern-matching. Directs the search through
 *   BM_Srch(), BYG_Srch() or strstr() depending on heuristics based
 *   on pattern content.
 *-------------------------------------------------------------------
 */
BYTE *
MatchPattern( BYTE *pattern, BYTE *text, int text_len )
{
        char *dstr;  /* mostly for debugging */
        BYTE *pmatch;

        Debug("%s %s\n",pattern,igset[searchtype]);
        if(dstr = strpbrk((char *)pattern,"#."))
        {
                re_comp( (char *) pattern, igset[searchtype] );
                pmatch =  (BYTE *) re_exec(text);
        }
        else
        {
                if(strlen(pattern) > 2)
                        pmatch = BM_Srch(pattern,text,text_len,
                                         (BYTE *)igset[searchtype] );
                else
                {
                       pmatch = strstr( text, pattern );
                       beg_of_patt = pmatch;
                       end_of_patt = pmatch+1;
                       /*
                       ** Move end_of_patt beyond ignore set characters
                       */
                       while(strchr((char *)igset[searchtype],*end_of_patt))
                                end_of_patt++;
                }
        }

        return( pmatch );
}/*
END of MatchPattern */


/*-------------------------------------------------------------------
 *  ChangeIgnoreSet
 *
 *  Toggles the ignore set type and returns the new type.
 *-------------------------------------------------------------------
 */
int
ChangeIgnoreSet( void )
{
                
        return( searchtype = (searchtype==IGNORE) ? EXACT : IGNORE );
}/*
END of ChangeIgnoreSet */

