/**************************************
 **    TXTCIT.C
 **
 **    Utility/test code to read a tlg .txt
 **    file and decode the text printing each
 **    line with it's decoded citation
 **
 **************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tlg.h"
#include "intr.h"
#include "tlgmem.h"
#include "tclckall.h"
#include "citlow.h"
#include "citation.h"

#define SKIPHIGH(a)  while( HIBITSET(*(a)) ) (a)++

static  citation_t *citation;

main(int argc, char *argv[] )
{
    char *buffer;
    FILE *input,*output;
    int fcnt=8192,block=0;
    int start_block =0,end_block = 0;
    int args=0,result=1;

    input = stdin;
    output = stdout;
    args = 1;
    if( argc < 1 )
    {
       printf("Usage : %s [-i input-file] [-o output-file] [-sb startblock] [eb end-block]",argv[0]);
       printf("         input defaults to stdin; output defaults to stdout\n");
       exit(1);
    }
    while(args<argc)
    {
        if( !stricmp(argv[args],"-i" ) )
        {
           args++;
           input = fopen(argv[args],"rb");
           args++;
           if( !input )
           {
              puts("error opening input file ");
              exit(0);
           }
        }
        if( !stricmp(argv[args],"-o" )  )
        {
           args++;
           output = fopen(argv[args],"w+");
           args++;
           if( !output )
           {
              puts("error opening output file ");
              exit(0);
           }
        }
        if( !stricmp(argv[args],"-sb" )  )
        {
           args++;
           start_block = atoi(argv[args]);
           args++;
        }
        if( !stricmp(argv[args],"-eb" )  )
        {
           args++;
           end_block = atoi(argv[args]);
           args++;
        }
    }
    if( start_block )
    {
      if( end_block )
      {
        if( end_block <= start_block )
        {
          end_block = start_block + 2;
        }
      }
      else
      {
         end_block = start_block + 2;
      }
    }
    
    citation = ( citation_t * ) malloc( sizeof(citation_t));
    if( citation == ( citation_t * ) NULL )
    {
        puts("error malloc'ing citation structure ");
        exit(0);
    }
    buffer = ( char * ) malloc( 8200 );
    if( buffer == ( char * ) NULL )
    {
        puts("error malloc'ing buffer ");
        exit(0);
    }
    fcnt=8192;
    while( fcnt == 8192 )
    {
       fcnt = fread( buffer,1,8192,input );
       if( fcnt == 0 ) break;
       if( start_block == 0  )
       {
          result = process_block( output, buffer, block );
          if( result == 0 ) break;
       }
       else
       {
          if( ( block >= start_block ) && ( block <= end_block ) )
          {
             result = process_block( output, buffer, block );
             if( result == 0 ) break;
          }
       }
       block++;
    }
    if( input != stdin )
       fclose( input );
    if( output != stdout )
       fclose( output );
    free(buffer);
    free(citation);
    puts("ALL DONE!");

}

/*---- PROCESSBLOCK ---- */
int
process_block( FILE *output, BYTE *buffer, int block )
{

    char *lines[8000], *cits[8000], *line_ptr;
    char txt_line[1000];
    int line_count=0,line_index=0,char_index=0;
    int fcnt=8192;
    int start_block =0,end_block = 0;

    line_count = IndexBlock(buffer,lines,cits);
    fprintf(output,"Block %i *************************************\n",
                    block);
    if( line_count == 0 ) return(0);
    for(line_index=1;line_index<=line_count;line_index++)
    {
        if( lines[line_index-1] == 0x00 )
           break;
        line_ptr = lines[line_index-1];
        char_index=0;
        while( !HIBITSET(*line_ptr) )
        {
           txt_line[char_index] = *line_ptr;
           char_index++;
           line_ptr++;
        }
        txt_line[char_index] = 0x00;
        CitFromTextIndex(cits,line_index-1,citation);
        fprintf(output,"( %i )->%s.%s.%s.%s.%s.%s.%s --  %s\n",
                line_index,
                CIT_VALUE(CIT_AUTHOR),
                CIT_VALUE(CIT_WORK),
                CIT_VALUE(CIT_V),
                CIT_VALUE(CIT_W),
                CIT_VALUE(CIT_X),
                CIT_VALUE(CIT_Y),
                CIT_VALUE(CIT_Z),
                txt_line);
    }
    fflush(output);
    return(1);
}
/*---------------------------------------------------------
 *    IndexBlock             04-03-91
 *
 *    This routine indexs a block of text pointing one
 *    element of the array line_index at the beginning of
 *    each text line and one element of cit_index at the
 *    beginning of each citation entry.
 *
 *    last modified : 04-19-91
 *---------------------------------------------------------
 */
int
IndexBlock( BYTE *block, char *line_index[], char *cit_index[] )
{
	int line_count = 0;
    BYTE *beg_block=block;

    while(((block-beg_block)<8192) && (*block != 0x00 ) &&
          (*block != 0xFE ))
    {
        cit_index[line_count] = (char *) block;
        SKIPHIGH(block);
        line_index[line_count] = (char *) block;
        while( !( HIBITSET( *block )) && ( *block ) )
            block++;
        line_count++;
	}
    line_index[line_count] = 0x00;
    return( line_count );
}/*
END of IndexBlock */
