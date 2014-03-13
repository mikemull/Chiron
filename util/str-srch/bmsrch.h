/*
** $Header:$
*/
#ifndef bmsrch_h
#define bmsrch_h

#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "mem.h"

#define SKIP_ARRAY_SIZE 256
#define BM_MAXPATTERN   200
#define IGNORE          0
#define EXACT           1

BYTE *BM_Srch( BYTE *, BYTE *, int, BYTE *);
void  BM_Init( BYTE *, BYTE * );

#endif
