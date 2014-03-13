/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\wlist.c 2.1 1993/11/21 16:15:12 mwm Exp mwm $
 *   Contains routines for messing with the word list (TLGWLIST.INX)
 *
 *   int CountWords( BYTE *start, BYTE *word_pos )
 *   UL SearchWordList(WORD word,int word_val,WORD *word_info[],int word_type)
 *
 *   $Log: wlist.c $
 * Revision 2.1  1993/11/21  16:15:12  mwm
 * Changed word list to exclude text if not doing word list only search.
 *
 * Revision 2.0  1993/09/15  20:39:29  mwm
 * Start of new version
 *
 * Revision 1.6  92/04/16  20:00:59  ROOT_DOS
 * clean up for C++
 * 
 * Revision 1.5  91/12/02  17:11:20  ROOT_DOS
 * Added FreeWordList function
 * 
 * Revision 1.4  91/11/25  18:53:52  ROOT_DOS
 * Added BlockExtent function
 * 
 * Revision 1.3  91/11/24  12:40:10  ROOT_DOS
 * Moved createWordList into this file, moved match_words array to this file
 * 
 * Revision 1.2  91/09/05  18:09:24  ROOT_DOS
 * Added code to handle BETALETTER attributes.
 * 
 * Revision 1.1  91/09/03  19:25:28  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */

#include <string.h>
#include <stdlib.h>
#include "tlg.h"
#include "tlgmem.h"
#include "strsrch.h"
#include "wlist.h"
#include "text.h"
#include "win.h"
#include "notice.h"
#include "inxfile.h"
#include "bit.h"
#include "file.h"
#include "intr.h"

#define CURRWORD        word_info[words_fnd]
#define WORDARRAYSIZE   100

static BOOL BlockExtent( WORD * );

WORD    *match_words[WORDARRAYSIZE]; /* List of matched words        */
int     match_count;                 /* Number of words matched      */

/*-------------------------------------------------------------------
 *   CreateWordList          10.17.89 MM
 *
 *   Searches the word list, get the word counts entry, and decodes
 *   it to create a list of words and their associated authors and
 *   works.
 *   (formerly FindWord)
 *   02.10.90 MM
 *-------------------------------------------------------------------
 */
long
CreateWordList( char *Word, BOOL keep_text )
{
    extern FILE *TLGwlist,*TLGwcnts;
    unsigned WVal,i;
    UL match_num;
    WORD search_word;
    char temp_string[80];

    strcpy( search_word.word_text->text, Word );
    MassageWord( Word, search_word.word_text->m_text );
    if( !BlockExtent(&search_word) )
                match_num=0;
	else
	{
                makeNotice("   Searching Word List...  ",3,NOWAIT);
                addToNotice("Press <Escape> to Quit");
                MoveToBlock(TLGwlist,(UL) search_word.word_loc.start_wlblock );
                match_num=SearchWordList(search_word,match_words,keep_text);
                noticeDown();

                if(match_num == WORDARRAYSIZE)
                {
                        sprintf(temp_string,
                     "Word list may not contain all matches for pattern %s ",
                         search_word.word_text->text);
                        makeNotice(temp_string,2,WAIT);
                        noticeDown();
                }
	}

        return( match_num );
}/*
END of CreateWordList
*/


/*-------------------------------------------------------------------
 *   BlockExtent             11.24.91 MM
 *
 *   Sets the range of blocks to search in the WORD structure.
 *-------------------------------------------------------------------
 */
static BOOL
BlockExtent( WORD *pattern )
{
        BOOL in_list=TRUE,single_letter=FALSE;
        char *p,ts[3];
        unsigned WVal,next_index=1;

        p = pattern->word_text->m_text;

        Debug("BlockExtent:Block = %s \n",p);

        if ( LetterVal(p[0]) )
        {
                /*
                ** Check for single letter initial strings
                */
                if(strlen(p) == 1)
                        single_letter = TRUE;
                else
                {
                        p++;
                        while( !LetterVal( *p ) && *p )
                        {
                                if( (*p == '#') || (*p == '.') )
                                {
                                        single_letter = TRUE;
                                        break;
                                }
                                p++;
                        }
                }

                if(!single_letter)
                {
                    WVal = WordVal(pattern->word_text->m_text);
                    if( (WVal>=0) && (WVal<=701) )
                    {
                        pattern->word_loc.start_wlblock = SumBytes( WVal );
                        Debug("BlockNum = %d\n",pattern->word_loc.start_wlblock);

                        /*  Determine the number of blocks to look at
                        */
                        while( !LINXbuf[WVal + next_index] )
                              next_index++;
                        pattern->word_loc.end_wlblock =
                          pattern->word_loc.start_wlblock+
                            LINXbuf[WVal+next_index];
                    }
                    else
                        in_list = FALSE;
                }
                else
                {
                    ts[0] = pattern->word_text->m_text[0];
                    ts[1] = 'A';
                    ts[2] = '\0';
                    pattern->word_loc.start_wlblock = SumBytes(WordVal( ts ));
                    ts[1] = 'W';
                    ts[2] = '\0';
                    pattern->word_loc.end_wlblock = SumBytes( WordVal( ts ) );
                }
        }
        else
        {
        /*  No letters at start of pattern
        */
                if( !( (p[0] == '#') || (p[0] == '.') ) )
                     in_list = FALSE;
                else
                {
                     if(!makeNotice("Do you REALLY want to search the whole list",
                                2,YORN))
                        in_list = FALSE;
                     else
                     {
                        pattern->word_loc.start_wlblock = 0;
                        pattern->word_loc.end_wlblock = 1203; /* PARAMETERIZE
                                                                  tHIS */
                     }

                     noticeDown();
                }
        }

        return(in_list);
}/*
END of BlockExtent */


