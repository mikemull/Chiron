/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\citlow.h 2.0 1993/09/18 16:44:04 mwm Exp mwm $
 *
 *   Low Level re-usable Citation manipulation functions and types.
 *
 *   last modified  09.17.90  GH
 *-------------------------------------------------------------------
 */
#ifndef citlow_h
#define citlow_h

/******** MACROS ***********/

#define CIT_MAX_STR     150
#define CIT_BUFF_SIZE   150
#define CIT_MAX_LEVELS  8
#define CIT_OK          0
#define CIT_DESC(a)   (*citation)[a].level_desc
#define CIT_VALUE(a)  (*citation)[a].level_val

/******** TYPEDEFS *********/

typedef struct CIT_LEV
        {
                char        level_desc[CIT_MAX_STR];
                char        level_val[CIT_MAX_STR];
        } citation_level;

typedef citation_level citation_t[CIT_MAX_LEVELS];

enum level_names { CIT_AUTHOR, CIT_WORK, CIT_ABBREV,
                   CIT_V, CIT_W, CIT_X, CIT_Y, CIT_Z,
                   CIT_EOSTR,CIT_EOBLOCK,CIT_EOFILE
                 };

enum type_vals   { TYPE_EOF,TYPE_NEWAUTH,TYPE_NEWWORK,TYPE_NEWSECTION,
                   UNUSED_4,UNUSED_5,UNUSED_6,TYPE_NEWFILE,TYPE_BEGCIT,
                   TYPE_ENDCIT,TYPE_LASTCIT,TYPE_BEGEXCEPT,TYPE_ENDEXCEPT,
                   UNKNOWN_13,UNUSED_14,UNUSED_15,TYPE_AWDESC,TYPE_LLDESC,
                   UNUSED_18,UNUSED_19,UNUSED_20,UNUSED_21,UNUSED_22,
                   UNUSED_23,UNUSED_24,UNUSED_25,UNUSED_26,UNUSED_27,
                   UNUSED_28,UNUSED_29,UNUSED_30,TYPE_HEADER
                 };

/******** PROTOTYPES *******/

BOOL  StrUpdate( char *, char );
unsigned LoadCitBuffer( BYTE *, FILE * );
void  ResetCitation( int, citation_t * );
BYTE *DecodeLeftNibble( BYTE, BYTE, int *, BYTE * );
BYTE *DecodeRightNibble( citation_t *, BYTE, int, BYTE * );
BYTE *DecodeBinCitBuf( BYTE *, citation_t * );
int   DecodeBinCit( FILE *, citation_t * );
BYTE *DecodeNewAuthWork( BYTE *, unsigned *, unsigned *, citation_t * );
BYTE *DecodeNewSection( BYTE *, unsigned * );
BYTE *DecodeNewFile(BYTE *,unsigned *,unsigned long*, unsigned *,citation_t *);
BYTE *DecodeSectionStartCit( BYTE *, citation_t * );
BYTE *DecodeSectionEndCit( BYTE *, citation_t * );
BYTE *DecodeBlockEndCit( BYTE *, citation_t * );
BYTE *DecodeExceptionStart( BYTE *, unsigned *, citation_t * );
BYTE *DecodeExceptionEnd( BYTE *, citation_t * );
BYTE *DecodeAuthWorkDesc( BYTE *, citation_t * );
BYTE *DecodeLowLevDesc( BYTE *, citation_t * );
BYTE *DecodeCombinedHeader( BYTE * );
BYTE *DecodeUnknownType13( BYTE * );
void  InitCit( citation_t * );
BYTE *GetFullCitFromBuf( BYTE *, citation_t * );
BOOL  FindNextTypeCodeinIDT( FILE *, int );
BYTE *FindNextTypeCodeinBuf( BYTE *, int );
void  ParseLevelValue( char *, int *, char ** );
int   LvlCmp( char *, char * );
int   CompareCit( citation_t *, citation_t * );
#endif
