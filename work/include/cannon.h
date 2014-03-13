/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\cannon.h 2.0 1993/09/18 16:43:59 mwm Exp mwm $
*/

#ifndef cannon_h
#define cannon_h

typedef struct c_f_name
{
   char               cannon_field[100];
   struct c_f_name    *next;
} c_f_type;

void SkipJunk( BYTE ** );
int DetFieldName( BYTE ** );
void InitCannon( c_f_type * );
void GetCannonField( BYTE ** , char * );
BOOL FindAuthEntry( BYTE ** , int );
BOOL GetAuthEntry( BYTE ** , c_f_type * );
BOOL FindWorkEntry( BYTE ** , int , int );
BOOL FindEntryinBlock( BYTE ** , int , int , c_f_type * );
BOOL FindLoc( FILE * , FILE * , int , int ,  c_f_type * , BYTE ** );
BOOL GetCannon( FILE * , FILE * , c_f_type * , int , int );

#endif
