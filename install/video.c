/*===============================================
 *     Video.c          11-21-91  GH
 *
 *     Set of routines to handle configuration
 *     program interface
 *-----------------------------------------------
 */
#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <dir.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include <alloc.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <math.h>
#include <io.h>
#include <string.h>

#include "video.h"

/* EXTERNALS */

struct text_info vi;
unsigned char rv_attr;
unsigned char *bgfill;
int oldxl,oldxr,oldyt,oldyb;

/* MACROS */

#define NORMAL  vi.attribute
#define CR    0x0D
#define SPACE 0x20
#define SCR_HT vi.screenheight
#define SCR_WD vi.screenwidth

/*  FUNCTIONS */

void
VideoInit()
{
     unsigned char foretemp,backtemp;
     int i=0;

     gettextinfo(&vi);
     backtemp = ( NORMAL & 0x0F ) > 7 ? 7 : ( NORMAL & 0x0F );
     foretemp  = (( NORMAL & 0x70 ) >> 4);
     rv_attr = (( backtemp << 4 ) | ( foretemp ));
     bgfill = ( unsigned char * ) calloc(SCR_HT*SCR_WD*2,sizeof(char));
     for(i=0;i<SCR_HT*SCR_WD*2;i+=2)
     {
        bgfill[i] = 177;
        bgfill[i+1] = NORMAL;
     }
}

int
Select( int row, char *options )
{
    int len,i=0,inval=0,wsize=0;

    len = strlen(options);
    if( len % 2 )
      wsize = len + 1;
    else
      wsize = len + 2;

    window(((vi.screenwidth-wsize)/2),row,
           ((vi.screenwidth+wsize)/2),row);

    textattr(rv_attr);
    clrscr();
    for(i=0;i<len;i++)
    {
       gotoxy(i+2,1);
       putch(options[i]);
    }
    i=0;
    textattr(NORMAL);
    gotoxy(i+2,1);
    putchar(options[i]);
    while(( inval = getch() ) != CR )
    {
       if( inval == 0 )
       {
          switch( getch() )
          {
            case 75:  textattr(rv_attr);
                      gotoxy(i+2,1);
                      putch(options[i]);
                      if( i == 0 )
                         i = len-1;
                      else
                         i--;
                      gotoxy(i+2,1);
                      textattr(NORMAL);
                      putch(options[i]);
                      break;
            case 77:  textattr(rv_attr);
                      gotoxy(i+2,1);
                      putch(options[i]);
                      if( i == (len-1) )
                         i = 0;
                      else
                         i++;
                      gotoxy(i+2,1);
                      textattr(NORMAL);
                      putch(options[i]);
                      break;
            default:  break;
          }
       }
    }
    textattr(NORMAL);
    return(i);
}

char *
GetStrField( int row, char *defstr, int len )
{
        int wsize=0,x=0,inval=0;
        char *newpath;

        if( len == 0 )
           wsize = strlen(defstr);
        else
           wsize = len;
        if( wsize % 2 )
           wsize++;
 
        newpath = (char * ) calloc(wsize,sizeof(char));
        strcpy(newpath,defstr);
        window(((vi.screenwidth-wsize)/2),row,
               ((vi.screenwidth+wsize)/2),row);
        textattr(rv_attr);
        clrscr();
        cputs(newpath);
        while( ( inval = getch() ) != CR )
        {
            x = wherex();
            switch(inval)
            {
             case 0:  switch( getch() )
                      {
                        case 75:  gotoxy(x-1,1);
                                  break;
                        case 77:  gotoxy(x+1,1);
                                  break;
                        default:  break;
                      }
                      break;
             case 8:  if( x > 1 )
                      {
                         gotoxy(x-1,1);
                         putch(SPACE);
                         gotoxy(x-1,1);
                      }
                      break;
             case 27: gotoxy(1,1);
                      clreol();
                      break;
             default: if( isascii(inval) )
                      {
                         putch(inval);
                         newpath[x-1] = inval;
                      }
                      break;
            }
        }
        x = wherex();
        newpath[x-1] = NULL;
        return(newpath);
}

int
SelectStrH( int row, char *options[], char *option_desc[], int option_cnt )
{
       int field_width=15,wsize=0,i=0,inval=0;
       int show_desc;

       show_desc = (option_desc==NULL) ? 0 : 1;
       wsize = option_cnt * field_width;
       if( vi.screenwidth < wsize)
          return(-1);

       textattr(NORMAL);
       window(vi.winleft,vi.wintop,vi.winright,vi.winbottom);
       for(i=0;i<option_cnt;i++)
       {
         gotoxy((i*field_width)+(wsize/2),row);
         cputs(options[i]);
       }
       i=0;
       gotoxy((i*field_width)+(wsize/2),row);
       textattr(rv_attr);
       cputs(options[i]);
       if( show_desc) Description(row+2,option_desc[i]);
       window(vi.winleft,vi.wintop,vi.winright,vi.winbottom);
       while(( inval = getch() ) != CR )
       {
          if( inval == 0 )
          {
             switch( getch() )
             {
               case 75:  textattr(NORMAL);
                         gotoxy((i*field_width)+(wsize/2),row);
                         cputs(options[i]);
                         if( i == 0 )
                            i = option_cnt-1;
                         else
                            i--;
                         gotoxy((i*field_width)+(wsize/2),row);
                         textattr(rv_attr);
                         cputs(options[i]);
                         if( show_desc) Description(row+2,option_desc[i]);
                         window(vi.winleft,vi.wintop,vi.winright,vi.winbottom);
                         break;
               case 77:  textattr(NORMAL);
                         gotoxy((i*field_width)+(wsize/2),row);
                         cputs(options[i]);
                         if( i == (option_cnt-1) )
                            i = 0;
                         else
                            i++;
                         gotoxy((i*field_width)+(wsize/2),row);
                         textattr(rv_attr);
                         cputs(options[i]);
                         if( show_desc) Description(row+2,option_desc[i]);
                         window(vi.winleft,vi.wintop,vi.winright,vi.winbottom);
                         break;
               default:  break;
             }
          }
          else if( inval == 27 )
          {
             i=-1;
             break;
          }
       }
       textattr(NORMAL);
       return(i);
}



