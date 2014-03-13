#ifndef install_h
#define install_h

/* Macros     */

#define MAXPATHLEN  80
#define TRUE 1
#define FALSE 0

/*  Typedefs    */

typedef unsigned char BOOL;

/* Structures */
#include "config.h"

/* PROTOTYPES */

int  CheckPath( int, char* );
int  MakePath ( int, char* );
int  CheckSrcFiles ( int, char*, char **, int );
int  IntroIntroScr( void );
void GetSourcePath( void );
void GetDestPath( void );
void CopyFiles( void );
void Abort( void );
int IntroScr( void );
void Install( void );
int ConfigIntroScr( void );
int ConfigPrinter( void );
int ConfigPPort( void );
int SelectCD( void );
int WriteConfig( void );
void Config( void );

#endif