/*-------------------------------------------------------------------
 *      CountWords           10.07.90  GH
 *
 *      determines the number of words in the TLGwlist index
 *      from last position to word position and adds last
 *      count to result
 *
 *      last modified  10.07.90  GH
 *-------------------------------------------------------------------
 */

int
CountWords( BYTE *start, BYTE *word_pos )
{
	int count=0;

        while( start < word_pos )
        {
            if(HIBITSET(*start) && !HIBITSET(*(start+1)) )
	       count++;
            start++;
        }

        return(count);
}/*
END of CountWords */


/*-------------------------------------------------------------------
 *   SearchWordList          9.28.89 MM
 *
 *   Searches for a word in an 8K block of wordlist
 *   10.09.89 GH
 *-------------------------------------------------------------------
 */

long
SearchWordList( WORD word , WORD *word_info[], BOOL keep_text )
{
   extern FILE *TLGwlist;
   extern char diacrit[];
   int match_len=0,ncb=0,block_cnt=0;
   long words_fnd=0;
   BYTE *block,*block_index,*word_beg,*last_pos;
   BOOL srch_abort = FALSE;

   block     = (BYTE *) TLGmalloc( TLGBLOCKSIZE );
   block_cnt = word.word_loc.end_wlblock - word.word_loc.start_wlblock;

   do  /* Loop over all necessary blocks */
   {
      if(srch_abort = CheckForEscape())
        goto short_exit;

      CHECKREAD( block, TLGBLOCKSIZE, 1, TLGwlist);
      block_index = block;
      last_pos = block;

      while( (word_beg = MatchPattern( (BYTE *) word.word_text->m_text,last_pos,
                                 TLGBLOCKSIZE - ( last_pos - block ) ))  &&
              ((last_pos-block) < TLGBLOCKSIZE) )
      {
        if(srch_abort=CheckForEscape())
                goto short_exit;

        /*   Bail out if we need the full word and this ain't it
         */
         if( LetterVal(word.word_text->m_text[0]) && !HIBITSET(word_beg[-1]) )
         {
                last_pos = (BYTE *) end_of_patt;
                continue;
         }
         if( !HIBITSET(*end_of_patt) )
         {
                last_pos = (BYTE *) end_of_patt;
                continue;
         }

         if( words_fnd >= WORDARRAYSIZE )
                goto short_exit;

         /*
          *  Get whole word and character count
          */
         CURRWORD = (WORD *) TLGmalloc(sizeof(WORD));
         while(!HIBITSET(word_beg[-1])) word_beg--;
         block_index = word_beg;
         match_len=0;

         if(keep_text)
         {
           CURRWORD->word_text = (WORDTEXT *) TLGmalloc( sizeof(WORDTEXT) );

           while( !HIBITSET(*block_index) )
                 CURRWORD->word_text->text[match_len++] = *block_index++;

           CURRWORD->word_text->text[match_len] = '\0';
           MassageWord( CURRWORD->word_text->text,CURRWORD->word_text->m_text);
        }

        block_index = word_beg-1;

        /*
         *  Find the number of word occurences from count bytes.
         */
         ncb = 0;
         while( HIBITSET(*block_index) )
         {
                ncb++;
                block_index--;
         }
         CURRWORD->total = BitVal7xN((BYTE *)block_index+1,ncb);

         CURRWORD->word_loc.start_wlblock = word.word_loc.start_wlblock;
         CURRWORD->word_loc.WordNum = CountWords((BYTE *)block,
                                                 (BYTE *)word_beg);

         words_fnd++;
         word_beg+=match_len;

         last_pos = word_beg;
         while(!HIBITSET(*last_pos) && ((*last_pos) != 0) )
                last_pos++;;

      }/*END WHILE Strsrch finds match*/
      word.word_loc.start_wlblock++;

   } while( block_cnt-- );

short_exit:
   match_count = words_fnd;

   if(srch_abort)
   {
      FreeWordList();
      words_fnd = -1;
   }

   TLGfree(block);
   return(words_fnd);
}/*
END of SearchWordList
*/


/*-------------------------------------------------------------------
 *  FreeWordList             11.25.91 MM
 *
 *-------------------------------------------------------------------
 */
void
FreeWordList( void )
{
        int i;

        if( match_words[0] != NULL )
        {
                for(i=0;i<match_count;i++)
                {
                    if(match_words[i]->word_text != NULL)
                       TLGfree( match_words[i]->word_text );

                    TLGfree( match_words[i] );
                    match_words[i] = NULL;
                }
        }
        match_count = 0;
}/*
END of FreeWordList */

