/*====================================================================
 *    idt2txt                09.19.90  GH
 *
 *    Reads a .idt file and produces a translated text file.
 *
 *===================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tlg.h"
#include "citlow.h"
#include "citation.h"

main( int argc, char *argv[] )
{
    BYTE *buffer, *buf_ptr;
    unsigned len=0,loc=0,dummy=0,block=0;
    unsigned excepblk=0;
    int args;
    unsigned long ldummy=0L;
    FILE *input=NULL,*output=NULL;
    citation_t *citation;
    BOOL NewSection=FALSE,NewAuth=FALSE,NewWork=FALSE;
    BOOL count_works=FALSE;
    int work_count = 0;

    input = stdin;
    output = stdout;
    args = 1;
    while(args<argc)
    {
        if( !stricmp(argv[args],"-i" ) )
        {
           args++;
           input = fopen(argv[args],"rb");
           args++;
           if( !input )
           {
              fcloseall( );
              fprintf(stderr,"Error opening input file\n");
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
              fcloseall(  );
              fprintf( stderr,"Error opening output file\n");
              exit(0);
           }
        }
        if( !stricmp(argv[args],"-w" ) )
        {
           args++;
           count_works = TRUE;
        }
        if( !stricmp(argv[args],"-n" ) )
        {
            args++;
            output = ( FILE * ) NULL;
        }
    }

   citation = ( citation_t * )calloc(1,sizeof(citation_t));
   if( citation == NULL )
   {
      fprintf(stderr,"Unable to malloc citation \n");
      exit( 0 );
   }

   buffer = CitationLoadFromIdt( input );
   if( buffer == NULL )
   {
       fprintf(stderr,"Failure in CitationLoadFromIdt\n");
       free( citation );
       fcloseall(  );
       exit(0);
   }
   buf_ptr = buffer;

   while( ( *buf_ptr != TYPE_EOF ) && ( *buf_ptr ) && ( buf_ptr ) )
   {
      switch( *buf_ptr )
      {
         case 0x00         :    fprintf(stderr,"Typecode == 0x00\n");
                                fcloseall( );
                                free( buffer );
                                free( citation );
                                exit(1);
                                break;
         case TYPE_NEWAUTH :  InitCit(citation);
                              buf_ptr=DecodeNewAuthWork(buf_ptr,&len,
                                                        &loc,citation);
                              NewAuth = TRUE;
                              break;
         case TYPE_NEWWORK :  buf_ptr=DecodeNewAuthWork(buf_ptr,&len,
                                                        &loc,citation);
                              NewWork = TRUE;
                              if( count_works ) work_count++;
                              break;
         case TYPE_NEWSECTION : buf_ptr=DecodeNewSection(buf_ptr,&block);
                                NewSection = TRUE;
                                break;
         case TYPE_NEWFILE : buf_ptr=DecodeNewFile(buf_ptr,&dummy,&ldummy,
                                                   &dummy,citation);
                             if( output )
                             {
                               fprintf(output,"---NEW FILE FOUND - Ignoring --\n");
                             }
                             break;
         case TYPE_BEGCIT  : buf_ptr=DecodeSectionStartCit( buf_ptr,citation);
                             if( output )
                             {
                               fprintf(output,"Section Starting ID %s.%s.%s.%s.%s.%s.%s.%s\n",
                                        (*citation)[0].level_val,
                                        (*citation)[1].level_val,
                                        (*citation)[2].level_val,
                                        (*citation)[3].level_val,
                                        (*citation)[4].level_val,
                                        (*citation)[5].level_val,
                                        (*citation)[6].level_val,
                                        (*citation)[7].level_val );
                             }
                             break;
         case TYPE_ENDCIT  : buf_ptr=DecodeSectionEndCit( buf_ptr, citation );
                             if( output )
                             {
                                fprintf(output,"Section Ending ID %s.%s.%s.%s.%s.%s.%s.%s\n",
                                        (*citation)[0].level_val,
                                        (*citation)[1].level_val,
                                        (*citation)[2].level_val,
                                        (*citation)[3].level_val,
                                        (*citation)[4].level_val,
                                        (*citation)[5].level_val,
                                        (*citation)[6].level_val,
                                        (*citation)[7].level_val );
                             }
                             break;
         case TYPE_LASTCIT : buf_ptr=DecodeBlockEndCit( buf_ptr, citation );
                             if( NewSection )
                                NewSection = FALSE;
                             else
                                block++;
                             if( output )
                             {
                                fprintf(output,"Last ID in Block %i.  %s.%s.%s.%s.%s.%s.%s.%s\n",
                                        block,
                                        (*citation)[0].level_val,
                                        (*citation)[1].level_val,
                                        (*citation)[2].level_val,
                                        (*citation)[3].level_val,
                                        (*citation)[4].level_val,
                                        (*citation)[5].level_val,
                                        (*citation)[6].level_val,
                                        (*citation)[7].level_val );
                             }
                             break;
         case TYPE_BEGEXCEPT : buf_ptr=DecodeExceptionStart(buf_ptr, &excepblk,
                                                            citation );
                               if( output )
                               {
                                  fprintf(output,"Exception Beg.in block %i.  %s.%s.%s.%s.%s.%s.%s.%s\n",
                                          excepblk,
                                          (*citation)[0].level_val,
                                          (*citation)[1].level_val,
                                          (*citation)[2].level_val,
                                          (*citation)[3].level_val,
                                          (*citation)[4].level_val,
                                          (*citation)[5].level_val,
                                          (*citation)[6].level_val,
                                          (*citation)[7].level_val );
                               }
                               break;
         case TYPE_ENDEXCEPT : buf_ptr=DecodeExceptionEnd(buf_ptr, citation );
                               if( output )
                               {
                                  fprintf(output,"Exception End.  %s.%s.%s.%s.%s.%s.%s.%s\n",
                                          (*citation)[0].level_val,
                                          (*citation)[1].level_val,
                                          (*citation)[2].level_val,
                                          (*citation)[3].level_val,
                                          (*citation)[4].level_val,
                                          (*citation)[5].level_val,
                                          (*citation)[6].level_val,
                                          (*citation)[7].level_val );
                               }
                               break;
         case TYPE_AWDESC : buf_ptr=DecodeAuthWorkDesc(buf_ptr,citation);
                            if( NewAuth )
                            {
                               if( output )
                               {
                                  fprintf(output,"New Author %s. %i bytes long, begins in block %i\n",
                                          (*citation)[0].level_desc,len,loc);
                               }
                               NewAuth = FALSE;
                            }
                            else if( NewWork )
                            {
                               if( output )
                               {
                                  fprintf(output,"New Work %s. %i bytes long, begins in block %i\n",
                                          (*citation)[1].level_desc,len,loc);
                               }
                               NewWork = FALSE;
                            }
                            break;
         case TYPE_LLDESC  : buf_ptr=DecodeLowLevDesc(buf_ptr,citation);
                             if( output )
                             {
                                fprintf(output,"Low Level Description= %s,%s,%s,%s,%s\n",
                                        (*citation)[3].level_desc,
                                        (*citation)[4].level_desc,
                                        (*citation)[5].level_desc,
                                        (*citation)[6].level_desc,
                                        (*citation)[7].level_desc );
                             }
                             break;
         case TYPE_HEADER  : buf_ptr=DecodeCombinedHeader(buf_ptr );
                              if( output )
                              {
                                 fprintf(output,"---Combined Header Found\n");
                              }
                             break;
         case UNKNOWN_13   :
                             buf_ptr = DecodeUnknownType13( buf_ptr );
                             if( output )
                             {
                                fprintf(output,"---Undocumented Typecode 13 Found\n");
                             }
                             break;
         default           : if( *buf_ptr > TYPE_HEADER )
                             {
                                fprintf(stderr,"Bad type code : %i \n",*buf_ptr );
                                fcloseall( );
                                free( buffer );
                                free( citation );
                                exit(1);
                             }
                             break;
      }
   }
   fcloseall();
   free( citation );
   free( buffer );
   if( count_works ) fprintf( stdout,"Work count = %i\n",work_count );
   exit(0);
}
