/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\text.h 2.0 1993/09/18 16:43:43 mwm Exp mwm $
*/

#ifndef text_h
#define text_h

#define SKIPTEXT(a)  while( !HIBITSET(*(a)) ) (a)++
#define SKIPHIGH(a)  while( HIBITSET(*(a)) ) (a)++
#define TEXTCOUNT(a,i) while( !HIBITSET((a)[i]) ) i++;
#define HIGHCOUNT(a,i) while( HIBITSET((a)[i]) ) i++;
#define ISTEXT(a)  ((a != '.') &&\
		    (a != '*') &&\
		    (a != ' ') &&\
		    (a != ','))

BOOL SearchTextFile( char *, FILE *, int, int );
BOOL WordIsInLine( char *, char *);
int IndexBlock( BYTE *, char **, char ** );
char *NextWord( char *, BYTE * );
char *skipText( char * );
void MassageWord( char *, char * );
BOOL CheckChar( char );
unsigned CheckDiacrit( char );
#endif
