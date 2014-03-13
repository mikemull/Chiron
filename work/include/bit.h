/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\bit.h 2.0 1993/09/18 16:43:47 mwm Exp mwm $
*/

#ifndef bit_h
#define bit_h

unsigned BitVal2( BYTE * );
unsigned BitVal5( BYTE * );
unsigned BitVal7( BYTE * );
UL       BitVal7xN( BYTE *, unsigned );
unsigned BitVal13( BYTE *, short );
unsigned BitVal14( BYTE *, short );
unsigned BitVal15( BYTE *, short );
unsigned BitVal16( BYTE *, short );

#endif

