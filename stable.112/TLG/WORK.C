/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\work.c 2.1 1994/02/13 19:04:04 mwm Exp mwm $
 *
 *  First implementation of a "work" object
 *
 *  $Log: work.c $
 * Revision 2.1  1994/02/13  19:04:04  mwm
 * Make sure match_list  is not NULL before checking if more
 * matches are available.
 *
 * Revision 2.0  1993/09/18  16:24:20  mwm
 * Start of new version.
 *
 * Revision 1.19  92/07/09  18:41:14  ROOT_DOS
 * Added several small functions
 * 
 * Revision 1.18  92/06/01  17:52:14  ROOT_DOS
 * Numerous changes including addition of WorkMoveLineIndex
 * 
 * Revision 1.17  92/04/27  19:14:28  ROOT_DOS
 * Added WorkGotoBeg stuff.
 * 
 * Revision 1.15  92/04/15  18:22:52  ROOT_DOS
 * Added code to make line updates and cit updates internal to object
 * 
 * Revision 1.14  92/04/02  14:02:54  ROOT_DOS
 * Took out hard-coded contexts and replaced with Work.context
 * 
 * Revision 1.13  92/03/24  20:27:50  ROOT_DOS
 * Added goto citation functionality.
 * 
 * Revision 1.12  92/03/10  19:29:13  ROOT_DOS
 * Added Work.line_ptr
 * 
 * Revision 1.11  92/03/07  18:09:52  ROOT_DOS
 * Changed operator to oper because this is a  C++ keyword
 * Changed exit to SYSTEMCRASH macro
 * 
 * Revision 1.10  92/02/04  18:28:05  ROOT_DOS
 * Added SortMatchList Function
 * 
 * Revision 1.9  91/12/02  19:56:14  ROOT_DOS
 * removed references to searchtype
 * 
 * Revision 1.8  91/12/02  17:07:39  ROOT_DOS
 * Added first version of ChangeContext function
 * 
 * Revision 1.7  91/12/01  19:35:17  ROOT_DOS
 * Added check for number of patterns; with one pattern skip 3 blocks
 * at a time
 * 
 * Revision 1.6  91/12/01  19:14:01  ROOT_DOS
 * Added functions to return beginning, end of pattern and match count
 * 
 * Revision 1.5  91/12/01  16:40:08  ROOT_DOS
 * added NextMatch function
 * 
 * Revision 1.4  91/12/01  13:12:00  ROOT_DOS
 * Added AND, OR, and NOT list functions.
 * 
 * Revision 1.3  91/12/01  11:00:00  ROOT_DOS
 * Added MATCHLIST data structure, changed code accordingly
 * 
 * Revision 1.2  91/12/01  09:25:44  ROOT_DOS
 * First crack at context checking
 * 
 * Revision 1.1  91/11/12  18:00:49  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include <stdio.h>
#include <math.h>
#include <sys\stat.h>
#include <graphics.h>
#include "tlg.h"
#include "citlow.h"
#include "citation.h"
#include "work.h"       /* includes ptree.h */
#include "tlgmem.h"
#include "text.h"
#include "strsrch.h"
#include "wlist.h"      /* for CountWords prototype */
#include "author.h"     /* for idt file             */
#include "callback.h"   /* for SetNewCit            */
#include "file.h"
#include "greek.h"      /* for ISPUNCT              */
#include "inxfile.h"    /* for DiaVal prototype     */

WORK            Work = {FALSE,NULL};   /* Work structure for current work */

static MATCHLIST  *match_list=NULL;    /* list of all matches in 3-blocks */
static int        curr_match=0;
static int        first_work = TRUE;
static BOOL       work_beg_found = FALSE;


/*-------------------------------------------------------------------
 *  NewWork                  10.01.91 MM
 *
 *  Set up a new work.  Loads the first 3 blocks and inits all the
 *  variables
 *-------------------------------------------------------------------
 */
