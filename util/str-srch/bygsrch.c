/*===================================================================
 *   $Header:$
 *
 *   String searching via the Baeza-Yates-Gonnet algorithm.
 *   This implements exact matches as well as single ( . ),
 *   and multiple ( # ) wildcards, character sets ( [1..9] ),
 *   fuzzy matching ( up to k errors ), mixed exact and fuzzy
 *   matches and beginning and end of line matches ( ^, $ ).
 *
 *   A single pattern can be no longer than 32 chars ( sizeof( UL ) ).
 *   We will use this as a, hopefully, faster implementation of some
 *   of the features of regex.
 *
 *   $Log:$
 *-------------------------------------------------------------------
 */
#include <mem.h>
#include "tlg.h"
#include "bygsrch.h"

void bit_print( long, char *, ... );

static UL S[256];       /* The Alphabet bit-array, called S by convention */
static BYTE I[256];     /* Ignore characters ( diacrits ) bit-array */
static UL patt_errs=0;  /* Current Number of Levenshtein errors */
static UL m;            /* The pattern length, 'm' by convention */
static UL wildcard;     /* Holds the position of wildcards in patterns */

const UL ulone = ( UL ) 1;  /* An unsigned long 1 */

#define CHARCLASSBEG    '['
#define CHARCLASSEND    ']'
#define CHARCLASSSEP    ','
#define CHARCLASSCLASSSEP '-'
#define CHARCLASSCOMP   '~'

/*-------------------------------------------------------------------
 *   BYG_Srch                 04.24.90  GH
 *
 *   BYG_Srch implements the Baeza-Yates-Gonnet string searching algorithm
 *   for use in searching blocks of text for a given Case Sensitive
 *   substring. BYG_Srch returns a pointer to the beginning of
 *   pattern in text if found otherwise it returns ( char * )NULL.
 *
 *   The Standard BYG algorithm has been modified to handle the
 *   concept of IgnoreSets ( Characters in the pattern which should
 *   be treated as if they don't exist when doing the matching )
 *
 *	 EXTERNS :
 *		The caller must define the beg_of_patt and end_of_patt
 *		variable as BYTE *. BYG_Srch declares these as externs and
 *		will set them to the beginning and end of pattern respectively
 *      when it finds the pattern.
 *
 *   last modified  GH
 *-------------------------------------------------------------------
 */
