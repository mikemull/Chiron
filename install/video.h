#ifndef video_h
#define video_h

void VideoInit( void );
int Select( int, char* );
char *GetStrField( int, char *, int );
int SelectStrH( int, char **, char **, int );
int SelectStrV( int, char **, char **, int );
void FillScreen( void );
void Description( int, char * );
void box_window( void );
#endif
