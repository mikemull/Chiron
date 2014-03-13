#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "bitprint.h"

const unsigned long lone = ( unsigned long )1;

void
bit_print( long ba, char *fmt, ... )
{
    int i;
    va_list argptr;

    va_start(argptr, fmt );
    vprintf(fmt,argptr);
    va_end( argptr );
    for( i=31; i>=0; i-- )
       printf("%1d",( ba & ( lone << i ) ) != 0 );
    printf("\n");
}
