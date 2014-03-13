/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\author.h 2.0 1993/09/18 16:43:56 mwm Exp mwm $
*/

#ifndef author_h
#define author_h

typedef struct {
    int number;
    FILE *idtfile;
    FILE *txtfile;
} Author;

void NewAuthor( int num );
void CloseAuthor( void );
FILE *IdtFile( void );
FILE *TextFile( void );

#endif
    
