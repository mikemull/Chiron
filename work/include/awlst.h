/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\awlst.h 2.0 1993/09/18 16:43:27 mwm Exp mwm $
*/

#ifndef awlst_h
#define awlst_h

typedef struct
        {
		unsigned AuthorNumber;
		unsigned WorkNumber;
        unsigned count;
        } AWnums;

typedef struct
    {
        UL total_match;
        unsigned size;
        unsigned auth_index;
        unsigned curr_index;
        AWnums *AWlst;
    } AWlist;

int CreateAWList( unsigned * );
int CreateAWListFromAuth( int *, int );
BOOL CreateFullAWList( void );
unsigned GetAuthNum( BYTE * );
unsigned GetWorkNum( BYTE * );
AWnums DecodeAWEntry( BYTE * );
void SetAuthorIndex( int author );
void SetWorkIndex( int work_num );
BOOL SetAWIndex( int new_index );
void SetMatchTotal( unsigned total );
UL MatchTotal( void );
int CurrentAWIndex( void );
int AuthorAWIndex( void );
unsigned AWAuthor( unsigned );
unsigned AWWork( unsigned );
unsigned AWSize( void );
void FreeAWList( void );

#endif
