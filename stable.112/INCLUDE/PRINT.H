/*=========================================================
 *    $Header: C:\MIKE\TLG\CHIRON\RCS\print.h 2.0 1993/09/18 16:43:53 mwm Exp mwm $
 *
 *    Print routines Header file.
 *
 *   last modified : 03-12-91
 *=========================================================
 */

#ifndef PRINT_H
#define PRINT_H

#include "greek.h"
#include "canon.h"
#define EPSON_FX

typedef enum {
greek, latin, nlq_greek
} fonts;

typedef struct {
	int (*Setup)();
	int (*PrintStr)(char *, int );
	int (*InitPrinter)();
	int (*HomePrinter)();
	int (*SelectFont)( fonts );
	int (*FormFeed)();
	int (*LineFeed)();
	int (*FlushPrinter)();
    int (*XlateBetaLine)( BETALETTER *, char *, int );
} PrintStruc;

typedef enum {
#ifdef EPSON_FX
epson_fx
#endif
} prn_devs;

void SelectPrinter( prn_devs );
void InitializePrinter( fonts );
void PrintBlock( char * );
void PrintContext( char **, int, int, int );
BOOL PingPrinter( int );
BOOL PrintCanon( void );

#endif
