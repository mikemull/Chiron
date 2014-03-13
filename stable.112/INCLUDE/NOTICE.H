/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\notice.h 2.0 1993/09/18 16:44:15 mwm Exp mwm $
*/

#ifndef notice_h
#define notice_h

enum {NOWAIT=0,WAIT,YORN};
unsigned char makeNotice( char *, int, int );
void noticeDown( void );
void switchNotice( char * );
void addToNotice( char * );
void WaitForKey( void );

#endif


