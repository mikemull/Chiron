/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\bit.c 2.0 1993/09/18 16:23:27 mwm Exp mwm $
 *
 *   Functions to perform weird bit extractions.
 *   Re-typed in on 10.19.89 GH
 *   11.4.89 MM
 *
 *   $Log: bit.c $
 * Revision 2.0  1993/09/18  16:23:27  mwm
 * Start of new version.
 *
 *
 *-------------------------------------------------------------------
 */

#include "tlg.h"
#include "bit.h"

/*-------------------------------------------------------------------
 *   BitVal14                10.10.89  GH
 *
 *   Makes a 14 bit value from the low 7 bits of two consecutive BYTES
 *   11.4.89 MM
 *-------------------------------------------------------------------
 */

unsigned BitVal14( BYTE *Value, short reverse )
{
	unsigned first, second;
	if ( reverse )
	{
		first = 0;
		second = 1;
	}
	else
	{
		first = 1;
		second = 0;
	}

	return ( (((((unsigned)Value[first]) & 0x007f) << 7 ) & 0x3f80 ) |
		 (((unsigned)Value[second]) & 0x007f ));

}/*
END OF BitVal14
*/


/*-------------------------------------------------------------------
 *   BitVal7                 10.10.89  GH
 *
 *   Chops the high bit off and returns an unsigned
 *   11.4.89 MM
 *-------------------------------------------------------------------
 */

unsigned BitVal7( BYTE *Value)
{
	return ( Value[0] & 0x7f );
}/*
END OF BitVal7
*/


/*-------------------------------------------------------------------
 *   BitVal7xN
 *
 *   Makes a single number from the low 7 bits of N consecutive bytes
 *
 *-------------------------------------------------------------------
 */

unsigned long BitVal7xN( BYTE *List, unsigned N )
{
	unsigned long Result=0,i=0;

	while( N-- )
	{
		Result |= (((unsigned long) BitVal7( &List[N] )) << (i*7));
		i++;
	}
	return( Result );
}/*
END of BitVal7xN
*/



/*-------------------------------------------------------------------
 *    BitVal15               10.10.89  GH
 *
 *    Makes an unsigned 15 bit number from two bytes.
 *    11.4.89
 *-------------------------------------------------------------------
 */

unsigned BitVal15( BYTE *Value, short reverse )
{
	unsigned first, second,i,j,k;
	if ( reverse )
	{
		first = 0;
		second = 1;
	}
	else
	{
		first = 1;
		second = 0;
	}

	return(( ( ((unsigned)Value[first]) << 8 ) & 0x7f00 ) |
		 ( ((unsigned)Value[second]) & 0x00ff));
}/*
END OF BitVal15
*/


/*-------------------------------------------------------------------
 *   BitVal13                10.10.89  GH
 *
 *   Makes an unsigned from bits 2-6 of one byte and all 8 bits of
 *   the other byte
 *   11.4.89 MM
 *-------------------------------------------------------------------
 */

unsigned BitVal13( BYTE * Value, short reverse )
{
	unsigned first, second;
	if ( reverse )
	{
		first = 0;
		second = 1;
	}
	else
	{
		first = 1;
		second = 0;
	}

	return ( (((unsigned)Value[first] & 0x007c ) << 6 ) |
		 (((unsigned)Value[second]) & 0x00ff) );
}/*
END OF BitVal13
*/


/*-------------------------------------------------------------------
 *   BitVal2                 10.16.89  GH
 *
 *   Makes an unsigned from the two low bits of a byte.
 *   11.4.89 MM
 *-------------------------------------------------------------------
*/

unsigned BitVal2( BYTE *Value )
{
	unsigned i;
	return( (unsigned) (Value[0] & 0x03) );
}/*
END OF BitVal2
*/


/*-------------------------------------------------------------------
 *   BitVal5                 10.17.89  GH
 *
 *   Makes an unsigned from bits 2-6 of a byte
 *   11.4.89 MM
 *-------------------------------------------------------------------
 */

unsigned BitVal5( BYTE *Value )
{
	return ( (( Value[0] & 0x7c ) >> 2 ) & 0x1f );
}/*
END OF BitVal5
*/


/*-------------------------------------------------------------------
 *   BitVal16                10.17.89  GH
 *
 *   Makes an unsigned from all 16 bits of two bytes.
 *   11.4.89 MM
 *-------------------------------------------------------------------
 */

unsigned BitVal16( BYTE *Value, short reverse )
{
	unsigned first, second;
	if ( reverse )
	{
		first = 0;
		second = 1;
	}
	else
	{
		first = 1;
		second = 0;
	}

	return ( ((((unsigned)Value[first] ) << 8 ) & 0xff00 ) |
		(((unsigned)Value[second]) & 0x00ff ) );
}/*
END OF BitVal16
*/

