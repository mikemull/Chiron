/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\ptree.c 2.0 1993/09/15 20:38:27 mwm Exp mwm $
 *
 *  Functions to parse expressions into a tree
 *
 *  $Log: ptree.c $
 * Revision 2.0  1993/09/15  20:38:27  mwm
 * Start of new version.
 *
 * Revision 1.5  92/03/08  17:07:25  ROOT_DOS
 * Added quoted tokens with < and > characters
 * 
 * Revision 1.4  92/03/08  16:30:06  ROOT_DOS
 * Added check on return from Expression in Parse in case null
 * tree is returned.
 * 
 * Revision 1.3  91/12/01  19:26:04  ROOT_DOS
 * Added PTreePatternCount function
 * 
 * Revision 1.2  91/12/01  19:06:14  ROOT_DOS
 * Added ParseTree and FreeParseTree functions
 * 
 * Revision 1.1  91/09/12  19:13:42  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tlg.h"
#include "ptree.h"
#include "text.h"
#include "tlgmem.h"

static char tpatterns[5][50];   /* Array to hold words in expression */
static int word_cnt;            /* Count of words                    */
static char token[50];          /* Array to hold most recent token   */
static int  token_type;         /* token type -- see next enum       */
static char *input;             /* Global pointer to input expression*/
static TREENODE *treenode=NULL; /* Pointer to top of tree */


/*-------------------------------------------------------------------
 *  Parse                    09.11.91 MM
 *
 *  Points static pointer at input expression and then calls
 *  real parsing routines
 *-------------------------------------------------------------------
 */
unsigned
Parse(char *in_expression)
{
        word_cnt = 0;
        input = in_expression;
        FreeParseTree( treenode );
        treenode = Expression();
        return( (treenode != NULL) ? word_cnt : 0 );
}/*
END of Parse */


/*-------------------------------------------------------------------
 *  Expression               09.11.91 MM
 *
 *  Parses expressions
 *-------------------------------------------------------------------
 */
TREENODE *
Expression(void)
{
        TREENODE *tnode=NULL;

        tnode=Element();

        GetToken();
        switch( *token )
	{
                case '&' :
                  return( Newop(tnode,Expression(),'&') );
                case '%' :
                  return( Newop(tnode,Expression(),'%') );
                case '~' :
                  return( Newop(tnode,Expression(),'~') );
                case '}' :
		case NULL :
                  return(tnode);
		default : return( NULL );
	}
}
/*
END OF Expression  */


/*-------------------------------------------------------------------
 *  Newop                    09.11.91 MM
 *
 *  Adds new operator and operands to tree
 *-------------------------------------------------------------------
 */
TREENODE *
Newop( TREENODE *left, TREENODE *right, char op )
{
        TREENODE *opnode;

        opnode = (TREENODE *) TLGmalloc(sizeof(TREENODE));
        opnode->type = TOP;
        opnode->elem.oper = op;
        opnode->left = left;
        opnode->right = right;

        return(opnode);
}/*
END of Newop */


/*-------------------------------------------------------------------
 *  Element                  09.11.91 MM
 *
 *  Parses Elements
 *-------------------------------------------------------------------
 */
TREENODE *
Element( void )
{
        TREENODE *tnode;

	SKIPSPACE(input);
        if( isalpha(*input) || ( *input == '#' ) || ( *input == '.') ||
            ( *input == '<' ) || ( *input == '\'') )
        {
                GetToken();
                MassageWord(token,tpatterns[word_cnt]);
                tnode = (TREENODE *) TLGmalloc(sizeof(TREENODE));
                tnode->type = token_type;
                tnode->elem.word = tpatterns[word_cnt];
                word_cnt++;
                return(tnode);
        }
        else if ( *input == '{')
	{
		input++;
                return(Expression());
	}
	else
		return ( NULL );
}/*
END of Element */


/*-------------------------------------------------------------------
 *  GetToken                 07.19.1990
 *
 *   Returns the next space delimited string from the input.
 *
 *-------------------------------------------------------------------
 */
void
GetToken( void )
{
        char *p=token;

	SKIPSPACE(input);

        /*
        ** < and > indicate quotes on pattern
        */
        if( *input == '<' )
        {
                while( (*++input != '>') && (*input) )
                        *p++ = *input;
                input++;
                token_type = TSTR;
        }
        else if( strchr( "{}&%~", (int) *input ) )
        {
                *p++ = *input++;
                token_type = TOP;
        }
        else
        {
                while(! strchr( "{}&%~ ",(int) *input ) )
                        *p++=*input++;
                token_type = TWRD;
        }

        *p = NULL;

	SKIPSPACE(input);
}/*
END OF GetToken
*/


/*-------------------------------------------------------------------
 *   ParseTree               09.15.91 MM
 *
 *   Returns the top of the last parse tree.
 *-------------------------------------------------------------------
 */
TREENODE *
ParseTree( void )
{
        return(treenode);
}/*
END of ParseTree */


/*-------------------------------------------------------------------
 *   FreeParseTree           09.15.91 MM
 *
 *   Frees the parse tree.
 *-------------------------------------------------------------------
 */
 void
 FreeParseTree( TREENODE *ptree )
 {
        if(ptree==NULL)
                return;
        else if ( (ptree->type == TWRD) || (ptree->type == TSTR) )
                TLGfree(ptree);
        else
        {
                FreeParseTree( ptree->right );
                FreeParseTree( ptree->left );
                TLGfree(ptree);
        }
}/*
END of FreeParseTree */


/*-------------------------------------------------------------------
 *   PTreePatternCount       12.01.91 MM
 *
 *   Frees the parse tree.
 *-------------------------------------------------------------------
 */
int
PTreePatternCount( void )
{
        return( word_cnt );
}/*
END of PTreePatternCount */

#ifdef TEST
static char t1[]= "C%{A&B}";
static char t2[]= "C";
static char t3[] = "{{A&B}~ {C%B}}";
static char t4[] = "A&B% C";

main()
{
        TREENODE *test;

        word_cnt = 0;
        input = t1;
        test = Expression();

        word_cnt = 0;
        input = t2;
        test = Expression();

        word_cnt = 0;
        input = t3;
        test = Expression();

        word_cnt = 0;
        input = t4;
        test = Expression();

        return(0);
}
#endif

