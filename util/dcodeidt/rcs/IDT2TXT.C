head     1.2;
branch   ;
access   ;
symbols  ;
locks    ROOT_DOS:1.2; strict;
comment  @ * @;


1.2
date     93.02.04.00.31.11;  author ROOT_DOS;  state Exp;
branches ;
next     1.1;

1.1
date     93.02.01.01.58.33;  author ROOT_DOS;  state Exp;
branches ;
next     ;


desc
@  .IDT file to text translator
@


1.2
log
@Modified to use new citlow.c
@
text
@/*====================================================================
 *    idt2txt                09.19.90  GH
 *
 *    Reads a .idt file and produces a translated text file.
 *
 *===================================================================
 */
#include <stdio.h>
#include "tlg.h"
#include "citlow.h"

main()
{
   char infile[30],outfile[30],blank[6];
   BYTE buffer[8194], *buf_ptr;
   int typecode=0,excepblk=0,bytes_read=0;
   unsigned len=0,loc=0,dummy=0,block=0;
   unsigned long ldummy=0L;
   FILE *f,*o;
   citation_t *citation;
   BOOL NewSection=FALSE,NewAuth=FALSE,NewWork=FALSE;
   long floc=0;

   citation = ( citation_t * )calloc(1,sizeof(citation_t));
   puts("Enter file to be translated");
   gets(infile);
   if( ( f = fopen(infile,"rb") ) == NULL )
   {
      printf("\nERROR OPENING %s. PROGRAM ABORTING!!",infile);
      exit(99);
   }
   puts("\nEnter output file name");
   gets(outfile);
   if( ( o = fopen(outfile,"w+") ) == NULL )
   {
      printf("\nERROR OPENING %s. PROGRAM ABORTING!!",outfile);
      exit(99);
   }

   bytes_read = fread(buffer,1,8192,f);
   buf_ptr = buffer;
   do
   {
      while( ( *buf_ptr != '\0' ) && ( buf_ptr ) )
      {
         /*
         fprintf(o,"\n-- TC = %i --",(int *) buf_ptr);
         */
         switch( *buf_ptr )
         {
            case TYPE_NEWAUTH :  InitCit(citation);
                                 buf_ptr=DecodeNewAuthWork(buf_ptr,&len,
                                                           &loc,citation);
                                 NewAuth = TRUE;
                                 break;
            case TYPE_NEWWORK :  buf_ptr=DecodeNewAuthWork(buf_ptr,&len,
                                                           &loc,citation);
                                 NewWork = TRUE;
                                 break;
            case TYPE_NEWSECTION : buf_ptr=DecodeNewSection(buf_ptr,&block);
                                   NewSection = TRUE;
                                   break;
            case TYPE_NEWFILE : buf_ptr=DecodeNewFile(buf_ptr,&dummy,&ldummy,
                                                      &dummy,citation);
                                fprintf(o,"---NEW FILE FOUND - Ignoring --\n");
                                break;
            case TYPE_BEGCIT  : buf_ptr=DecodeSectionStartCit( buf_ptr,citation);
                                fprintf(o,"Section Starting ID %s.%s.%s.%s.%s.%s.%s.%s\n",
                                           (*citation)[0].level_val,
                                           (*citation)[1].level_val,
                                           (*citation)[2].level_val,
                                           (*citation)[3].level_val,
                                           (*citation)[4].level_val,
                                           (*citation)[5].level_val,
                                           (*citation)[6].level_val,
                                           (*citation)[7].level_val );
                                break;
            case TYPE_ENDCIT  : buf_ptr=DecodeSectionEndCit( buf_ptr, citation );
                                fprintf(o,"Section Ending ID %s.%s.%s.%s.%s.%s.%s.%s\n",
                                           (*citation)[0].level_val,
                                           (*citation)[1].level_val,
                                           (*citation)[2].level_val,
                                           (*citation)[3].level_val,
                                           (*citation)[4].level_val,
                                           (*citation)[5].level_val,
                                           (*citation)[6].level_val,
                                           (*citation)[7].level_val );
                                break;
            case TYPE_LASTCIT : buf_ptr=DecodeBlockEndCit( buf_ptr, citation );
                                if( NewSection )
                                   NewSection = FALSE;
                                else
                                   block++;
                                fprintf(o,"Last ID in Block %i.  %s.%s.%s.%s.%s.%s.%s.%s\n",
										   block,
                                           (*citation)[0].level_val,
                                           (*citation)[1].level_val,
                                           (*citation)[2].level_val,
                                           (*citation)[3].level_val,
                                           (*citation)[4].level_val,
                                           (*citation)[5].level_val,
                                           (*citation)[6].level_val,
                                           (*citation)[7].level_val );
                                break;
            case TYPE_BEGEXCEPT : buf_ptr=DecodeExceptionStart(buf_ptr, &dummy,
                                                               citation );
                                  fprintf(o,"Exception Beg.in block %i.  %s.%s.%s.%s.%s.%s.%s.%s\n",
											 excepblk,
                                             (*citation)[0].level_val,
                                             (*citation)[1].level_val,
                                             (*citation)[2].level_val,
                                             (*citation)[3].level_val,
                                             (*citation)[4].level_val,
                                             (*citation)[5].level_val,
                                             (*citation)[6].level_val,
                                             (*citation)[7].level_val );
                                  break;
            case TYPE_ENDEXCEPT : buf_ptr=DecodeExceptionEnd(buf_ptr, citation );
                                  fprintf(o,"Exception End.  %s.%s.%s.%s.%s.%s.%s.%s\n",
                                             (*citation)[0].level_val,
                                             (*citation)[1].level_val,
                                             (*citation)[2].level_val,
                                             (*citation)[3].level_val,
                                             (*citation)[4].level_val,
                                             (*citation)[5].level_val,
                                             (*citation)[6].level_val,
                                             (*citation)[7].level_val );
                                  break;
            case TYPE_AWDESC : buf_ptr=DecodeAuthWorkDesc(buf_ptr,citation);
                               if( NewAuth )
                               {
                                  fprintf(o,"New Author %s. %i bytes long, begins in block %i\n",
                                             (*citation)[0].level_desc,len,loc);
                                  NewAuth = FALSE;
                               }
                               else if( NewWork )
                               {
                                  fprintf(o,"New Work %s. %i bytes long, begins in block %i\n",
                                             (*citation)[1].level_desc,len,loc);
                                  NewWork = FALSE;
                               }
                               break;
            case TYPE_LLDESC  : buf_ptr=DecodeLowLevDesc(buf_ptr,citation);
                                fprintf(o,"Low Level Description= %s,%s,%s,%s,%s\n",
                                           (*citation)[3].level_desc,
                                           (*citation)[4].level_desc,
                                           (*citation)[5].level_desc,
                                           (*citation)[6].level_desc,
                                           (*citation)[7].level_desc );
                                break;
            case TYPE_HEADER  : buf_ptr=DecodeCombinedHeader(buf_ptr );
                                fprintf(o,"---Combined Header Found\n");
                                break;
            default           : if( *buf_ptr > TYPE_HEADER )
                                   SYSTEMCRASH(EXIT_BADTYPECODE);
                                break;
         }

      }
   }while( ( bytes_read = fread(buffer,1,8192,f) ) == 8192 );
  close(f);
  close(o);
  free( citation );
}
@


