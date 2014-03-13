/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\callback.c 2.3 1994/02/13 19:06:14 mwm Exp mwm $
 *
 *  These are the top level routines, usually called indirectly by
 *  the interface manager.  The functions here get the author and
 *  work lists and get the associated text.
 *
 *  $Log: callback.c $
 * Revision 2.3  1994/02/13  19:06:14  mwm
 * Fixed "find all in all".
 *
 * Revision 2.2  1993/11/21  16:27:38  mwm
 * Changed to new word list structure.
 *
 * Revision 2.1  1993/08/31  20:47:12  mwm
 * Fix of author list generation for D-Rom
 *
 * Revision 2.0  1993/08/31  20:42:35  mwm
 * start of new version
 *
 * Revision 1.25  92/06/23  18:07:53  ROOT_DOS
 * Several minor changes
 * 
 * Revision 1.24  92/05/12  19:28:20  ROOT_DOS
 * Numerous changes
 * 
 * Revision 1.23  92/04/07  17:26:58  ROOT_DOS
 * First crack at FindAllMatchesInText
 * 
 * Revision 1.22  92/04/06  09:07:52  ROOT_DOS
 * Added SaveFileTrigger function and removed Citation calc. from
 * SetNewCit
 * 
 * Revision 1.21  92/03/28  17:37:31  ROOT_DOS
 * Added SetContext functionality
 * 
 * Revision 1.20  92/03/15  15:28:17  ROOT_DOS
 * Added function key invocation of interface code and corresponding help
 * 
 * Revision 1.19  92/03/11  21:22:42  ROOT_DOS
 * Added get next work functionality
 * 
 * Revision 1.18  92/03/10  20:06:11  ROOT_DOS
 * Added code to search current work
 * 
 * Revision 1.17  92/03/07  15:01:28  ROOT_DOS
 * Changed print callback to output callback
 * Added print canon
 * 
 * Revision 1.16  92/03/04  18:49:21  ROOT_DOS
 * Added logic to check for no author matches
 * 
 * Revision 1.15  92/03/04  16:38:08  ROOT_DOS
 * Added TextListCallback
 * Added code to make author list from approx author name
 * Added author_is_set and work_is_set flags
 * 
 * Revision 1.14  92/03/01  14:27:31  ROOT_DOS
 * Removed stuff to open and close text files. This is now in
 * author.c code.
 * 
 * Revision 1.13  92/02/24  17:25:00  ROOT_DOS
 * Added help callback
 * Added notice to print number of matches
 * 
 * Revision 1.12  92/01/29  18:59:09  ROOT_DOS
 * Changes regarding complete re-write of AW list code
 * 
 * Revision 1.11  92/01/18  14:09:41  ROOT_DOS
 * Changed NewWork call arguments
 * 
 * Revision 1.10  91/12/02  18:21:25  ROOT_DOS
 * Added code to handle 0 word matches, and to Parse word list matches
 * 
 * Revision 1.9  91/12/02  18:04:37  ROOT_DOS
 * Removed references to global "searchtype"
 * 
 * Revision 1.8  91/12/02  17:12:33  ROOT_DOS
 * Changed printing routine
 * 
 * Revision 1.7  91/12/01  19:30:37  ROOT_DOS
 * Removed code to free word list, replace with FreeWordList
 * 
 * Revision 1.6  91/11/25  20:01:20  ROOT_DOS
 * *** empty log message ***
 * 
 * Revision 1.5  91/11/24  11:32:54  ROOT_DOS
 * Minor change to logic for searching and displaying text.
 * 
 * Revision 1.4  91/10/21  18:30:49  ROOT_DOS
 * Made changes to accomodate new Work handler code
 * 
 * Revision 1.3  91/09/15  14:43:13  ROOT_DOS
 * Changed GetWordList for new parsing routines
 * 
 * Revision 1.2  91/09/03  18:53:30  ROOT_DOS
 * Added code to handle exact/ingore matches in GetWordList
 * 
 * Revision 1.1  91/08/20  18:14:46  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include "tlg.h"
