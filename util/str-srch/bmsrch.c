/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\strsrch.c 2.0 1993/09/18 16:27:24 mwm Exp mwm $
 *
 *   String Search Implementations.
 *   Functions to implement Boyer-Moore string search algorithm.
 *
 *   BYTE *BM_Srch( BYTE *pattern, BYTE *text, int text_len, BYTE *ignore_set)
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
#include "bmsrch.h"

static BYTE skip_array[SKIP_ARRAY_SIZE];
static int  pattern_len;
static unsigned int text_index;

/*-------------------------------------------------------------------
 *   BM_Srch                 04.24.90  GH
 *
 *   BM_Srch implements the Boyer-Moore string searching algorithm
 *   for use in searching blocks of text for a given Case Sensitive
 *   substring. BM_Srch returns a pointer to the beginning of
 *   pattern in text if found otherwise it returns ( char * )NULL.
 *
 *	 EXTERNS :
 *		The caller must define the beg_of_patt and end_of_patt
 *		variable as BYTE *. BYG_Srch declares these as externs and
 *		will set them to the beginning and end of pattern respectively
 *
 *   last modified  02.23.95  GH
 *-------------------------------------------------------------------
 */
BYTE *
BM_Srch( BYTE *pattern, BYTE *text, int text_len, BYTE *ignore_set)
{
    extern   BYTE *beg_of_patt, *end_of_patt;   /* defined in strsrch.c */
    static char last_pattern[BM_MAXPATTERN] = "";
    static char last_igset[BM_MAXPATTERN] = "";
	int 	 pattern_index;
	int 	 text_rbf_index=0;
	int 	 skip_index;
	unsigned char_pos;

	/*
	 *	Make sure the pattern isn't too long
	 */
    pattern_len = strlen( (char *) pattern );
    if( pattern_len > BM_MAXPATTERN )
       return( ( BYTE * ) NULL );

    /*
     *  Only recalculate the skip_array and other
     *  initializations if the pattern or ignore_set
     *  has changed.
     */
    if( strcmp(last_pattern,pattern) || strcmp(last_igset,ignore_set))
    {
	   BM_Init( pattern, ignore_set );
       strcpy(last_pattern,pattern );
       strcpy(last_igset,ignore_set);
    }

    /*
	 *	Search the text pointed at by text for the pattern
	 *	in srch_pattern[].
	 */
	while((text_index <= text_len ) && ( pattern_len > 0 ))
	{
	   if( skip_array[text[text_index]] == 1 )
	   {
	      text_rbf_index = text_index;
	      pattern_index = pattern_len - 1;
		  end_of_patt = (char *) &text[text_rbf_index] + 1;
		  while( strchr((char *)ignore_set,text[text_rbf_index]) )
			 text_rbf_index--;
          while((pattern[pattern_index]==text[text_rbf_index])
				 && ( pattern_index >= 0 ))
	      {
			 pattern_index--;
			 text_rbf_index--;
			 while( (strchr((char *)ignore_set,text[text_rbf_index])) &&
					(text_rbf_index >=0 ))
			 {
				text_rbf_index--;
			 }
	      }
	      if( pattern_index < 0 )
		  {
			 beg_of_patt = (char *)&(text[text_rbf_index+1]);
			 /*
			  * Move end_of_patt beyond ignore set characters (mike,21791)
			  */
			 while(strchr((char *)ignore_set,*end_of_patt))
				end_of_patt++;
			 return( ( BYTE * )&(text[text_rbf_index+1]));
		  }
	      else
	      {
			 if( skip_array[text[text_rbf_index]]==1 )
				text_index++;
			 else
				text_index += skip_array[text[text_rbf_index]]-1;
		  }
	   }
	   else
		  text_index += skip_array[text[text_index]]-1;
	}
	return( ( BYTE * )NULL );
}/*
END OF BM_Srch
*/

/*-------------------------------------------------------------------
 *    BM_Init
 *
 *    Performs the initializations required for a Boyer-Moore
 *    search with ignore sets.
 *-------------------------------------------------------------------
 */
void
BM_Init( BYTE *pattern, BYTE *ignore_set )
{
   int      pattern_index;
   int      ignore_index=0;
   int      ignore_len;
   unsigned char_pos;

   /*
	*  Set up the srch_pattern and various integer quantities
	*  based on it.
	*/
   ignore_len = strlen((char *)ignore_set);
   text_index = pattern_len - 1;

   /*
	*  Initialize the skip_array by setting all values to
	*  pattern_len+1.
	*/
   memset(skip_array,pattern_len+1,SKIP_ARRAY_SIZE);

   /*
	*  If the ignore_set is non-NULL, we need to make some
	*  adjustments to the skip_array. This says that when
	*  ever we encounter a character from the ignore set in
	*  the text we should just skip over it to the next
	*  character.
	*/
   if( ignore_len )
   {
	  while( ignore_index <= ignore_len - 1 )
			skip_array[ignore_set[ignore_index++]] = 1;
   }

   /*
    *   Fill in the Skip array
    */
   pattern_index = 0;
   char_pos = text_index + 1;
   while( pattern_index <= pattern_len-1  )
   {
      skip_array[pattern[pattern_index]] = char_pos--;
	  pattern_index++;
   }
}
