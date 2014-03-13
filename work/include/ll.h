/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\ll.h 2.0 1993/09/18 16:43:22 mwm Exp mwm $
*/

#ifndef ll_h
#define ll_h

typedef struct _node
	 {
	      AWnums AW;
	      unsigned  count;
	      struct _node *	ptr;
	 }node ;

typedef enum { AWNODE=0 } NodeType;

void *NewNode( NodeType );
void AddToTop( void **, void *, NodeType );
void RemoveList( void *, NodeType );
node *MakeAWList();
void print_List( node *);
BOOL s_List( node *, node *);
void CloseList( int, node *);
int CountNodes( node *);
node *MergeAWList( node *, node *);
node *SortAWList( node *);
BOOL CompareNode( node *, node *);

#endif
