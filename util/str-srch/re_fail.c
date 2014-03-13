
#ifdef vms
#include stdio
#else
#include <stdio.h>
#endif
#include <stdlib.h>
#include "regex.h"
/* 
 * re_fail:
 *	internal error handler for re_exec.
 *
 *	should probably do something like a
 *	longjump to recover gracefully.
 */ 
void	
re_fail(s, c)
char *s;
char c;
{
	fprintf(stderr, "%s [opcode %o]\n", s, c);
	exit(1);
}
