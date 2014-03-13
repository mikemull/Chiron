#include <stdio.h>
#include <values.h>
#include "bitprint.h"

main()
{
    unsigned long foo,bar;
    int i;

    bar = MAXLONG;
    bit_print(bar,"bar 1 by %i\n",i);

    foo = 1;
    i =0;
    while( i < 32 )
    {
       foo = ( ( unsigned long ) 1 << i );
       bit_print(foo,"bitshift 1 by %i\n",i);
       printf("%lu\n",foo);
       i++;
    }
}