#include <string.h>
#include <stdlib.h>
#include "ptree.h"
#include "snlist.h"
#include "win.h"
#include "chiron.h"
#include "callback.h"
#include "makelist.h"
#include "awlst.h"
#include "entry.h"
#include "txtwin.h"
#include "button.h"
#include "list.h"
#include "notice.h"
#include "text.h"
#include "tlgmem.h"
#include "wlist.h"
#include "print.h"
#include "help.h"
#include "strsrch.h"
#include "work.h"
#include "author.h"
#include "config.h"
#include "atab.h"
#include "savetext.h"
#include "intr.h"

/* Statics
*/
#ifdef DEBUG_CODE
#define MN 20
#define MAX_AUTH 20
#else
#define MN 403  /*  Author 2062 contains 402 works */
#endif

static BOOL     word_is_set=FALSE;  /*Flag                              */
static BOOL     author_is_set=FALSE;/*Flag                              */
static BOOL     work_is_set=FALSE;  /*Flag                              */
static BOOL     work_is_open=FALSE; /*Flag                              */
static BOOL     text_search=FALSE;  /*Flag                              */
static unsigned *sn_list;           /*Pointer to SN list                */
static char     **match_list=NULL;  /*List of matched word names        */
static char     *author_list[MAX_AUTH];/*List of author names           */
static char     *work_list[MN];     /*List of work names                */
static char     *filter_wlist[MN];  /*Filtered list of work names       */
static int      block_list[MN];     /*List of block numbers             */
static int      filter_blist[MN];   /*Filtered block start list         */
static int      filter_elist[MN];   /*Filtered block end list           */
static int      work_nums[MN];      /*List of work numbers              */
static int      filter_wnums[MN];   /*Filter work numbers               */
static int      num_f_works;        /*Number of filtered works          */
static int      work_index;         /*Index of current work             */
static int      auth_index;         /*Index of current author           */

static enum PRINTTYPE {PRINTCIT,PRINTALL,PRINTCANON,SAVECIT,SAVEALL,SAVECANON};
static BOOL     save_mode = SAVECIT;     /*Flag, basically                   */

/*  Macros
*/
#define AUTHOR  AWAuthor( CurrentAWIndex() )
#define WORK    AWWork( CurrentAWIndex() )

/*-------------------------------------------------------------------
 *  GetWordList            05.20.90
 *
 *  Routine called by interface.  Creates and displays the matched
 *  words
 *  10.15.90 MM
 *-------------------------------------------------------------------
 */
int
GetWordList( int selection_num, char *selection)
{
        int i,search_type,author_count;
        enum menu_opts { WORKS,WORDLIST,ALLTEXT,AUTHORS,DIGNORE };
        static char *smlist[2] = { "Ignore Diacriticals",
                                   "Include Diacriticals" };


        switch( selection_num )
        {
        case DIGNORE:
                search_type = ChangeIgnoreSet();
                ChangeListItem((Handle) &(((Button *)(*search_button))->list),
                                DIGNORE,smlist[search_type]);
        break;

        case AUTHORS:
                FindApproxAuthors();
        break;

        case WORDLIST:
                WordListOption();
        break;

        case WORKS:
                FindWorksWithPattern();
        break;

        case ALLTEXT:
                FindAllMatchesInText();
        break;
        default:
                SYSTEMCRASH(EXIT_UNDEF);  /* MAJOR BADNESS */
        }

        return(word_is_set);
}/*
END of GetWordList
*/


/*-------------------------------------------------------------------
 *  FindWorksWithPattern
 *
 *  Find all the works that contain the currently set pattern. This
 *  invokes the creation of the AWlist from a serial number list
 *-------------------------------------------------------------------
 */
