/*
** $Header
*/

#ifndef file_h
#define file_h

FILE *OpenTLG( char *file_name );
void SetCDROMLetter( int drive );
void MoveToBlock( FILE *, unsigned long );

#endif
