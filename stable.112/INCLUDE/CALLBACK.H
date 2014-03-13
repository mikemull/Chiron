/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\callback.h 2.0 1993/09/18 16:43:31 mwm Exp mwm $
*/

#ifndef gettxt_h
#define gettxt_h

/*
**   List callbacks
*/
int  GetWordList( int, char * );
void TextListCallback( int, char *);
void SetWord( int, char *);
void SetAuthor( int, char *);
void SetWork( int, char * );
int  OutputCallback(int,char *);
int  HelpCallback( int, char * );

/*
**  Non-fixed entry triggers
*/
void SetNewContext( void );
void SaveTextTrigger( void );

/*
**   Action procedures
*/
void GetTextForWord( void );
void WordListOption( void );
void FindWorksWithPattern( void );
void FindApproxAuthors( void );
void FindAllMatchesInText( void );
void SetNextWork( void );
BOOL OpenNextWork( void );
BOOL OpenCurrentWork( void );
void GoToCitation( void );
BOOL GoToNextMatchCitation( void );
void GotoNextMatch( void );
int  GetAuthorList( void );
void authorsForWord( void );
void WorksForAuthor( void );
void PrintThisCit( void );
void PrintAllCits( void );
void SaveAllCits( void );
void SetNewCit( void );

void FreeAll(void);

#endif

