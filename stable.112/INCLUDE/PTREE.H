/*
**  $Header: C:\MIKE\TLG\CHIRON\RCS\ptree.h 2.0 1993/09/18 16:43:49 mwm Exp mwm $
*/

#ifndef ptree_h
#define ptree_h

#define SKIPSPACE(a)	while(isspace(*(a))) (a)++;
#define TWRD            0
#define TSTR            1
#define TOP             2

typedef struct _tnode
{
        unsigned char type;
        union
        {
                char *word;
                char oper;
        }elem;
        struct _tnode *left,*right;
} TREENODE;

unsigned    Parse( char * );
TREENODE    *Expression( void );
TREENODE    *Newop( TREENODE *,TREENODE *,char);
TREENODE    *Element( void );
void        GetToken( void );
TREENODE    *ParseTree();
void        FreeParseTree( TREENODE * );
int         PTreePatternCount( void );

#endif