int
SelectStrV( int row, char *options[], char *option_desc[], int option_cnt )
{
       int field_width=0,wlen=0,i=0,inval=0;

       for(i=0;i<option_cnt;i++)
          field_width = max( field_width,strlen(options[i]));
       wlen = option_cnt;
       if( vi.screenheight < wlen || vi.screenwidth < field_width )
          return(-1);

       textattr(NORMAL);
       window(vi.winleft,vi.wintop,vi.winright,vi.winbottom);
       for(i=0;i<option_cnt;i++)
       {
         gotoxy((vi.screenwidth-field_width)/2, row+i+1);
         cputs(options[i]);
       }
       i=0;
       gotoxy((vi.screenwidth-field_width)/2, row+i+1);
       textattr(rv_attr);
       cputs(options[i]);
       Description((row+option_cnt+8),option_desc[i]);
       window(vi.winleft,vi.wintop,vi.winright,vi.winbottom);
       while(( inval = getch() ) != CR )
       {
          if( inval == 0 )
          {
             switch( getch() )
             {
               case 72:  textattr(NORMAL);
                         gotoxy((vi.screenwidth-field_width)/2, row+i+1);
                         cputs(options[i]);
                         if( i == 0 )
                            i = option_cnt-1;
                         else
                            i--;
                         gotoxy((vi.screenwidth-field_width)/2, row+i+1);
                         textattr(rv_attr);
                         cputs(options[i]);
                         Description((row+option_cnt+8),option_desc[i]);
                         window(vi.winleft,vi.wintop,vi.winright,vi.winbottom);
                         break;
               case 80:  textattr(NORMAL);
                         gotoxy((vi.screenwidth-field_width)/2, row+i+1);
                         cputs(options[i]);
                         if( i == (option_cnt-1) )
                            i = 0;
                         else
                            i++;
                         gotoxy((vi.screenwidth-field_width)/2, row+i+1);
                         textattr(rv_attr);
                         cputs(options[i]);
                         Description((row+option_cnt+8),option_desc[i]);
                         window(vi.winleft,vi.wintop,vi.winright,vi.winbottom);
                         break;
               default:  break;
             }
          }
          else if( inval == 27 )
          {
             i=-1;
             break;
          }
       }
       textattr(NORMAL);
       return(i);
}

void
FillScreen( void )
{

    textattr(NORMAL);
    puttext(vi.winleft,vi.wintop,vi.winright,vi.winbottom,bgfill);

}

void
Description( int row, char *text )
{
    int w_wd=0, w_ht=0, text_len=strlen(text),word_len=0,line_left=0;
    char *tmp_txt, *curr_word;
    static int oldxl,oldxr,oldyt,oldyb;

    if(( oldxl && oldyt && oldxr && oldyb ) && ( oldyt == row ) )
    {
       window( oldxl,oldyt,oldxr,oldyb);
       textattr(NORMAL);
       clrscr();
    }
    w_wd = ( vi.winright - vi.winleft) - 10;
    if( w_wd > text_len+6 )
        w_wd = text_len+6;
    w_ht = ( int ) ceil( ( float )text_len /(float)w_wd );
    tmp_txt = ( char * ) calloc( text_len,sizeof(char));
    strcpy(tmp_txt,text );
    window((vi.screenwidth-w_wd)/2,row,(vi.screenwidth+w_wd)/2,row+w_ht+2);
    textattr(NORMAL);
    clrscr();
    box_window();
    oldxl = ( vi.screenwidth-w_wd) /2;
    oldxr = ( vi.screenwidth+w_wd)/2;
    oldyt = row;
    oldyb = row+w_ht+1;
    gotoxy(4,2);
    line_left = w_wd-4;
    textattr(NORMAL);

    curr_word = strtok( tmp_txt," ");
    while( curr_word )
    {
        word_len = strlen(curr_word);
        if( line_left > word_len )
        {
           cprintf("%s ",curr_word);
           line_left -= word_len+1;
        }
        else
        {
           cputs("\n\r");
           line_left = w_wd-4;
           gotoxy(4,wherey());
           cprintf("%s ",curr_word);
           line_left -= word_len+1;
        }
        curr_word = strtok(NULL," ");
    }
    free(tmp_txt);
}

void
box_window( void )
{
    struct text_info win_info;
    static char box_chars[6] = "ɻȼͺ";
    int i,w_ht;

    gettextinfo( &win_info );
    w_ht = win_info.winbottom - win_info.wintop;
    textattr(NORMAL);

    gotoxy(1,1);
    putch(box_chars[0]);
    for(i=1;i<win_info.winright-win_info.winleft-1;i++)
       putch(box_chars[4]);
    putch(box_chars[1]);

    for(i=2;i<(w_ht);i++)
    {
       gotoxy(1,i);
       putch(box_chars[5]);
       gotoxy(win_info.winright-win_info.winleft,i);
       putch(box_chars[5]);
    }

    gotoxy(1,w_ht);
    putch(box_chars[2]);
    for(i=1;i<win_info.winright-win_info.winleft-1;i++)
       putch(box_chars[4]);
    putch(box_chars[3]);
}
