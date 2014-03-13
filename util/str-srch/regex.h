/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\regex.h 2.0 1993/09/18 16:43:58 mwm Exp mwm $
*/

#ifndef regex_h
#define regex_h

char *re_comp( char *, char * );
char *re_exec( unsigned char * );
void re_modw( char * );
int re_subs( char *, char *);
char *pmatch( unsigned char *, unsigned char * );
void re_fail( char *, char );
#endif