BYTE *
BYG_Srch( char *pattern, char *text, int textlen, char *ignore_set )
{
    static char last_pattern[BYG_MAXPATTERN] = "";
    static char last_igset[BYG_MAXPATTERN] = "";
    extern BYTE *beg_of_patt, *end_of_patt;

    int match_found = FALSE;
    int i;
    int txtindex,begindex,endindex;
    UL  R[MAXLEVERRORS+1];    /* The match arrays. R[0] = exact match */
    UL  Rjp1[MAXLEVERRORS+1]; /* A temp array used during calculation */
    UL  foo;
    UL  mstart;
    UL  testbit;

    printf("pattern errors = %i\n",patt_errs);
    m = (UL) BYG_PattLen(pattern );
    printf("pattern length = %i\n",m);
    /*
     *  Make sure the pattern isn't too long
     */
    if( m > BYG_MAXPATTERN )
       return( ( BYTE * ) NULL );

    /*
     *  Only recalculate alphabet bit-arrays and srch_pattern
     *  if the supplied pattern or ignore set has changed.
     */
    if( strcmp(last_pattern,pattern) || strcmp(last_igset,ignore_set))
    {
       BYG_Init( pattern, ignore_set );
       strcpy(last_pattern,pattern );
       strcpy(last_igset,ignore_set);
    }

    /*
     *  Setup the R arrays so we can begin matching.
     *  R[0] is for exact matches, R[1] is for matches with 1 error, etc
     *  R[i] defined as 11..100..00 ( i 1 ones ) where i is the number of
     *       errors allowed in pattern
     */
    for(i=0;i<=patt_errs;i++)
    {
        R[i] = ((( ulone << i ) - ulone ) << m );
    }

    /*
     *  We use mstart as a mask to set the first bit in
     *  the pattern for each R array. This is a work-around
     *  for the fact that Rshift ( >> ) zero-fills in MS-DOS with
     *  Borland C
     */
    mstart = ulone << ( m-ulone );

    /*
     *  Now step through the text buffer of length textlen
     *  and look for a match.
     *
     *  In this implementation the algorithm only finds
     *  exact and k-error matches. i.e. No wildcards - etc..
     */
    txtindex = 0;
    while( ( txtindex < textlen ) && !match_found )
	{
        if( ( text[txtindex] > 0 ) && ( !I[text[txtindex]] ) )
		{
            /*
            printf("%c",text[txtindex]);
            */
            /*
             *  Right shift with 1 fill all the R arrays
             *  and & with S[*text]. This is all we need
             *  to do for the zero error case.
             */
            for( i=0;i<=patt_errs;i++ )
            {
               Rjp1[i] = ((( R[i] >> ulone ) | mstart ) & S[text[txtindex]] );
            }
            /*
             *  Finish calculating R arrays with errors
             */
            for( i=1;i<=patt_errs;i++ )
            {
               foo = ((((R[i-1]|Rjp1[i-1])>>ulone)|mstart)|R[i-1]);
               Rjp1[i] = Rjp1[i] | foo;
               /*
               bit_print( Rjp1[i],"Rjp1[%i] : ",i );
               */
            }

            /*
             *  Copy Rjp1 arrays back into R arrays
             */
            memcpy( R, Rjp1,((patt_errs+1)*sizeof(R[0])));

           /*
            *  Now see if we have any matches. We should only
            *  need to check the R[patt_errs] array.
            *  Unfortunately, this only works for the zero
            *  error case. :>(.
            *  Finding the beginning of the pattern in the
            *  arbitrary number of errors case is more
            *  difficult.
            */
           if( ( patt_errs == 0 )  && ( R[patt_errs] & mstart ) )
              beg_of_patt = ( BYTE * ) &text[txtindex];
        }
        match_found = R[patt_errs] & ulone;

        /*
         *  Increment to the next character in the text.
         */
        txtindex++;
	}

    if( match_found )
    {
       if( patt_errs > 0 )
       {
          /*
           *  Look for the beginning of the pattern if the
           *  number of pattern errors != 0
           */
          i = 0;
          testbit = ulone << ( BYG_MAXPATTERN - 1 );
          bit_print( Rjp1[patt_errs],"\nFinal Rjp1 pattern : ");
          while( i < BYG_MAXPATTERN )
          {
              if(Rjp1[patt_errs] & testbit )
                 break;
              i++;
              testbit = testbit >> ulone;
          }
          if( i == BYG_MAXPATTERN )
          {
             /* We've got some read error */
             return( ( BYTE * )NULL );
          }
          else
          {
              i = BYG_MAXPATTERN - i;
              printf("\npattern begins %i chars from end",i);
              begindex = ((txtindex-i) < 0 ) ? 0 : (txtindex-i);
              beg_of_patt =( BYTE *) &text[begindex];
          }
       }
       printf("\npattern begins with char %c\n",*beg_of_patt);
       /*
        *   And the end of the pattern is somewhere around here.
        *   This should work for any number of pattern errors
        */
       endindex = ((txtindex + patt_errs - 1 ) >= textlen) ? textlen - 1 :
                   (txtindex + patt_errs - 1 );
       end_of_patt = ( BYTE * ) &text[endindex];
       printf("pattern ends with char %c\n",text[endindex]);
    }
    else
    {
       end_of_patt = ( BYTE * ) NULL;
       beg_of_patt = ( BYTE * ) NULL;
    }

    return( beg_of_patt );
}

/*-------------------------------------------------------------------
 *   BYG_Init                 04.24.90  GH
 *
 *   BYG_Init implements the initialization functions for
 *   performing the BYG_Srch algorithm. BYG_Init sets the
 *   Alphabet arrays and the ignore arrays based on the
 *   contents of the pattern and ignoreset arguments.
 *
 *   BYG_Init currently handles :
 *             exact matches
 *             exact matches with a fixed number of Levenshtein errors
 *             character classes and complements of character classes.
 *
 *   last modified  GH
 *-------------------------------------------------------------------
 */
