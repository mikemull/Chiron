/*
**   $Header: C:\MIKE\TLG\CHIRON\RCS\work.h 2.0 1993/09/18 16:44:05 mwm Exp mwm $
*/

#ifndef work_h
#define work_h

#include "ptree.h"
#include "citlow.h"
#include "citation.h"

#define FORWARD         0
#define BACKWARD        1
#define MAXLINES        1700  /* Alcaus Lyr., Fragmenta- 1607 lines/block*/
#define NUMBLOCKS       3
#define MAX_MATCH       25

typedef struct _work
{
        BOOL is_open;
        BOOL is_search_mode;
        FILE *text_file;
        char name[80];
        int author_num,work_num;
        int start_block,end_block,curr_block;
        int num_blocks_loaded;
        int num_blocks_in_work;
        char *buffers[NUMBLOCKS];
        int line_cnt[NUMBLOCKS];
        int line_ptr[NUMBLOCKS];
        int line_index;
        int line_max;
        int line_min;
        int context;
        char *linestart[MAXLINES];
        char *citstart[MAXLINES];
        citation_t currcit;
} WORK;

typedef struct _match
{
        char *begin,*end;
        int line;
} MATCH;


typedef struct _match_list
{
        int count;
        MATCH matches[MAX_MATCH];
} MATCHLIST;

extern WORK Work;

BOOL NewWork( FILE *, char *, int, int, int, int, BOOL );
BOOL GetNewBlock( int );
BOOL LoadBlocks( void );
void MakeLineIndex( int num_blocks );
BOOL SearchWork( void );
MATCHLIST *Find( TREENODE * );
MATCHLIST *SearchText( char *, int str_type );
void SortMatchList( void );
MATCHLIST *AndMatchList( MATCHLIST *, MATCHLIST * );
MATCHLIST *OrMatchList( MATCHLIST *, MATCHLIST * );
MATCHLIST *NotMatchList( MATCHLIST *, MATCHLIST * );
BOOL NextMatch( void );
BYTE *WorkMatchBegin( int );
BYTE *WorkMatchEnd( int );
int WorkMatchCount( void );
void WorkChangeContext( int );
BOOL WorkGotoCit( char * );
BOOL WorkRecalcCit( void );
int WorkLineIndex( void );
int WorkMaxIndex( void );
BOOL WorkMoveLineIndex( int );
char *WorkTextLine( int index );
char *WorkCitLine( int index );
int WorkNumLinesInBlock( int block_num );
citation_t *WorkCitStruct( void );
BOOL WorkGotoBeg( void );
BOOL WorkBegFound( void );
BOOL WorkIsSearchMode( void );
BOOL WorkIsOpen( void );

#endif

