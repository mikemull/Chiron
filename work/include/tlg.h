/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\tlg.h 2.1 1993/11/21 17:00:43 mwm Exp mwm $
 *  TLG.H                    9.24.89 MM
 *
 *  General header file for TLG code.
 *
 *--------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>

/*
** DEFINES and MACROS
*/
#define TRUE            1
#define FALSE           !TRUE
#define MAXSERIAL       4778        /*  Total number of work serial numbers */
#define MAX_AUTH        850         /* actually 839, but what the heck */
#define MAX_AUTH_NUM    9216
#define MAXWORDLEN      64          /* This may not even be big-enough */
#define TLGBLOCKSIZE    8192
#define HIBITSET(a)  	( (a) & 0x80 )

/* Error exits */
#define  EXIT_BADREAD    1
#define  EXIT_BADOPEN    2
#define  EXIT_MALLOC     3
#define  EXIT_UNDEF      99
/* Citation errors */
#define  EXIT_LOADBUFFER        201
#define  EXIT_LEFTNIBBLE        202
#define  EXIT_LEFTESC           203
#define  EXIT_LEFTSPECIAL       204
#define  EXIT_STRUPDATE         205
#define  EXIT_RGHTNIBBLE        206
#define  EXIT_RGHTLONGSTR       207
#define  EXIT_AWDESCLEN         208
#define  EXIT_LLDESCLEN         209
#define  EXIT_BADTYPECODE       210
#define  EXIT_BADLEVELINDEX     211
#define  EXIT_CITAUTHERROR      212
#define  EXIT_CITWORKERROR      213
#define  EXIT_WHEREISCIT        214
#define  EXIT_CITBADBUFPTR      215
#define  EXIT_CITBADIDTMALLOC   216
#define  EXIT_CITBADIDTREAD     217
#define  EXIT_CITIDTTOOLARGE    218
/* ATAB errors */
#define  EXIT_ATABAUTHNUM       220
#define  EXIT_AUTHNAMELEN       221
#define  EXIT_ATABMAXAUTH       222
/* CANON errors */
#define EXIT_BADCANAUTHCIT      230
#define EXIT_BADCANCITINDEX     231
#define EXIT_CANLINETOOLONG     232
#define EXIT_BADCANWORKCIT      233
#define EXIT_BLOCKTOOBIG        234
#define EXIT_READLINEMALLOCFAIL 235

#define BEEP        do { sound(440); delay(200);nosound(); } while(0)
#define CHECKREAD(ptr,size,num,stream) \
do \
{ \
	if((num) != fread((void *)(ptr),(size),(num),(stream))) \
	{ \
		closegraph();\
        fprintf(stderr,"Could not read %d bytes in %s at line %d\n",\
                ((size)*(num)),__FILE__,__LINE__ );\
        exit(EXIT_BADREAD);\
    } \
} while(0)

#define SYSTEMCRASH( errnum ) \
do \
{ \
closegraph(); \
ClockShutdown(); \
fprintf(stderr, "Fatal error number %d in %s at line %d\n", errnum, \
        __FILE__,__LINE__);\
exit(errnum); \
} while(0)

#define DEBUG if(debug_chiron) Debug

/*
#define DEBUG_MALLOC
*/


#ifdef DEBUG_MALLOC
#define TLGmalloc(a) Tcl_DbCkalloc(a, __FILE__, __LINE__ )
#define TLGfree(a)  Tcl_DbCkfree(a, __FILE__, __LINE__ )
#define TLGrealloc(a,b )  Tcl_DbCkrealloc( a,b,__FILE__,__LINE__)
#endif

/*
** TYPEDEFS
*/
typedef unsigned char BYTE;
typedef unsigned short BOOL;
typedef unsigned long UL;

typedef struct worknames
	{
		char			WorkDesc[30];
		unsigned        	BlockLoc;
		struct worknames	*NextNode;
	} WorkNames;

extern FILE *cdebug;        /* Debug file */
extern BOOL debug_chiron;
extern BYTE *LINXbuf;
extern UL   *CINXbuf;
extern BYTE *AWLSTbuf;
extern int  word_num;
extern char expression[],author[],work[];

/*
** PROTOTYPES
*/
void Debug(char *,...);
void ExitMessage();