1.1
log
@Initial revision
@
text
@d8 3
d14 36
a49 28

  #include <stdio.h>;
  #include "tlg.h";

  char infile[30],outfile[30],blank[6];
  int typecode=0,len=0,loc=0,block=0,excepblk=0;
  FILE *f,*o;
  citation_type cit;
  BOOL NewSection=FALSE,NewAuth=FALSE,NewWork=FALSE;
  long floc=0;


  puts("Enter file to be translated");
  gets(infile);
  if( ( f = fopen(infile,"rb") ) == NULL )
     printf("\nERROR OPENING %s. PROGRAM ABORTING!!",infile);
  else
  {
     puts("\nEnter output file name");
     gets(outfile);
     if( ( o = fopen(outfile,"w+") ) == NULL )
        printf("\nERROR OPENING %s. PROGRAM ABORTING!!",outfile);
     else
     {
       while((typecode = fgetc(f)) != 0x00)
       {
         fprintf(o,"\n-- TC = %i --",typecode);
         switch(typecode)
d51 107
a157 22
            case 1 :  InitCit(&cit);
                      DecodeType1or2(f,&len,&loc,&cit);
                      cit.level_a.level_desc[0] = 0x00;
                      NewAuth = TRUE;
                      break;

            case 2 :  DecodeType1or2(f,&len,&loc,&cit);
                      cit.level_b.level_desc[0] = 0x00;
                      NewWork = TRUE;
                      break;

            case 3 :  DecodeType3(f,&block);
                      NewSection = TRUE;
                      break;

            case 4 :
            case 5 :
            case 6 :  break;

            case 7 :  fprintf(o,"---TYPE CODE 7 - Ignoring --\n");
                      fread(blank,6,1,f);
                      break;
d159 2
a160 115
            case 8 :  DecodeBinCit(f,&cit);
                      fprintf(o,"Section Starting ID %s.%s.%s.%s.%s.%s.%s.%s\n",
                         cit.level_a.level_val,
                         cit.level_b.level_val,
                         cit.level_c.level_val,
                         cit.level_v.level_val,
                         cit.level_w.level_val,
                         cit.level_x.level_val,
                         cit.level_y.level_val,
                         cit.level_z.level_val);
                      break;

            case 9 :  DecodeBinCit(f,&cit);
                      fprintf(o,"Section Ending ID %s.%s.%s.%s.%s.%s.%s.%s\n",
                         cit.level_a.level_val,
                         cit.level_b.level_val,
                         cit.level_c.level_val,
                         cit.level_v.level_val,
                         cit.level_w.level_val,
                         cit.level_x.level_val,
                         cit.level_y.level_val,
                         cit.level_z.level_val);
                      break;

            case 10 : floc = ftell(f);
                      if( floc >=864 )
                         floc = floc;
                      DecodeBinCit(f,&cit);
                      if(!NewSection)
                         block++;
                      else
                         NewSection = FALSE;
                      fprintf(o,"Last ID in Block %i.  %s.%s.%s.%s.%s.%s.%s.%s\n",
                         block,cit.level_a.level_val,
                         cit.level_b.level_val,
                         cit.level_c.level_val,
                         cit.level_v.level_val,
                         cit.level_w.level_val,
                         cit.level_x.level_val,
                         cit.level_y.level_val,
                         cit.level_z.level_val);
                      break;

            case 11 : fread(blank,2,1,f);
                      excepblk = BitVal16(blank,1);
                      DecodeBinCit(f,&cit);
                      fprintf(o,"Exception Beg.in block %i.  %s.%s.%s.%s.%s.%s.%s.%s\n",
                         excepblk,cit.level_a.level_val,
                         cit.level_b.level_val,
                         cit.level_c.level_val,
                         cit.level_v.level_val,
                         cit.level_w.level_val,
                         cit.level_x.level_val,
                         cit.level_y.level_val,
                         cit.level_z.level_val);
                      break;

            case 12 : DecodeBinCit(f,&cit);
                      fprintf(o,"Exception End.  %s.%s.%s.%s.%s.%s.%s.%s\n",
                         cit.level_a.level_val,
                         cit.level_b.level_val,
                         cit.level_c.level_val,
                         cit.level_v.level_val,
                         cit.level_w.level_val,
                         cit.level_x.level_val,
                         cit.level_y.level_val,
                         cit.level_z.level_val);
                      break;

            case 13 :
            case 14 :
            case 15 :   break;

            case 16 : DecodeType16(f,&cit);
                      if( NewAuth )
                      {
                         fprintf(o,"New Author %s. %i bytes long, begins in block %i\n",
                            cit.level_a.level_desc,len,loc);
                         NewAuth = FALSE;
                      }
                      else if( NewWork )
                      {
                         fprintf(o,"New Work %s. %i bytes long, begins in block %i\n",
                            cit.level_b.level_desc,len,loc);
                         NewWork = FALSE;
                      }
                      break;

            case 17 : DecodeType17(f,&cit);
                      fprintf(o,"Low Level Description= %s,%s,%s,%s,%s\n",
                         cit.level_v.level_desc,
                         cit.level_w.level_desc,
                         cit.level_x.level_desc,
                         cit.level_y.level_desc,
                         cit.level_z.level_desc);
                      break;

            case 18 :
            case 19 :
            case 20 :
            case 21 :
            case 22 :
            case 23 :
            case 24 :
            case 25 :
            case 26 :
            case 27 :
            case 28 :
            case 29 :
            case 30 :
            case 31 :   break;
         }
       }
     }
  }
d163 1
@
