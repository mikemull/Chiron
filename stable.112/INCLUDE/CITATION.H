/*===================================================================
 *   citation.c              10.23.89  GH
 *
 *   File contains code to perform TLG citation manipulations.
 *
 *   last modified : 02.03.90  GH
 *
 *===================================================================
 */
#ifndef citation_h
#define citation_h

/******** MACROS ***********/

#define CIT_NOT_FOUND           -200
#define AUTH_NOT_FOUND          -201
#define WORK_NOT_FOUND          -202
#define FIND_AUTH_BAD_CODE      -203
#define FIND_WORK_BAD_CODE      -204

/******** PROTOTYPES *********/

BYTE *FindAuthBlockByNumber( BYTE *, unsigned, unsigned * );
BYTE *FindWorkBlockByNumber( BYTE *, unsigned, unsigned * );
BYTE *WhereisAuthWorkNum( BYTE *, unsigned, unsigned, unsigned * );
BYTE *FindAuthBlockByName( BYTE *, char *, unsigned * );
BYTE *FindWorkBlockByName( BYTE *, char *, unsigned * );
BYTE *FindAuthBlock( BYTE *, citation_t *, unsigned * );
BYTE *FindWorkBlock( BYTE *, citation_t *, unsigned * );
BYTE *FindLowBlock( BYTE *, citation_t *, unsigned * );
int  CheckCit( citation_t * );
BYTE *CitationLoadFromIdt( FILE *idt_file );
BYTE *WhereisCit( BYTE *, citation_t *, unsigned * );
void CitFromTextIndex( char **, int, citation_t * );
void CitStrfromStruc( citation_t *, char * );
void CitStrtoStruct( char *, citation_t * );

#endif
