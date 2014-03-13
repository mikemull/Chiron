/*=========================================================
 *
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\epson_fx.h 2.0 1993/09/18 16:43:55 mwm Exp mwm $
 *
 *   Print Driver routines for Epson FX-80 and compatable
 *   Printers.
 *
 *   Last Modified : 03-11-91   GH
 *=========================================================
 */

#ifndef EPSON_FX_H
#define EPSON_FX_H

int FX_Setup();
int FX_PrintStr( char *, int );
int FX_FlushPrinter();
int FX_FormFeed();
int FX_LineFeed();
int FX_HomePrinter();
int FX_InitPrinter();
int FX_SelectFont(fonts);
int FX_XlateBetaLine( BETALETTER *, char *, int );

#endif
