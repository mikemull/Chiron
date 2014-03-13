/*
 * This stuff is temporary until Tcl is included into discover?
 */

#define TCL_OK 0
#define TCL_ERROR 1

void  Tcl_ValidateAllMemory ();
int   Tcl_AnyActiveMemory ();
int   Tcl_DumpMemoryStats ();
int   Tcl_DumpActiveMemory ();
char *Tcl_DbCkalloc();
char *Tcl_DbCkrealloc();
int   Tcl_DbCkfree();
