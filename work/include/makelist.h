/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\makelist.h 2.0 1993/09/18 16:43:42 mwm Exp mwm $
*/

#ifndef makelist_h
#define makelist_h

unsigned createAuthorNameList( char ** );
unsigned GetWorks( FILE *, char **, int *, int* );
unsigned FilterWorkList(int,int *,char **, char**,
                        int *,int *,int *,int *,int *);
void freeNameList( char ** );

#endif
