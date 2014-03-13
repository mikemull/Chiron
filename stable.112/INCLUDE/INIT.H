/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\init.h 2.0 1993/09/18 16:44:16 mwm Exp mwm $
*/

#ifndef init_h
#define init_h

#define LINXBUFSIZE     701
#define CINXBUFSIZE     1536
#define ATABBUFSIZE     26624
#define DOCCAN1SIZE     2048

extern FILE *TLGwlist,*TLGwcnts,*TLGcanidt,*TLGcantxt,*atab;

void Initialize( void );
FILE *InitOpenTLG( char *, char *);
void SwapCINX( void );

#endif