BOOL
NewWork( FILE *text_file,char *name,int anum,int wnum,int sblock,int eblock,
         BOOL is_search )
{
        int i,prev_cnt,blocks_left;
        struct stat file_stat;

        if( first_work )
        {
                Work.buffers[0] = (char *) TLGmalloc( 8192 );
                Work.buffers[1] = (char *) TLGmalloc( 8192 );
                Work.buffers[2] = (char *) TLGmalloc( 8192 );
                first_work = FALSE;
        }

        MoveToBlock( text_file, (UL) sblock );

        Work.text_file = text_file;
        /*
        ** Get the file size in 8K blocks
        */
        fstat(fileno(text_file), &file_stat);
        Work.num_blocks_in_work = (file_stat.st_size/8192);

        Work.line_max=0;
        Work.curr_block = Work.start_block = sblock;
        Work.end_block = (eblock < 0) ? (Work.num_blocks_in_work-1) : eblock;
        strncpy(Work.name,name,79);
        Work.author_num = anum;
        Work.work_num = wnum;
        Work.is_search_mode = is_search;

        if(match_list) TLGfree( match_list );
        match_list = NULL;

        Work.is_open = LoadBlocks();

        if(!WorkGotoBeg())
                Work.line_index = 1;

        Work.line_min = Work.line_index;

        if(is_search)
                Work.is_open = SearchWork();

        WorkRecalcCit();
        return(Work.is_open);
}/*
END of NewWork */


/*-------------------------------------------------------------------
 *  LoadBlocks               03.18.92 MM
 *
 *  Load the next 3 blocks or whatever is left in the work.
 *-------------------------------------------------------------------
 */
BOOL
LoadBlocks( void )
{
        register int i,blocks_to_read,load_ok = TRUE;
        register int blocks_left = Work.end_block - Work.curr_block +1;

        blocks_to_read = blocks_left < 3 ? blocks_left : 3;

        if(blocks_to_read > 0)
        {
           for(i=0; i < blocks_to_read; i++)
           {
              CHECKREAD( Work.buffers[i], sizeof(BYTE), 8192, Work.text_file );
              Work.buffers[i][8191] = '\0';
           }

           Work.curr_block += blocks_to_read;
           Work.num_blocks_loaded = blocks_to_read;
           MakeLineIndex(blocks_to_read);
        }
        else
                load_ok = FALSE;

        return(load_ok);
}/*
END of LoadBlocks */



/*-------------------------------------------------------------------
 *  MakeLineIndex            03.18.92 MM
 *
 *  Make an index of the text lines and citation beginnings over all
 *  blocks indicated by argument
 *-------------------------------------------------------------------
 */
void
MakeLineIndex( int num_blocks )
{
        int i,prev_cnt,save_line_index;

        Work.line_max = 0;
        for(i=0,prev_cnt=0;i<num_blocks;i++)
        {
           Work.line_ptr[i] = Work.line_max;
           Work.line_max += IndexBlock((BYTE *)Work.buffers[i],
                                          &Work.linestart[Work.line_max],
                                          &Work.citstart[Work.line_max]);
           Work.line_cnt[i] = Work.line_max - prev_cnt;
           prev_cnt += Work.line_cnt[i];
        }

        if(Work.line_max > MAXLINES)
                SYSTEMCRASH(99);

        save_line_index = Work.line_index;

        Work.line_index = Work.line_max;
        while(!WorkRecalcCit() && (Work.line_index > 0) )
                Work.line_index--;
        Work.line_max = Work.line_index;

        Work.line_min = 0;

        Work.line_index = save_line_index;
}/*
END of MakeLineIndex */



/*-------------------------------------------------------------------
 *  GetNewBlock              10.01.91 MM
 *
 *  Read in a new block either BACKWARD or FORWARD.
 *-------------------------------------------------------------------
 */
BOOL
GetNewBlock( int direction )
{
        char *hold;
        int i,prev_cnt;

        if(direction==FORWARD)
        {
            if( Work.curr_block > Work.end_block )
                  return(FALSE);
            hold = Work.buffers[0];
            Work.buffers[0] = Work.buffers[1];
            Work.buffers[1] = Work.buffers[2];
            Work.buffers[2] = hold;
        }
        else
        {
            if( (Work.curr_block-4) < Work.start_block )
                return(FALSE);
            hold = Work.buffers[2];
            Work.buffers[2] = Work.buffers[1];
            Work.buffers[1] = Work.buffers[0];
            Work.buffers[0] = hold;
            fseek(Work.text_file,-4*8192,SEEK_CUR);
            Work.curr_block -= 4;
        }

        if(! (8192==fread(hold,sizeof(BYTE),8192,Work.text_file) ))
                return(FALSE);
        Work.buffers[i][8191] = '\0';

        MakeLineIndex( 3 );
        Work.curr_block++;

        return(TRUE);
}/*
END of GetNewBlock */


