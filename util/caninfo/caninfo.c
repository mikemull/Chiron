/*====================================================================
 *    caninfo                09.19.90  GH
 *
 *    Takes an Author and optional work number and gets the
 *    Canon Bibliography entry from the doccan1.[txt][idt] files
 *    on the 'D' TLG CD-ROM
 *
 *===================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "tlg.h"
#include "tlgmem.h"
#include "alloc.h"
#include "text.h"
#include "init.h"
#include "canon.h"

BYTE *CANIDTbuf;
FILE *TLGcantxt;

main( int argc, char *argv[] )
{
   FILE *output;
   int auth_num, work_num;
   char **bibliography;
   FILE *canidtfile;
   int args;
   int i;


   /*********************************************
    **  Process Command Line Arguments
    ********************************************/
   output = stdout;
   args = 1;
   while(args<argc)
   {
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
      if( !stricmp(argv[args],"-a" )  )
      {
         args++;
         auth_num = atoi( argv[args]);
         args++;
         if( !auth_num )
         {
            puts("invalid author number ");
            exit(0);
         }
      }
      if( !stricmp(argv[args],"-w" )  )
      {
         args++;
         work_num = atoi( argv[args]);
         args++;
         if( !work_num )
         {
            puts("invalid work_number ");
            exit(0);
         }
      }
   }

   /*********************************************
    **  Initialize global Canon FILE * and IDT buffer
    ********************************************/
   CANIDTbuf = ( BYTE * ) calloc( DOCCAN1SIZE, sizeof(BYTE ) );
   canidtfile = fopen("e:doccan1.idt","rb");
   CHECKREAD( CANIDTbuf, sizeof(BYTE), DOCCAN1SIZE, canidtfile );
   fclose( canidtfile );
   TLGcantxt = fopen("e:doccan1.txt","rb");
   Tcl_ValidateAllMemory(__FILE__,__LINE__);
   bibliography = CanBibGetBiblio( auth_num, work_num );
   Tcl_ValidateAllMemory(__FILE__,__LINE__);
   i=0;
   while( bibliography[i] != NULL )
   {
      fprintf(output,"%s\n",bibliography[i]);
      i++;
   }

   if( output != stdout )
      fclose( output );
   Tcl_ValidateAllMemory(__FILE__,__LINE__);
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

        while( ((block-beg_block)<8192) && (*block != 0) )
	{
            cit_index[line_count] = (char *) block;
            SKIPHIGH(block);
            line_index[line_count] = (char *) block;
            while( !( HIBITSET( *block )) && ( *block ) )
               block++;
	    line_count++;
	}
        return( line_count );
}/*
END of IndexBlock */