void
BYG_Init( char *pattern, char *ignore_set )
{
    int i,plen;
	UL bitpos;
    char *pattern_ptr;
    char *char_class_ptr;

    /*
     *  Clear the Alphabet ( S ) and Ignore-set ( I ) arrays.
     */
    memset( S, 0, 256*sizeof(UL));
    memset( I, 0, 256 );

    /*
	 * Loop through and create the ignore set array
	 */
	plen = strlen(ignore_set);
    while(plen--)
	{
        I[*ignore_set] = 1;
        ignore_set++;
	}

    /*
     *   Create the S array based on the ignore-set-less srch_pattern
     */
    plen = m;
    bitpos = ulone<<plen;
    pattern_ptr = &pattern[0];

    /*
     *  Loop through the pattern and create the S ( alphabet ) array.
     *  as follows.
     */
    while(plen--)
	{
        /*
         *  Get the 1 in the correct position in the pattern
         */
        bitpos >>= ulone;
        /*
         *  Process this character
         */
        if( *pattern_ptr == CHARCLASSBEG )
        {
           /*
            *  This is the start of a character class
            *  [1234], or [a-z] or [A-Z,1234].
            *  So hand this off the the BYG_CharClass routine to process.
            *  We get back a pointer to the first character following
            *  the character class.
            */
           pattern_ptr = BYG_CharClass( pattern_ptr, bitpos );
        }
        else
        {
           /*
            *  A simple character, so just set it in the S array and
            *  continue on.
            */
           S[*pattern_ptr] |= bitpos;
        }
        pattern_ptr++;
	}
    /*
     *  All Done with Initialization
     */
}
/*-------------------------------------------------------------------
 *   BYG_CharClass                04.24.95
 *
 *   Processes a character class into the S alphabet array
 *
 *   A character class begins with the character CHARCLASSBEG '['
 *   ends with CHARCLASSEND ']'. Characters in character ranges are
 *   seperated by CHARCLASSSEP '-' and multiple classes are seperated by
 *   CHARCLASSCLASSSEP ','. Compliments are indicated by preceeding a
 *   valid character class by CHARCLASSCOMP '~'.
 *
 *  Examples of valid character classes are :
 *    [1-9]     All characters between 1 and 9 inclusive
 *    [a-z]     All characters between a and z inclusive
 *    [a-z,A-Z] All characters between a and z and A and Z inclusive
 *    [12345abcd] Any one of 1 or 2 or 3 or 4 or a or b or c or d
 *    [~1-9]    All characters except 1 to 9 inclusive
 *    [~abcd]   All characters except a and b and c and d.
 *
 *-------------------------------------------------------------------
 */
