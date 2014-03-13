/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\tlgmem.h 2.1 1993/11/21 17:01:30 mwm Exp mwm $
*/

#ifndef tlgmem_h
#define tlgmem_h

#ifndef DEBUG_MALLOC
void *TLGmalloc( unsigned );
void TLGfree( void * );
void *TLGrealloc( void *, unsigned );
#else
void *Tcl_DbCkalloc( unsigned, char *, int );
void *Tcl_DbCkrealloc( char *, unsigned, char *, int );
void Tcl_DbCkfree( void *, char *, int );
void Tcl_ValidateAllMemory( char *, int );
void Tcl_DumpMemoryStats( void * );

#endif

#endif
