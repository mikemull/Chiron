/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\wlist.h 2.1 1993/11/21 17:02:55 mwm Exp mwm $
*/

#ifndef wlist_h
#define wlist_h

typedef struct
{
    unsigned short start_wlblock,end_wlblock;
    unsigned short WordNum;
}WORDLOC;

typedef struct
{
    char text[40];
    char m_text[40];
}WORDTEXT;

typedef struct _word
{
    unsigned long  total;
    WORDLOC word_loc;
    WORDTEXT *word_text;
    struct _word *next;
    struct _word *prev;
}WORD;

extern WORD *match_words[];
extern int   match_count;

long CreateWordList( char *, BOOL );
int CountWords( BYTE * , BYTE * );
long SearchWordList( WORD , WORD **, BOOL );
void FreeWordList( void );

#endif
