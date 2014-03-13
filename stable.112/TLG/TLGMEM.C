/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\tlgmem.c 2.1 1993/11/21 16:40:02 mwm Exp mwm $
 *
 *   Wrapper around malloc that checks for errors and keeps track
 *   of mallocs/frees.
 *
 *   $Log: tlgmem.c $
 * Revision 2.1  1993/11/21  16:40:02  mwm
 * Added TLGrealloc.
 *
 * Revision 2.0  1993/09/18  16:21:05  mwm
 * Start of new version.
 *
 * 
 *-------------------------------------------------------------------
 */
#include <alloc.h>
#include <graphics.h>
#include <stdlib.h>
#include "tlg.h"

int alloc_count=0;
#ifndef DEBUG_MALLOC
void *
TLGmalloc( unsigned size )
{
	void *tptr;

    if ( (tptr = calloc(1, size )) == NULL )
    {
        closegraph();
        fprintf(stderr, "TLG: Out of memory\n" );
        SYSTEMCRASH(EXIT_MALLOC);
	}
	alloc_count++;
	return(tptr);
}

void
TLGfree( void *chunk )
{
        if(!chunk) fputs("TLG:Can't free NULL pointer",stderr);
        alloc_count--;
        free( chunk );
}

void *
TLGrealloc( void *chunk, unsigned size )
{
    void *tptr;

    if( !chunk )
    {
        closegraph();
        fputs("TLG: Can't realloc NULL pointer",stderr);
        SYSTEMCRASH(EXIT_MALLOC);
    }
    return( realloc( chunk, size ) );
}
#endif
