/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\strsrch.h 2.0 1993/09/18 16:43:51 mwm Exp mwm $
*/

#ifndef strsrch_h
#define strsrch_h

#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "mem.h"

#define SKIP_ARRAY_SIZE 256
#define IGNORE          0
#define EXACT           1

extern char betachars[],diacrit[];
extern BYTE *beg_of_patt,*end_of_patt;

BYTE *StrSrch( BYTE *pattern, BYTE *text, int text_len, BYTE *ignore_set);
BYTE *MatchPattern( BYTE *pattern, BYTE *text, int text_len );
int   ChangeIgnoreSet( void );

#endif
