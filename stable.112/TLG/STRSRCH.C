/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\strsrch.c 2.0 1993/09/18 16:27:24 mwm Exp mwm $
 *
 *   String Search Implementations.
 *   Functions to implement Boyer-Moore string search algorithm.
 *
 *   BYTE *StrSrch( BYTE *pattern, BYTE *text, int text_len, BYTE *ignore_set)
 *   BYTE *MatchPattern( BYTE *pattern, BYTE *text, int text_len,
 *                       BYTE *ignore_set)
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
/*
#define testcode
*/
#include "tlg.h"
#include "strsrch.h"
#include "regex.h"


char betachars[] = "'ABGDEVZHQIKLMNCOPRSTUFXYW";
char diacrit[] = "()|+/\\=";
BYTE *beg_of_patt, *end_of_patt;
static char *igset[2] = { diacrit, "\\/=" };
static int     searchtype=IGNORE;


/*-------------------------------------------------------------------
 *   StrSrch                 04.24.90  GH
 *
 *   StrSrch implements the Boyer-Moore string searching algorithm
 *   for use in searching blocks of text for a given Case Sensitive
 *   substring.
 *
 *   last modified  06.27.90  GH
 *-------------------------------------------------------------------
 */
BYTE *StrSrch( BYTE *pattern, BYTE *text, int text_len, BYTE *ignore_set)
{
	int pattern_len,pattern_index,text_rbf_index=0,skip_index,ignore_index=0;
	unsigned char_pos,text_index;
	BYTE skip_array[SKIP_ARRAY_SIZE],temp_pattern[200];

        strcpy((char *)temp_pattern,(char *)pattern);
        pattern_len = strlen((char *)temp_pattern);
	text_index = pattern_len - 1;
	char_pos = text_index +1;

	memset(skip_array,pattern_len+1,SKIP_ARRAY_SIZE);

	pattern_index = 0;

        while( ignore_index <= strlen((char *)ignore_set) - 1 )
	      skip_array[ignore_set[ignore_index++]] = 1;

	while( pattern_index <= pattern_len-1  )
	{
	   skip_array[temp_pattern[pattern_index]] = char_pos--;
           if( strchr((char *)ignore_set,temp_pattern[pattern_index]))
	   {
	      memmove(&temp_pattern[pattern_index],
		      &temp_pattern[pattern_index+1],
		      (pattern_len-pattern_index));
	      pattern_len--;
	      pattern_index--;
	   }
	      pattern_index++;
	}

	while((text_index <= text_len ) && ( pattern_len > 0 ))
	{
	   if( skip_array[text[text_index]] == 1 )
	   {
	      text_rbf_index = text_index;
	      pattern_index = pattern_len - 1;
              end_of_patt = (char *) &text[text_rbf_index] + 1;
              while( strchr((char *)ignore_set,text[text_rbf_index]) )
		text_rbf_index--;
	      while((temp_pattern[pattern_index]==text[text_rbf_index])
		 && ( pattern_index >= 0 ))
	      {
		 pattern_index--;
		 text_rbf_index--;
                 while( (strchr((char *)ignore_set,text[text_rbf_index])) &&
                        (text_rbf_index >=0 ))
		   text_rbf_index--;
	      }
	      if( pattern_index < 0 )
              {
                 beg_of_patt = (char *)&(text[text_rbf_index+1]);
                 /*
                 ** Move end_of_patt beyond ignore set characters (mike,21791)
                 */
                 while(strchr((char *)ignore_set,*end_of_patt))
                        end_of_patt++;
		 return (&(text[text_rbf_index+1]));
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
	return NULL;
}/*
END OF StrSrch
*/


/*-------------------------------------------------------------------
 *   MatchPattern                 04.24.90  GH
 *
 *   Catch-all routine for pattern-matching. Directs the search through
 *   strsrch or regex stuff depending on regex characters.
 *-------------------------------------------------------------------
 */
BYTE *MatchPattern( BYTE *pattern, BYTE *text, int text_len )
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
                        pmatch = StrSrch(pattern,text,text_len,
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

        return pmatch;
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


#ifdef testcode

main()
{

	BYTE c,pattern[200],dup[200],text[201],*p,*q;
	int skiparray[33],match=0,tindex=0,index=0;
	BYTE s[33];
	unsigned long itcount,lindex;
	time_t beg1,end1,beg2,end2;
	unsigned inp_pat_len;
	BYTE ignore[12];

	randomize();

	puts(" STRSRCH2 \n");
	puts("Enter number of iterations ");
	scanf("%lu",&itcount);

/*
	while( index <=8141 )
	{
		text[index++] = random(92)+34;
	}
	text[201] = 0x00;
*/

	strcpy(text,"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz12344567890abcdefghijklmnopqrstuvwxyz!@#$%^&*()abcdefghijklmnopqrstuvxyz1!2@3#4$5%6^7&8*9(0)abcdefghijklmnopqrstuvwxyz!!@@##$$%%^^&&**(())!@##@!$%^^%%");

	puts("TEXT :\n");
	puts(text);
	puts("\n");
	puts("\nEnter pattern ");
	scanf("%s",&pattern);
	strcpy(dup,pattern);
	puts("Enter ignore set chars ");
	scanf("%s",&ignore);

	time ( &beg1);
	for ( lindex = 0; lindex < itcount; lindex++ )
		p = strstr(text,pattern);
	time ( &end1);

	time( &beg2 );
	for ( lindex = 0; lindex < itcount; lindex++ )
	{
		q = StrSrch(pattern,text,strlen(text),ignore);
		strcpy(pattern,dup);
	}
	time ( &end2 );

	printf("\nstrstr elapsed time = %ld seconds. Found pattern = %s.\n",end1-beg1,p);
	printf("Strsrch  elapsed time = %ld seconds. Found pattern = %s.\n",end2-beg2,q);
	printf("for %lu iterations\n\n\n",itcount);
	getch();
}

#endif