void
FindWorksWithPattern( void )
{
        char match_pattern[80],message[180];

        FreeAll();
        EntryGetValue(word_entry,match_pattern);
        Debug("GetWordList: match_pattern %s",match_pattern);
        if( ! Parse(match_pattern) )
        {
                makeNotice("     Pattern is incorrect     ",2,WAIT);
                noticeDown();
        }
        else
        {
                if( sn_list = SNListFromPTree( ParseTree() ) )
                {
                      if ( !CreateAWList(sn_list) )
                      {
                              word_is_set=FALSE;
                              makeNotice("     No matches found    ",2,WAIT);
                              noticeDown();
                      }
                      else
                      {
                              sprintf(message,
                                      "         Total = %ld        ",
                                      MatchTotal() );
                              makeNotice(message,2,WAIT);
                              noticeDown();
                              word_is_set=TRUE;
                              authorsForWord();
                              EntryMakeList( author_entry );
                      }
                }
                else
                {
                        sprintf(message,"No matches found for pattern %s",
                                match_pattern);
                        makeNotice(message,2,WAIT);
                        noticeDown();
                }
                if(word_is_set)
                {
                      work_is_set = FALSE;
                      work_is_open =FALSE;
                      text_search = TRUE;
                }
        }/* end of else does parse */
}/*
END of FindWorksWithPattern */


/*-------------------------------------------------------------------
 *  WordListOption           09.14.91 MM
 *
 *  This is done of only the word list is to be searched. See above.
 *-------------------------------------------------------------------
 */
void
WordListOption( void )
{
        int i;
        long num_match;
        char match_pattern[80];

        FreeAll();
        EntryGetValue( word_entry, match_pattern );
        if(strpbrk(match_pattern,"%&~"))
        {
            word_is_set = FALSE;
            makeNotice("Can not search for combinations in word list",
                        2,WAIT);
            noticeDown();
        }
        else
        {
            ClockToggle(TRUE);
            num_match=CreateWordList(match_pattern,TRUE);
            ClockToggle(FALSE);
            if(num_match > 0)
            {
                 match_list=(char **)
                       TLGmalloc((match_count+1)*sizeof(char *));

                 for(i=0;i<match_count;i++)
                       match_list[i] = match_words[i]->word_text->text;
                 match_list[i] = NULL;

                 EntrySetList( word_entry, match_list, match_count );
                 EntryMakeList( word_entry );
                 word_is_set = text_search = TRUE;
            }
            else if (num_match == 0)
            {
                 makeNotice("   No matches found in word list   ",
                        2,WAIT);
                 noticeDown();
            }
            else
            {
                 makeNotice(" <Escape> pressed. Word list search aborted ",
                        2,WAIT);
                 noticeDown();
            }
        }
}/*
END of WordListOption */


/*-------------------------------------------------------------------
 *  FindApproxAuthors        03.15.92 MM
 *
 *  Find author names similar to that in the author field.  Calls
 *  GetAuths to get author names and numbers, and then invokes the
 *  creation of the AW list from the author numbers
 *-------------------------------------------------------------------
 */
void
FindApproxAuthors( void )
{
        int author_count;
        int *temp_author_nums;
        char match_pattern[80];

        work_is_set = FALSE;
        text_search = FALSE;

        FreeAll();
        temp_author_nums = (int *) TLGmalloc( MN * sizeof(int) );
        makeNotice("    Searching For Authors    ",2,NOWAIT);

        EntryGetValue(author_entry,match_pattern);
        Tcl_ValidateAllMemory(__FILE__,__LINE__);
        if(author_count = GetAuths(match_pattern,author_list,
                                        temp_author_nums,MN))
        {
                CreateAWListFromAuth( temp_author_nums, author_count );
                TLGfree(temp_author_nums);
                noticeDown();
                EntrySetValue( author_entry, author_list[0] );
                EntrySetList( author_entry, author_list, author_count);
                EntryMakeList( author_entry );
        }
        else
        {
                noticeDown();
                TLGfree(temp_author_nums);
                makeNotice("No matching author names found",2,WAIT);
                noticeDown();
        }
}/*
END of FindApproxAuthors */



