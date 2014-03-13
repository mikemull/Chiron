#include <stdio.h>
#include "tlg.h"
#include "atab.h"

main()
{
    int authnum=0,key=0, nums[200], i=0;
	BOOL res;
    char authname[100], *authlist[200];
	FILE *atab;

    atab_buf = ( BYTE * ) calloc(23000,sizeof(BYTE));
    atab = fopen("c:\\dev\\chiron\\data\\AUTHTAB.DIR","rb");
    fread(atab_buf,22520,1,atab);
	fclose(atab);
	MakeAtabIndex();
	MakeAnumIndex();
	puts("press space for GetAuths Test, return for NumbertoName");
	key = getch();
	if ( key == 13 )
	{
	  while ( key != 27 )
	  {
	     printf("AuthorNumber ?");
	     scanf("%i",&authnum);
	     res = ATABNumbertoName(authnum,authname);
	     if( res )
		  printf(" Author Number %i = %s \n",authnum,authname);
	     else
		  printf(" ATABNumbertoName failed \n");
	     key = getch();
	  }
	}
	else
	{
	  puts("AuthorName ?");
	  gets(authname);
      authnum = GetAuths(authname,authlist,nums);
      printf(" %i authors found to match %s \n",authnum,authname);
      for( i=0;i<authnum;i++)
         printf("%s ===> %i \n",authlist[i],nums[i]);
      puts("Done");
	  key = getch();
	}
}