/*-------------------------------------------------------------------
 *  SearchWork               12.1.91 MM
 *
 *  Search the current work for matches
 *-------------------------------------------------------------------
 */
BOOL
SearchWork( void )
{
        int pcount,i;
        BOOL more_blocks=TRUE;
        curr_match = 0;

        pcount = PTreePatternCount();

        if(match_list) TLGfree( match_list );

        do
        {
                if( (match_list = Find( ParseTree() )) )
                {
                                Work.line_index = match_list->matches[0].line;
                                SortMatchList();
                                WorkRecalcCit();
                                return(TRUE);
                }

                /* Skip extra blocks if only one pattern since context is
                 * irrelevant
                 */
                if(pcount == 1)
                        more_blocks = LoadBlocks();
                else
                        more_blocks = GetNewBlock(FORWARD);
        }
        while( more_blocks );

        return(FALSE);
}/*
END of SearchWork */


/*-------------------------------------------------------------------
 *  Find                     12.1.91 MM
 *
 *  Try to find matches based on parse tree.
 *-------------------------------------------------------------------
 */
MATCHLIST *
Find( TREENODE *ptree )
{
        if( ptree == NULL )
                return FALSE;
        else if( (ptree->type == TWRD) || (ptree->type == TSTR) )
        {
                return( SearchText( ptree->elem.word, ptree->type ) );
        }
        else
        {
                switch( ptree->elem.oper )
                {
                        case '&': /* AND */
                                return( AndMatchList( Find(ptree->left),
                                                      Find(ptree->right) ) );
                        case '%': /* OR */
                                return( OrMatchList( Find(ptree->left),
                                                     Find(ptree->right) ) );
                        case '~': /* NOT */
                                return( NotMatchList( Find(ptree->left),
                                                      Find(ptree->right) ) );
                        default:
                                SYSTEMCRASH(EXIT_UNDEF);  /* BAD BAD BAD */
                        break;
                }
        }
        return(FALSE);
}/*
END of Find */


/*-------------------------------------------------------------------
 *  SearchText                     12.1.91 MM
 *
 *  Find all matches of pattern in current 3-block frame.
 *-------------------------------------------------------------------
 */
MATCHLIST *
SearchText( char *tpattern, int str_type )
{
        int i,is_found = FALSE,valid_match = TRUE,num_line=0,pc=0,match_line;
        MATCHLIST *mlist;
        BYTE bc,ac;
        BYTE *last_pos,*dork;
        unsigned delta;

        mlist = (MATCHLIST *) TLGmalloc( sizeof(MATCHLIST) );

        for(i=0; (i<Work.num_blocks_loaded) && (pc<MAX_MATCH); i++)
        {
            Work.buffers[i][8191] = '\0';
            last_pos = (BYTE *) Work.buffers[i];
            delta = 8192;

            while(MatchPattern((BYTE *) tpattern,last_pos, delta))
            {
                 valid_match = TRUE;
                 /*
                 ** Check to see if this is a word
                 */
                 if(str_type == TWRD)
                 {
                        valid_match = FALSE;
                        /*
                        ** Look backward for '*' because betatext is wierd.
                        */
                        dork = beg_of_patt-1;
                        while( DiaVal(*dork) ) dork--;
                        if( (*dork) == '*' )
                                bc = '*';
                        else
                                bc = *(beg_of_patt-1);

                        ac = *end_of_patt;
                        if( ((bc==' ') || (bc == '*') || HIBITSET(bc)) &&
                            ((ac==' ') || HIBITSET(ac) || ISPUNCT(ac)) )
                                valid_match = TRUE;
                 }

                 match_line = CountWords((BYTE *)Work.buffers[i],
                                         (BYTE *)beg_of_patt ) + num_line-1;

                 /*
                 ** Check to see if the match is really in the current work
                 */
                 if(match_line > Work.line_max)
                        valid_match = FALSE;

                 if(match_line < Work.line_min)
                        valid_match = FALSE;

                 if(valid_match)
                 {
                        mlist->matches[pc].begin = beg_of_patt;
                        mlist->matches[pc].end = end_of_patt;
                        mlist->matches[pc].line = match_line;

                        pc++;
                 }

                 last_pos = end_of_patt;
                 /*
                 ** KLUDGE. Well not really, but this may not work because
                 ** of half-assed pc-segment shit
                 */
                 delta = 8192 - (last_pos-Work.buffers[i]);

                 if( (pc >= MAX_MATCH) || (delta > 8192) )
                        break;
            }

            num_line += Work.line_cnt[i];
	}

        mlist->count = pc;

        /* return NULL list if no matches found */
        if(pc == 0)
        {
                TLGfree(mlist);
                mlist = NULL;
        }
        return( mlist);
}/*
END of SearchText */


