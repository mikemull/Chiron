/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\wcnts.h 2.1 1993/11/21 17:02:15 mwm Exp mwm $
*/

#ifndef wcnts_h
#define wcnts_h

#include "wlist.h"

BYTE *GetWCEntry( WORDLOC, FILE * );
unsigned long DecodeWCEntry( WORD **, unsigned *, int );

#endif