char *
BYG_CharClass( char *beg, UL bitpos )
{
   char *CharClass_ptr = beg;   /* Pointer to current character in class */
   char FoundChars[256];        /* Array of characters encountered so far */
   char FoundCharsCount=0;      /* How many characters found so far */
   int CharComp = FALSE;        /* Is this character class compliment ? */
   int CharRange = FALSE;       /* Is this a character class range ? */
   int i;
   char charsetbeg = 'a';
   char charsetend = 127;
   char tmp;

   /*
    *  Make sure we were called correctly
    */
   if( *CharClass_ptr != CHARCLASSBEG )
   {
      /*
         An error. This isn't the beginning of a character class!
      */
      printf("How the Hell did we get here!\n");
      return((char *) NULL);
   }

   CharClass_ptr++;
   /*
    *  Loop until we see the end of the character class
    */
   while( *CharClass_ptr != CHARCLASSEND )
   {
      switch( *CharClass_ptr )
      {
         case CHARCLASSEND      :
         case CHARCLASSCLASSSEP :
            /*
             *  End of character class - process it. We treat these
             *  two cases the same because functionally they both
             *  represent the end of a character class specification.
             *  The only difference is that for CHARCLASSCLASSSEP we
             *  continue processing where for CHARCLASSEND we know
             *  we are done and we return control to caller.
             *
             *  First NULL terminate the found chars array. This
             *  makes it easier to work with
             */
            FoundChars[FoundCharsCount] = '\0';
            if( CharRange )
            {
                /*
                 *  A character Range [a-z]
                 */
                if( FoundChars[0] >= FoundChars[1] )
                {
                   /*
                    *  The beginning range char must be less then
                    *  the ending char, so switch them.
                    */
                   tmp = FoundChars[0];
                   FoundChars[0] = FoundChars[1];
                   FoundChars[1] = tmp;
                }
                if( CharComp )
                {
                   /*
                    *  A compliment range. We want all characters
                    *  except the ones in the specified range.
                    */
                   for(i=charsetbeg; i<FoundChars[0]; i++)
                      S[i] |= bitpos;
                   for(i=FoundChars[1]; i<charsetend; i++)
                      S[i] |= bitpos;
                }
                else
                {
                   /*
                    *  A simple Character Range
                    */
                   for( i=FoundChars[0]; i<=FoundChars[1]; i++ )
                   {
                      S[i] |= bitpos;
                   }
                }
            }
            else
            {
                /*
                 * A simple character sequence [abcd]
                 */
                if( CharComp )
                {
                   /*
                    *  We want the compliment of the characters
                    *  in the FoundChars array. I.E. we add char
                    *  c to array S only if C is not in FoundChars.
                    *
                    *  charsetbeg is the first valid character
                    *  charsetend is the last valid character.
                    */
                   for(i=charsetbeg; i<=charsetend; i++ )
                   {
                      if( strchr(FoundChars,i) == ( char *)NULL )
                         S[i] |= bitpos;
                   }
                }
                else
                {
                   /*
                    *  Simpliest case. We just want any one of the
                    *  chars in the FoundChars array.
                    */
                   for(i=0;i<FoundCharsCount;i++)
                   {
                      S[FoundChars[i]] |= bitpos;
                   }
                }
            }
            /*
             *  Figure out if we are all done or if there is more
             *  to process.
             */
            if( *CharClass_ptr == CHARCLASSCLASSSEP )
            {
               /*
                *  Reset everything since there is another subclass
                *  to process.
                */
                CharRange = FALSE;
                CharComp  = FALSE;
                FoundCharsCount = 0;
                CharClass_ptr++;
            }
            else
            {
               /*
                *  End of all classes, so return control
                */
               return( ++CharClass_ptr);
            }
            break;
         case CHARCLASSCOMP     :
            /*
             *  This is a character class compliment. Set a flag and
             *  deal with it when we process the actual class.
             */
            CharComp = TRUE;
            CharClass_ptr++;
            break;
         case CHARCLASSSEP      :
            /*
             *  This is a character range ( [a-b] ).
             *  For this to be valid, we must already have read
             *  a beginning character and the next character must be
             *  a simple character.
             */
            if( FoundCharsCount != 1 )
            {
               /*
                *  Syntax error!
                */
               return((char *)NULL);
            }
            CharRange = TRUE;
            CharClass_ptr++;
            break;
         case ( char * ) NULL   :
            /*
             *  If we see a NULL it is an error and we give up.
             */
            return((char *)NULL);
         default                :
            /*
             *  A simple character. Stick it in the found chars
             *  array. It will be processed when we get to the end
             *  of the character class.
             */
            FoundChars[FoundCharsCount] = *CharClass_ptr;
            FoundCharsCount++;
            CharClass_ptr++;
            break;
      }
   }
   return((char *)NULL);
}

/*-------------------------------------------------------------------
 *   BYG_PattLen                04.24.95 GH
 *
 *   BYG_PattLen determines the logical pattern length for
 *   a supplied pattern. For simple patterns this will be the
 *   same as the value returned by strlen(). For more complex
 *   patterns, especially those containing Character classes, the
 *   value returned by this function will be different then that
 *   returned by strlen().
 *
 *   last modified  GH
 *-------------------------------------------------------------------
 */
int
BYG_PattLen( char *pattern )
{
   int len=0;
   char *pattern_ptr = pattern;

   while( *pattern_ptr )
   {
      switch( *pattern_ptr )
      {
         case CHARCLASSBEG  :
            len++;
            while( *pattern_ptr != CHARCLASSEND )
              pattern_ptr++;
            pattern_ptr++;
            break;
         default            :
            len++;
            pattern_ptr++;
            break;
      }
   }
   return(len);
}



/*-------------------------------------------------------------------
 *   BYG_SetErrors                 04.24.95 GH
 *
 *   BYG_SetErrors is a simple routine for setting the number
 *   of Levenshtein errors on a search.
 *
 *   last modified  GH
 *-------------------------------------------------------------------
 */
int
BYG_SetErrors( int byg_err )
{
    if( byg_err > MAXLEVERRORS )
       patt_errs = MAXLEVERRORS;
    else
       patt_errs = byg_err;

    return( patt_errs );
}