/*-------------------------------------------------------------------
 *  SortMatchList             02.02.91 MM
 *
 *  Sort the final match list with a shell sort
 *-------------------------------------------------------------------
 */
void
SortMatchList( void )
{
        int             i,j,h=1;
        MATCH           tm;
        MATCHLIST       *m = match_list;

        do
        {
                h = 3*h;
        } while (h <= (m->count) );

        do
        {
                h /= 3;
                for(i=h; i< (m->count); i++ )
                {
                        tm = m->matches[i];
                        j = i;
                        while( (m->matches[j-h].line) > tm.line )
                        {
                                m->matches[j] = m->matches[j-h];
                                j -= h;
                                m->matches[j] = tm;
                                if(j<=h)
                                        break;
                        }
                }
        }while(h > 1);
}/*
END of SortMatchList */


/*-------------------------------------------------------------------
 *  AndMatchList             12.1.91 MM
 *
 *-------------------------------------------------------------------
 */
MATCHLIST *
AndMatchList( MATCHLIST *l1, MATCHLIST *l2 )
{
        int i,j,n=0;
        MATCHLIST *l3;

        if( (l1 == NULL) || (l2 == NULL) )
        {
                if(l1 != NULL) TLGfree(l1);
                if(l2 != NULL) TLGfree(l2);
                return(NULL);
        }

        l3 = (MATCHLIST *) TLGmalloc( sizeof(MATCHLIST) );

        for(i=0; i < l1->count; i++)
        {
                for(j=0; j < l2->count; j++)
                {
                        if( (abs( l2->matches[i].line - l1->matches[j].line )
                            < Work.context) && (n < MAX_MATCH) )
                        {
                                l3->matches[n++] = l2->matches[i];
                                break;
                        }
                }
        }

        for(i=0; i < l2->count; i++)
        {
                for(j=0; j < l1->count; j++)
                {
                        if( (abs( l1->matches[i].line - l2->matches[j].line )
                            < Work.context) && (n < MAX_MATCH) )
                        {
                                l3->matches[n++] = l1->matches[i];
                                break;
                        }
                }
        }

        TLGfree(l1);
        TLGfree(l2);

        l3->count = n;
        return(l3);
}/*
END of AndMatchList */


/*-------------------------------------------------------------------
 *  OrMatchList              12.1.91 MM
 *
 *-------------------------------------------------------------------
 */
MATCHLIST *
OrMatchList( MATCHLIST *l1, MATCHLIST *l2 )
{
        int i=0;

        if( (l1 == NULL) && (l2 != NULL) ) return( l2 );
        if( (l2 == NULL) && (l1 != NULL) ) return( l1 );
        if( (l1 == NULL) && (l2 == NULL) ) return( NULL );

        while( (l1->count < MAX_MATCH ) && (i < l2->count) )
                l1->matches[(l1->count)++] = l2->matches[i++];

        TLGfree(l2);
        return(l1);
}/*
END of OrMatchList */


/*-------------------------------------------------------------------
 *  NotMatchList             12.1.91 MM
 *
 *-------------------------------------------------------------------
 */
