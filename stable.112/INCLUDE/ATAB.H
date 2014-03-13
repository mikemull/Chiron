/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\atab.h 2.0 1993/09/18 16:43:34 mwm Exp mwm $
*/

#ifndef atab_h
#define atab_h

/*-------  Prototypes  -----------*/

int FileNametoNumber( char * );
int GetAuthName( int , char *, BYTE * );
int GetFileName( char * , int, BYTE * );
BOOL CheckAuth( char * , char * );
int MakeAtabIndex( void );
int GetAuths( char * , char ** , int * , int );
BOOL ATABNumbertoName( int , char * );

#endif
