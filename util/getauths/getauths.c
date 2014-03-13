main()
{
  #include "stdio.h";
  #include "ctype.h";

  FILE *atab, *output;
  char filename[9],authname[70],rem[30],filesize[2],lang[2];
  char syn[5] [50];
  int index=0,code=0,more=0,i=0,ii=0,iii=0,iiii=0,j=0,check=0,test=0,idiot,c=0,count=0;
  atab = fopen("E:AUTHTAB.DIR","rb");
  output = fopen("ATABLSTD.TXT","wt");

  fputs("--FILE---AUTHOR----------------SYN COUNT---REM--SIZE--LANG\n",output);

  while (1)
  {
    filename[0] = 0x00;
    authname[0] = 0x00;
    i = 0;
    rem[0] = 0x00;
    filesize[0] = 0x00;
    lang[0] = 0x00;
    syn[0] [0] = 0x00;
    syn[1] [0] = 0x00;
    syn[2] [0] = 0x00;
    syn[3] [0] = 0x00;
    syn[4] [0] = 0x00;

    fread(filename,8,1,atab);

    if(!isalpha(filename[0]) && (filename[0] != '*'))
    {
      for ( idiot=0;idiot<=7;idiot++)
	 filename[idiot] = filename[idiot+1];
      filename[7] = 0x20;
    }
    filename[8] = 0x00;

    if( !strcmp(filename,"*END"))
      break;

    while( ( test = fgetc(atab) ) <= 0x7f )
      authname[index++] = test;
    idiot = 0;


    if(!isalpha(authname[0]))
    {
      for(idiot=0;idiot<index;idiot++)
	authname[idiot] = authname[idiot+1];
      authname[idiot-1] = 0x00;
    }

    if ( idiot ==0 )
       authname[index] = 0x00;
    idiot=0;
    index=0;
    ungetc(test,atab);

    while (( code = fgetc(atab)) != 0xff )
      {
	i=0;
	j=0;
	ii=0;
	iii=0;
	iiii=0;
	switch(code)
	  {
	    case 0x80 : ungetc(code,atab);
			while( ( more = fgetc(atab)) == 0x80)
			  {
			    test = 0x00;
			    j=0;
			    while(( test = fgetc(atab)) <= 0x7f)
			      syn[i][j++]=test;
			    ungetc(test,atab);
			    syn[i][j] = 0x00;
			    i++;
			  }
			ungetc(more,atab);
			break;

	    case 0x81 : test=0x00;
			while(( test=fgetc(atab)) <= 0x7f)
			  rem[ii++] = test;
			rem[ii] = 0x00;
			ungetc(test,atab);
			break;

	    case 0x82 : test=0x00;
			while( (test=fgetc(atab)) <=0x7f)
			  filesize[iii++] = test;
			filesize[iii] = 0x00;
			ungetc(test,atab);
			break;

	    case 0x83 : test=0x00;
			while( (test=fgetc(atab)) <0x7f )
			  lang[iiii++] = test;
			lang[iiii] = 0x00;
			ungetc(test,atab);
			break;
	  }
      }
	  fprintf(output,"%s -%s -%i -%s -%s -%s\n",filename,authname,i,rem,filesize,lang);
      count++;
	  if(i>0)
	  {
	    for( c=0;c<i;c++)
	      fprintf(output,"%s |-------  %s\n",filename,syn[c]);
	  }
  }
  fprintf(output,"\n\nTotal Author count is %i\n",count);
  fclose(output);
  fclose(atab);
}
