/*
** $Header:$
*/
#ifndef strsrch_h
#define bygsrch_h

#include <string.h>
#include <mem.h>

#define BYG_MAXPATTERN  ( sizeof( UL ) * 8 )   /* Maximum pattern length */
#define MAXLEVERRORS   ( 5 )          /* Maximum number of errors allowed */

void BYG_Init( char *, char * );
BYTE *BYG_Srch( char *, char *, int, char * );
int  BYG_SetErrors( int );
int  BYG_PattLen( char * );
char *BYG_CharClass( char *, UL );
#endif