/*-------------------------------------------------------------------
 *  FindAllMatchesInText     04.06.92 MM
 *
 *  Goes through all of the TLG text looking for matches (yipes!)
 *-------------------------------------------------------------------
 */
 void
 FindAllMatchesInText( void )
 {
        int i;
        char match_pattern[80];

        FreeAll();
        EntryGetValue(word_entry,match_pattern);
        Debug("FindAllMatchesInText: match_pattern %s",match_pattern);
        if( ! Parse(match_pattern) )
        {
                makeNotice("     Pattern is incorrect     ",2,WAIT);
                noticeDown();
        }
        else if( !CreateFullAWList() )
        {
                makeNotice("     Insufficient Memory To Do This    ",2,WAIT);
                noticeDown();
        }
        else
        {
                word_is_set = TRUE;
                text_search = TRUE;
                authorsForWord();
                SetAWIndex(0);
                NewAuthor(AUTHOR);
                WorksForAuthor();
                work_is_set = TRUE;
                if(OpenCurrentWork())
                        displayText( txtwin );
                else
                        GotoNextMatch();
        }
}/*
END of FindAllMatchesInText */


/*-------------------------------------------------------------------
 *  GetTextForWord           05.20.90 MM
 *
 *  Routine called by interface that does a little housekeeping, the
 *  calls GetText.
 *
 *-------------------------------------------------------------------
 */
void
GetTextForWord()
{
        if( work_is_set )
        {
                if(OpenCurrentWork())
                        displayText( txtwin );
                else
                {
                        makeNotice("Can not find match or open work",1,WAIT);
                        noticeDown();
                }
        }
        else
        {
                makeNotice("     No work selected yet     ",1,WAIT);
                noticeDown();
	}

}/*
END of GetTextForWord
*/



/*-------------------------------------------------------------------
 *  TextListCallback         10.17.90 MM
 *
 *  Callback for text button list.
 *-------------------------------------------------------------------
 */
void
TextListCallback( int selection_num, char *selection )
{
        char match_pattern[80];
        enum text_opts { OPENCURR, OPENNEXT, SEARCHCURR, GOTOCIT, SETCONTEXT };


        switch( selection_num )
        {
                case OPENCURR:
                GetTextForWord();
                break;

                case OPENNEXT:
                SetNextWork();
                break;

                case SEARCHCURR:
                EntryGetValue(word_entry,match_pattern);
                if( ! Parse(match_pattern) )
                {
                    makeNotice("     Pattern is incorrect     ",2,WAIT);
                    noticeDown();
                }
                else
                {
                        word_is_set = text_search = TRUE;
                        GetTextForWord();
                        text_search = FALSE;
                }
                break;

                case GOTOCIT:
                GoToCitation();
                break;

                case SETCONTEXT:
                SetAttributes(context_entry,E_VISIBLE,TRUE,END_LIST);
                break;
        }

}/*
END of TestListCallback  */



/*-------------------------------------------------------------------
 *  GoToCitation             04.09.92 MM
 *
 *  Everything needed to go to the citation in the cit entry field.
 *-------------------------------------------------------------------
 */
void
GoToCitation( void )
{
        char match_pattern[80];
        int i;

        if(work_is_set)
        {
                EntryGetValue(cit_entry,match_pattern);

                makeNotice("  Looking for citation  ", 2, NOWAIT );
                if( !WorkGotoCit( match_pattern ) )
                {
                        noticeDown();
                        makeNotice("  Can not find this citation  ",2,WAIT);
                        noticeDown();
                }
                else
                {
                        noticeDown();
                        displayText(txtwin);
                }
        }
        else
        {
                makeNotice("     No work specified yet     ",2,WAIT);
                noticeDown();
        }
}/*
END of GoToCitation */