MATCHLIST *
NotMatchList( MATCHLIST *l1, MATCHLIST *l2 )
{
        return(l1);
}/*
END of NotMatchList */



/*-------------------------------------------------------------------
 *  NextMatch
 *
 *  Get the next match either from match list or by new search.
 *-------------------------------------------------------------------
 */
BOOL
NextMatch( void )
{
        BOOL new_match = TRUE;

        if( match_list && (curr_match < (match_list->count-1)) )
                Work.line_index = match_list->matches[++curr_match].line ;
        else
        {
                if(LoadBlocks())
                        new_match = SearchWork();
                else
                        new_match = FALSE;
        }
        if(new_match) WorkRecalcCit();
        return( new_match );
}/*
END of NextMatch */


/*-------------------------------------------------------------------
 *  WorkMoveLineIndex        06.01.92 MM
 *
 *  Move the line index delta lines if possible.  Get new block
 *  if neccessary.   Returns FALSE if can't move.
 *-------------------------------------------------------------------
 */
BOOL
WorkMoveLineIndex( int delta )
{
       BOOL can_move = FALSE;
       int lshift;

       if( (Work.line_index + delta) > Work.line_max )
       {
           lshift = WorkNumLinesInBlock(0);
           if(GetNewBlock(FORWARD))
              Work.line_index  -= lshift ;
           else
              return(FALSE);
       }
       else if( (Work.line_index + delta) < 0)
       {
           if(GetNewBlock(BACKWARD))
              Work.line_index += WorkNumLinesInBlock(0);
           else
              return(FALSE);
       }

       Work.line_index += delta;
       if( !(can_move = WorkRecalcCit()) )
           Work.line_index -= delta;

       return( can_move );
}/*
END of WorkMoveLineIndex */


/*-------------------------------------------------------------------
 *  WorkGotoCit              06.01.92 MM
 *
 *  Go to the given citation if possible.  Returns FALSE if citation
 *  not found.
 *-------------------------------------------------------------------
 */
BOOL
WorkGotoCit( char *cit_string )
{
        citation_t *citation = TLGmalloc( sizeof(citation_t) );
        unsigned i,new_block;
        BOOL good_cit = FALSE;
        BYTE *idt_buffer;

        CitStrtoStruct( cit_string, citation );
        sprintf(CIT_VALUE( CIT_AUTHOR ), "%04d", Work.author_num );
        sprintf(CIT_VALUE( CIT_WORK ), "%03d", Work.work_num );

        idt_buffer = CitationLoadFromIdt( IdtFile() );
        if( WhereisCit( idt_buffer, citation, &new_block ) )
        {
                if ((new_block+3) > Work.end_block)
                        new_block =  Work.end_block - 2;
                MoveToBlock( Work.text_file, new_block );
                Work.curr_block = new_block;
                (void) LoadBlocks();
                for(i=Work.line_min;i<Work.line_max;i++)
                {
                        CitFromTextIndex( Work.citstart, i, &Work.currcit );

                        if( CompareCit( &Work.currcit, citation ) >= 0 )
                        {
                                good_cit = TRUE;
                                Work.line_index = i;
                                break;
                        }
                }
        }

        TLGfree(citation);
        TLGfree(idt_buffer);
        return( good_cit );
}/*
END of WorkGotoCit */


/*-------------------------------------------------------------------
 *  WorkRecalcCit            06.01.92 MM
 *
 *  Calculate the current citation from the line index by starting
 *  at the beginning of the nearest block and working forward.
 *  Returns FALSE if citation is not in current work.
 *-------------------------------------------------------------------
 */
BOOL
WorkRecalcCit( void )
{
        int i;
        citation_t *citation;
        BOOL good_cit = FALSE;

        citation = (citation_t *) TLGmalloc(sizeof(citation_t));

        for( i=(Work.num_blocks_loaded-1); i>=0; i-- )
                if(Work.line_index > Work.line_ptr[i])
                        break;

        CitFromTextIndex( &Work.citstart[Work.line_ptr[i]],
                          Work.line_index-Work.line_ptr[i],
                          citation );

        if( Work.work_num == atoi(CIT_VALUE( CIT_WORK )) )
        {
                memcpy(&(Work.currcit), citation, sizeof(citation_t) );
                good_cit = TRUE;
        }

        TLGfree(citation);
        return(good_cit);
}/*
END of WorkRecalcCit */



