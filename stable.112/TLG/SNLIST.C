/*===================================================================
 *    $Header: C:\MIKE\TLG\CHIRON\RCS\snlist.c 2.1 1993/11/21 16:25:04 mwm Exp mwm $
 *
 *    Functions to generate Serial Number lists from a parse tree
 *
 *    $Log: snlist.c $
 * Revision 2.1  1993/11/21  16:25:04  mwm
 * Added word saving boolean to CreateWordList argument list.
 *
 * Revision 2.0  1993/09/18  16:22:36  mwm
 * Start of new version.
 *
 * Revision 1.4  92/02/15  13:53:33  ROOT_DOS
 * removed searchtype from CreateWordList argument list
 * 
 * Revision 1.3  91/12/02  17:09:02  ROOT_DOS
 * Added code to free word list after SN list is created
 * 
 * Revision 1.2  91/11/25  20:58:31  ROOT_DOS
 * Changed CreateWordList arguments
 * 
 * Revision 1.1  91/09/15  14:37:01  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include "tlg.h"
#include "ptree.h"
#include "snlist.h"     /* includes wlist.h */
#include "tlgmem.h"
#include "wcnts.h"

#define SKIPSPACE(a)	while(isspace(*(a))) (a)++;

/*-------------------------------------------------------------------
 *  SNListFromPTree          09.14.91 MM
 *
 *  Creates SN List from Parse Tree
 *-------------------------------------------------------------------
 */
unsigned *
SNListFromPTree( TREENODE *ptree )
{
        long cnt;
        unsigned *sn_list_ptr;

        if( (ptree->type == TWRD) || (ptree->type == TSTR) )
        {
                cnt=CreateWordList(ptree->elem.word, FALSE);
                if(cnt > 0)
                {
                   sn_list_ptr = CreateSNList(match_words,cnt);
                   FreeWordList();
                }
                else
                   sn_list_ptr = NULL;

                return(sn_list_ptr);
        }
        else
        {
           switch( ptree->elem.oper )
           {
                case '&' :
                  return(ANDList(SNListFromPTree(ptree->left),
                                 SNListFromPTree(ptree->right)) );
                case '%' :
                  return(ORList(SNListFromPTree(ptree->left),
                                 SNListFromPTree(ptree->right)) );
                case '~' :
                  return(NOTList(SNListFromPTree(ptree->left),
                                 SNListFromPTree(ptree->right)) );
		default : return( NULL );
           }
        }
}/*
END OF SNListFromPTree  */


/*-------------------------------------------------------------------
 *   ORList                 07.30.90  GH
 *
 *   Performs a logical OR on two author/work lists.
 *
 *   Last modified  08.14.90  GH
 *-------------------------------------------------------------------
 */
unsigned *
ORList( unsigned *list1, unsigned *list2 )
{
        int i;

        for( i=0; i<MAXSERIAL; i++)
                list1[i] += list2[i];

        TLGfree(list2);
        return(list1);
}/*
END OF ORList
*/

/*-------------------------------------------------------------------
 *   ANDList                8.1.90  GH
 *
 *   Performs relational AND on two linked Author/Work Lists
 *   It is up to caller to make sure list1 is the shortest list.
 *
 *   Last modified : 08.20.90
 *-------------------------------------------------------------------
 */
unsigned *
ANDList( unsigned *list1, unsigned *list2 )
{
        int i;

        for( i=0;i<MAXSERIAL;i++)
                list1[i] = ((list1[i] && list2[i]) ? (list1[i]+list2[i]) : 0);

        TLGfree(list2);
        return(list1);
}/*
END OF ANDList
*/

/*-------------------------------------------------------------------
 *   NOTList                8.1.90
 *
 *   Performs logical NOT on two Author/Work lists.
 *   A ~ B means every node in A which is not in B
 *   NOTList performs list1 NOT list2.
 *
 *  last modified  08.09.90  GH
 *-------------------------------------------------------------------
 */
unsigned *
NOTList( unsigned *list1, unsigned *list2 )
{
        int i;

        for( i=0;i<MAXSERIAL;i++)
                list1[i] = ((list1[i] && list2[i]) ? 0 : list1[i]) ;

        TLGfree(list2);
        return(list1);
}/*
END OF NOTList
*/


/*-------------------------------------------------------------------
 *  CreateSNList             06.05.91 MM
 *
 *  Make a serial number array.
 *------------------------------------------------------------------
 */
unsigned *
CreateSNList( WORD *word[], int cnt )
{
        int i;
        unsigned *serial_list;

        serial_list = (unsigned *) TLGmalloc(MAXSERIAL*sizeof(unsigned));

        for(i=0;i<MAXSERIAL;i++)
                serial_list[i] = 0;

        DecodeWCEntry(word,serial_list,cnt);

        return(serial_list);
}/*
END of CreateSNList */