/*-------------------------------------------------------------------
 *  GoToNextMatchCitation    04.07.92 MM
 *
 *  Skip to the next citation that corresponds to a match.
 *-------------------------------------------------------------------
 */
BOOL
GoToNextMatchCitation()
{
        int is_next_match=TRUE,list_size;

        list_size = AWSize() - 1;

        if(!NextMatch())
        {
            do
            {
                is_next_match = OpenNextWork();

                if(!text_search && is_next_match)
                     is_next_match = SearchWork();

            } while(!is_next_match && (CurrentAWIndex() < list_size) );
        }

        return(is_next_match);
}/*
END of GoToNextMatchCitation */



/*-------------------------------------------------------------------
 *  OpenCurrentWork          04.07.92 MM
 *
 *  Do the actions neccessary to open work currently pointed to by
 *  the work index
 *-------------------------------------------------------------------
 */
BOOL
OpenCurrentWork( void )
{
        int start_block,end_block;

        start_block = filter_blist[work_index];
        end_block = filter_elist[work_index];

        work_is_open =
           NewWork(TextFile(),filter_wlist[work_index],AUTHOR,WORK,start_block,
                   end_block,text_search);

        return(work_is_open);
}/*
END of OpenCurrentWork */



/*-------------------------------------------------------------------
 *  OpenNextWork             03.15.92 MM
 *
 *  Do the actions neccessary to open the next work in the AW list
 *-------------------------------------------------------------------
 */
BOOL
OpenNextWork( void )
{
        int old_auth,status=TRUE;

        old_auth = AuthorAWIndex();
        if( SetAWIndex( CurrentAWIndex() + 1 ) )
        {
                if( old_auth != AuthorAWIndex() )
                {
                        auth_index++;
                        NewAuthor( AUTHOR );
                        GenerateWorkLists();
                }
                else
                        work_index++;

                status = OpenCurrentWork();
        }
        else
                status = FALSE;

        return( status );
}/*
END of OpenNextWork */



/*-------------------------------------------------------------------
 *  SetNextWork             03.15.92 MM
 *
 *  Open the next work and reset the author and work fields.
 *-------------------------------------------------------------------
 */
void
SetNextWork( void )
{
        if(OpenNextWork())
        {
                EntrySetValue(author_entry,
                        author_list[auth_index] );
                EntrySetValue(work_entry,filter_wlist[work_index]);
        }
        else
        {
                makeNotice("     No more works or authors    ",2,WAIT);
                noticeDown();
        }
}/*
END of SetNextWork */



/*-------------------------------------------------------------------
 *  GotoNextMatch
 *
 *  Do the actions neccessary to go to the next match whether it
 *  is in this work on the next.
 *-------------------------------------------------------------------
 */
void
GotoNextMatch( void )
{
    if(work_is_set && text_search )
    {
        if( GoToNextMatchCitation() )
        {
            EntrySetValue(author_entry,author_list[auth_index] );
            EntrySetValue(work_entry,filter_wlist[work_index]);
            displayText(txtwin);
            SetNewCit();
        }
        else
        {
            makeNotice( "       No More Matches       ",2,WAIT);
            noticeDown();
        }
    }
    else
    {   if (!work_is_set)
           makeNotice("    No work specified yet     ",2,WAIT);
        else
           makeNotice("    No pattern specified yet     ",2,WAIT);

        noticeDown();
    }
}/*
END of GotoNextMatch */



/*-------------------------------------------------------------------
 *  SetWord                  10.17.90 MM
 *
 *  Callback for word entry list.  Creates an AW list for the
 *  selected word.  Author list made of AW list not null.
 *-------------------------------------------------------------------
 */
