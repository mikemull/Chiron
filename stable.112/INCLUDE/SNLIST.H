/*
**  $Header: C:\MIKE\TLG\CHIRON\RCS\snlist.h 2.0 1993/09/18 16:43:45 mwm Exp mwm $
*/

#ifndef snlist_h
#define snlist_h

#include <ctype.h>
#include <alloc.h>
#include <stdlib.h>
#include <time.h>
#include "wlist.h"

unsigned *SNListFromPTree( TREENODE * );
unsigned *ANDList( unsigned *, unsigned *);
unsigned *ORList( unsigned *, unsigned *);
unsigned *NOTList( unsigned *, unsigned *);
unsigned *CreateSNList( WORD **, int );

#endif
