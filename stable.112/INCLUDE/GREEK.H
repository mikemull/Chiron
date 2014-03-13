/*
**  $Header: C:\MIKE\TLG\CHIRON\RCS\greek.h 2.0 1993/09/18 16:43:26 mwm Exp mwm $
*/

#ifndef p_greek_h
#define p_greek_h

#define ISSIGMA(a)  ((a=='j') || (a=='J'))
#define ISPUNCT(a)  ((a=='.') || (a==';') || (a==',') || (a=='-') || (a==':') \
|| (a=='?'))
#define ISOPER(a)	((a=='&') || (a=='%') || (a=='~') || \
(a==']') || (a=='['))
#define ISESCAPE(a) ((a=='$') || (a=='&') || (a=='%') || (a=='\"') || \
(a=='@') || (a=='[') || (a==']') || (a=='<') || (a=='>') || (a=='{') || \
(a=='#') || (a=='^') || (a=='}'))
#define MISSING     33
#define PERIOD      46
#define SPACE       32
#define LCHAR       255
#define IOTASUBCODE 124

typedef struct
        {
            BYTE charcode;
            char dc_codes[3];
            BYTE dc_num;
            BYTE attrib;
        } BETALETTER;

extern BETALETTER   empty_betaletter;

void PrintGreekString( int, int, char *);
int PrintGreekChar( int, int , BETALETTER );
char *GetBetaLetter( char *, BETALETTER * );
void AddToBetaLetter( char, BETALETTER * );
char *GetBetaLine( char *, BETALETTER *, int, int * );

#endif