void
SetWord( int selection_num, char *selection )
{
        static char match_pattern[80];

        strcpy(match_pattern,selection);
        EntrySetValue( word_entry, selection );
        Parse( match_pattern );

        sn_list=CreateSNList(&match_words[selection_num],1 );
        if ( !CreateAWList(sn_list) )
        {
              word_is_set=FALSE;
        }
        else
        {
              word_is_set=TRUE;
              author_is_set = FALSE;
              work_is_set = FALSE;
              work_is_open = FALSE;
              authorsForWord();
              EntryMakeList( author_entry );
        }
}/*
END of SetWord
*/

/*-------------------------------------------------------------------
 *  AuthorsForWord	     09.13.90 MM
 *
 *  Given an expression a list of authors and works that contain
 *  matches is generated.  Then a list of author names is generated
 *  for display in the author entry field.
 *-------------------------------------------------------------------
 */
void
authorsForWord()
{
	unsigned author_count;

        if( author_list[0] !=NULL )
	{
                freeNameList( author_list );
                EntrySetList( author_entry, NULL, 0);
                EntrySetValue( author_entry, "");
		author_list[0] = NULL;
        }
        author_count=createAuthorNameList( author_list);
        auth_index = 0;
        EntrySetValue( author_entry, author_list[0] );
        EntrySetList( author_entry, author_list, author_count);
}/*
END of AuthorForWord
*/

/*-------------------------------------------------------------------
 *  SetAuthor		     09.13.90 MM
 *
 *  Sets the current author in the entry field (called by selection
 *  in entry list).  Sets the AW list pointer to the beginning of
 *  that author, and then calls GetWorks and FilterWorks to generate
 *  the list of works for the author and the works in which the
 *  expression is contained.
 *  09.13.90 MM
 *-------------------------------------------------------------------
 */
void
SetAuthor( int selection_num, char *selection )
{
        author_is_set = TRUE;
        work_is_set =FALSE;
        work_is_open =FALSE;
        auth_index = selection_num;

        EntrySetValue( author_entry, selection );
        SetAuthorIndex( selection_num );
        makeNotice("               Opening files for:              ",3,NOWAIT);
        addToNotice(selection);
        NewAuthor( AUTHOR );
        noticeDown();
        WorksForAuthor();
        EntryMakeList( work_entry );
}/*
END of SetAuthor
*/


/*-------------------------------------------------------------------
 *  WorksForAuthor           03.11.92 MM
 *
 *  Generate a new work list for the current author
 *-------------------------------------------------------------------
 */
void
WorksForAuthor( void )
{
        makeNotice("Making Work List",2,NOWAIT);

        GenerateWorkLists();
        work_is_set = TRUE;
        EntrySetValue( work_entry, filter_wlist[0] );
        EntrySetList( work_entry, filter_wlist, num_f_works );
	noticeDown();
}/*
END of WorksForAuthor */


int
GenerateWorkLists( void )
{
        int num_works;

        if( work_list[0] != NULL )
	{
		freeNameList( work_list );
                EntrySetList( work_entry, NULL, 0);
                EntrySetValue( work_entry, "");
		work_list[0]=NULL;
        }

        num_works = GetWorks( IdtFile(), work_list, block_list, work_nums );

        if(num_works > MN)
                SYSTEMCRASH(99);

        num_f_works = FilterWorkList( num_works,work_nums,
				      work_list,filter_wlist,
                                      block_list,filter_blist,filter_elist,
				      work_nums,filter_wnums);
        work_index = 0;
}


/*-------------------------------------------------------------------
 *  SetWork		     09.13.90 MM
 *
 *  Sets the selected work in the entry field.  Called by list
 *  selection.
 *  09.13.90 MM
 *-------------------------------------------------------------------
 */
void
SetWork( int selection_num, char *selection )
{
        work_is_set = TRUE;
        work_is_open = FALSE;
        EntrySetValue( work_entry, selection );
        work_index = selection_num;
        SetWorkIndex( filter_wnums[selection_num] );
	return;
}/*
END of SetWork
*/

