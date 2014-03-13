#ifndef canon_h
#define canon_h

/*---- MACROS ---- */

#define CAN_BIBLIO_MAX_LINES    100

/*---- Proto-types ---- */
void CanBibInit( void );
void CanBibFree( void );
int  CanBibFindCit( char ** );
int  CanBibGetBlock( long, BYTE **, BYTE ** );
void CanBibReadLine( BYTE *, char ** );
void CanBibGetAuthInfo( int );
void CanBibGetWorkInfo( int, int );
char **CanBibGetBiblio( int, int );
int  CanBibToDisk( char * );

#endif