/*-------------------------------------------------------------------
 *   WorkGotoBeg
 *
 *   Sets the line_index member of the Work structure to
 *   the first line of the current work.  Returns TRUE if
 *   succesful, FALSE if unable to find current work in
 *   the loaded blocks
 *-------------------------------------------------------------------
 */
BOOL
WorkGotoBeg( void )
{
        unsigned i;
        BOOL good_cit = FALSE;

        for(i=0;i<Work.line_max;i++)
        {
           CitFromTextIndex( Work.citstart, i, &Work.currcit );
           if( (Work.author_num == atoi(Work.currcit[CIT_AUTHOR].level_val)) &&
               (Work.work_num == atoi(Work.currcit[CIT_WORK].level_val)) )
           {
                good_cit = TRUE;
                Work.line_index = i;
                work_beg_found = TRUE;
                break;
           }
           else
                work_beg_found = FALSE;
        }
        return( good_cit );
}/*
END of WorkGotoBeg */


/*
** Light-weight methods that basically return or set a variable
*/

/*-------------------------------------------------------------------
 *  WorkMatchBegin
 *-------------------------------------------------------------------
 */
BYTE *
WorkMatchBegin( int n )
{
        return( (BYTE *) match_list->matches[n].begin );
}/*
END of WorkMatchBegin */


/*-------------------------------------------------------------------
 *  WorkMatchEnd
 *-------------------------------------------------------------------
 */
BYTE *
WorkMatchEnd( int n )
{
        return( (BYTE *) match_list->matches[n].end );
}/*
END of WorkMatchEnd */


/*-------------------------------------------------------------------
 *  WorkMatchCount
 *-------------------------------------------------------------------
 */
int
WorkMatchCount( void )
{
        return( (match_list == NULL) ? 0 : match_list->count );
}/*
END of WorkMatchCount */


/*-------------------------------------------------------------------
 *  WorkLineIndex
 *-------------------------------------------------------------------
 */
int
WorkLineIndex( void )
{
        return Work.line_index;
}

/*-------------------------------------------------------------------
 *  WorkMaxIndex
 *-------------------------------------------------------------------
 */
int
WorkMaxIndex( void )
{
        return Work.line_max;
}

/*-------------------------------------------------------------------
 *  WorkTextLine
 *-------------------------------------------------------------------
 */
char *
WorkTextLine( int index )
{
        return( Work.linestart[index] );
}

/*-------------------------------------------------------------------
 *  WorkCitLine
 *-------------------------------------------------------------------
 */
char *
WorkCitLine( int index )
{
        return( Work.citstart[index] );
}

/*-------------------------------------------------------------------
 *  WorkNumLinesInBlock
 *-------------------------------------------------------------------
 */
int
WorkNumLinesInBlock( int block_num )
{
        return( Work.line_cnt[block_num] );
}


/*-------------------------------------------------------------------
 *  WorkChangeContext
 *-------------------------------------------------------------------
 */
void
WorkChangeContext( int new_context )
{
        Work.context = new_context;
}/*
END of WorkChangeContext */

/*-------------------------------------------------------------------
 *  WorkCitStruct
 *-------------------------------------------------------------------
 */
citation_t *
WorkCitStruct( void )
{
        return(&Work.currcit);
}


/*-------------------------------------------------------------------
 *  WorkBegFound
 *-------------------------------------------------------------------
 */
BOOL
WorkBegFound( void )
{
        BOOL return_value = work_beg_found;
        work_beg_found = FALSE;
        return( return_value );
}

/*-------------------------------------------------------------------
 *  WorkIsSearchMode
 *-------------------------------------------------------------------
 */
BOOL
WorkIsSearchMode( void )
{
        return(Work.is_search_mode);
}

/*-------------------------------------------------------------------
 *  WorkIsOpen
 *-------------------------------------------------------------------
 */
BOOL
WorkIsOpen( void )
{
        return(Work.is_open);
}