/*--------------------------------------------------------------------
 *  OutputCallback           05.22.91 MM
 *
 *  List callback for output.
 *--------------------------------------------------------------------
 */
int
OutputCallback( int print_type, char *print_selection )
{
    extern struct CFG *chiron_cfg;

    if(work_is_open)
    {
        switch (print_type)
        {
           case PRINTCIT:
                PrintThisCit();
           break;

           case PRINTALL:
                PrintAllCits();
           break;

           case PRINTCANON:
                  if( PingPrinter(chiron_cfg->printer_port_id) )
                  {
                     InitializePrinter(latin);
                     if(!PrintCanon())
                     {
                        makeNotice("   Error Printing Cannon  ",2,WAIT);
                        noticeDown();
                     }
                  }
                  else
                  {
                        makeNotice("Printer Offline -- Unable to print",
                                    2,WAIT);
                        noticeDown();
                  }
           break;

           case SAVECIT:
                save_mode = SAVECIT;
                SetAttributes(sfile_entry,E_VISIBLE,TRUE,END_LIST);
           break;

           case SAVEALL:
                save_mode = SAVEALL;
                SetAttributes(sfile_entry,E_VISIBLE,TRUE,END_LIST);
           break;

           case SAVECANON :
                save_mode = SAVECANON;
                SetAttributes(sfile_entry,E_VISIBLE,TRUE,END_LIST);
           break;

        }
    }
    else
    {
        makeNotice("    No text retrieved yet    ",2,WAIT);
        noticeDown();
    }

    return;
}/*
END of OutputCallback */



/*--------------------------------------------------------------------
 *  PrintThisCit             03.15.92 MM
 *
 *  Print the current citation.
 *--------------------------------------------------------------------
 */
void
PrintThisCit( void )
{
    extern struct CFG *chiron_cfg;

        if(work_is_open)
        {
            if( PingPrinter(chiron_cfg->printer_port_id) )
            {
                makeNotice("     Printing current citation     ",2,NOWAIT);
                InitializePrinter(greek);
                PrintContext( Work.linestart, Work.line_max, Work.line_index,
                                 Work.context );
            }
            else
                makeNotice("Printer Offline -- Unable to print",2,WAIT);

            noticeDown();
        }
        else
        {
                makeNotice("     No text retrieved yet     ",2,WAIT);
                noticeDown();
        }
}/*
END of PrintThisCit */


/*--------------------------------------------------------------------
 *  PrintAllCits             03.15.92 MM
 *
 *  Print text of all matched citations
 *--------------------------------------------------------------------
 */
void
PrintAllCits( void )
{
    extern struct CFG *chiron_cfg;

        if(word_is_set)
        {
            SetAWIndex( 0 );
            NewAuthor( AUTHOR );
            GenerateWorkLists();
            text_search = TRUE;
            OpenCurrentWork();

            if( PingPrinter(chiron_cfg->printer_port_id) )
            {
                makeNotice("     Printing ALL Citations     ",3,NOWAIT);
                addToNotice("Press <Escape> to Stop");
                do
                {
                  if(CheckForEscape()) break;
                  InitializePrinter(greek);
                  PrintContext( Work.linestart, Work.line_max, Work.line_index,
                                 Work.context );
                }
                while(GoToNextMatchCitation());
            }
            else
                makeNotice("Printer Offline -- Unable to print",2,WAIT);

            noticeDown();
        }
        else
        {
                makeNotice("     No search performed yet     ",2,WAIT);
                noticeDown();
        }
}/*
END of PrintAllCits */


