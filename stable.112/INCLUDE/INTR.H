/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\intr.h 2.0 1993/09/18 16:43:52 mwm Exp mwm $
 *
 *===================================================================
 */
#ifndef intr_h
#define intr_h

void ClockToggle( BOOL );
void ClockSetup( void );
void ClockShutdown( void );
BOOL CheckForEscape( void );

#endif