/*-------------------------------------------------------------------
 *  SaveTextTrigger          03.30.92 MM
 *
 *  File called after indicating save file name.
 *-------------------------------------------------------------------
 */
 void
 SaveTextTrigger( void )
 {
        char sfile[128];

        if(work_is_open)
        {
                EntryGetValue(sfile_entry,sfile);
                SetSaveFile( sfile );
                if(save_mode == SAVECIT)
                        SaveCurrentCit();
                else if( save_mode == SAVEALL )
                        SaveAllCits();
                else if( save_mode == SAVECANON )
                {
                        CanBibGetBiblio( AUTHOR, WORK );
                        CanBibToDisk( sfile );
                }

                save_mode = SAVECIT;
        }
        else
        {
                makeNotice("No Text Retrieved Yet",2,WAIT);
                noticeDown();
        }
}/*
END of SaveFileTrigger */


/*--------------------------------------------------------------------
 *  SaveAllCits             04.08.92 MM
 *
 *  Save all citations that correspond to a match.
 *--------------------------------------------------------------------
 */
void
SaveAllCits( void )
{
        if(word_is_set)
        {
            SetAWIndex( 0 );
            NewAuthor( AUTHOR );
            GenerateWorkLists();
            text_search = TRUE;
            OpenCurrentWork();

            makeNotice("     Saving ALL Citations     ",3,NOWAIT);
            addToNotice("Press <Escape> to Stop");
            do
            {
               if(CheckForEscape()) break;
               SaveCurrentCit();
             }
             while(GoToNextMatchCitation());

            noticeDown();
        }
        else
        {
                makeNotice("     No search performed yet     ",2,WAIT);
                noticeDown();
        }
}/*
END of SaveAllCits */



/*-------------------------------------------------------------------
 *  SetNewCit                03.08.92 MM
 *
 *  Put the new citation in the Cit field.
 *-------------------------------------------------------------------
 */
void
SetNewCit( void )
{
        static char cit_line[50];

        sprintf(cit_line,"%s.%s.%s.%s.%s",Work.currcit[CIT_V].level_val,
                                Work.currcit[CIT_W].level_val,
                                Work.currcit[CIT_X].level_val,
                                Work.currcit[CIT_Y].level_val,
                                Work.currcit[CIT_Z].level_val);

        EntrySetValue( cit_entry, cit_line );
}/*
END of SetNewCit */



/*-------------------------------------------------------------------
 *  SetNewContext            03.24.92 MM
 *
 *  Send the new context to the Work object
 *-------------------------------------------------------------------
 */
void
SetNewContext( void )
{
        char context_string[20];
        int context;

        EntryGetValue( context_entry, context_string );
        if( context = atoi( context_string ))
                WorkChangeContext( context );
}/*
END of SetNewContext */



/*--------------------------------------------------------------------
 *  HelpCallback                    05.22.91 MM
 *
 *  List callback for help.
 *--------------------------------------------------------------------
 */
int
HelpCallback( int help_type, char *help_selection )
{
        help( help_type );
}/*
END of HelpCallback */

/*-------------------------------------------------------------------
 *  FreeAll		     09.18.90 MM
 *
 *  Conditionally frees all of the dynamic data structures.
 *  09.18.90 MM
 *-------------------------------------------------------------------
 */
void
FreeAll()
{
	extern long m0,m1,m2,m3,m4;
        int i;

	m0=coreleft();
	m1=coreleft();

        FreeWordList();
        if(match_list != NULL)
        {
                TLGfree(match_list);
                match_list = NULL;
                EntrySetList( word_entry, NULL, 0);
        }
        if( work_list[0] != NULL )
	{
		freeNameList( work_list );
                EntrySetList( work_entry, NULL, 0);
                EntrySetValue( work_entry, "");
		work_list[0]=NULL;
        }
	m2=coreleft();
        if( author_list[0] !=NULL )
	{
                freeNameList( author_list );
                EntrySetList( author_entry, NULL, 0);
                EntrySetValue( author_entry, "");
		author_list[0] = NULL;
        }
	m3=coreleft();
        FreeAWList();
	m4=coreleft();
}/*
END of FreeAll
*/
