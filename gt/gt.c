
#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>

#include "g.fnt"

int curchar;

static int chk[8][8];
FILE *fp,*font;

main()
{
        int gdriver=CGA,gmode=CGAHI,c,xpos=0,ypos=0,i,j,JustDeleted=0;
	void *buffer;
	void makechar();

        initgraph( &gdriver, &gmode, "C:\\LANG\\TC\\BGI" );

	fp=fopen("g.chr","w");
    for(i=0;i<8;i++)
        for(j=0;j<8;j++)
			chk[i][j]=0;

	setfillstyle(SOLID_FILL,1);
        rectangle(19,19,101,101);
	bar(xpos*10+20,ypos*10+20,xpos*10+30,ypos*10+30);
	getimage(20,20,30,30,buffer=malloc(imagesize(20,20,30,30)));

        while((c=getch()) != 27 )
	{
             rectangle(19,19,101,101);
             if(isprint(c))
             {
                curchar = c;
                putimage(500,100,&Greek[c][0],COPY_PUT);
                for(i=0;i<8;i++)
                {
                        for(j=0;j<8;j++)
                        {
                                chk[i][j]=getpixel(500+i,100+j);
                                putimage(i*10+20,j*10+20,buffer,
                                (chk[i][j] ? COPY_PUT : NOT_PUT));
                        }
                }
             }
             else if ( c == 0 )
             {

		if(chk[xpos][ypos]==0)
			putimage(xpos*10+20,ypos*10+20,buffer,XOR_PUT);
		switch(c=getch())
		{
            case 77: xpos = (xpos>6 ? xpos : xpos+1 );
				break;
			case 75: xpos = (xpos<1 ? 0 : xpos-1 );
				break;
			case 72: ypos = (ypos<1 ? 0 : ypos-1 );
				break;
            case 80: ypos = (ypos>6 ? ypos : ypos+1);
				break;
			case 82: putpixel(500+xpos,100+ypos,1);
				chk[xpos][ypos]=1;
				break;
			case 83: putpixel(500+xpos,100+ypos,0);
				 chk[xpos][ypos]=0;
				 JustDeleted=1;
				break;
                        case 73:
                                setfillstyle(EMPTY_FILL,0);
                                bar(499,99,520,120);
                                setfillstyle(SOLID_FILL,1);
                                for(i=0;i<8;i++)
                                {
                                   for(j=0;j<7;j++)
                                   {
                                        chk[i][j] = chk[i][j+1];
                                        putimage(i*10+20,j*10+20,buffer,
                                        (chk[i][j] ? COPY_PUT : NOT_PUT));
                                        if(chk[i][j])putpixel(500+i,100+j,1);
                                   }
                                }
                                break;
                        case 71:
                                setfillstyle(EMPTY_FILL,0);
                                bar(499,99,520,120);
                                setfillstyle(SOLID_FILL,1);
                                for(i=0;i<7;i++)
                                {
                                   for(j=0;j<8;j++)
                                   {
                                        chk[i][j] = chk[i+1][j];
                                        putimage(i*10+20,j*10+20,buffer,
                                        (chk[i][j] ? COPY_PUT : NOT_PUT));
                                        if(chk[i][j])putpixel(500+i,100+j,1);
                                   }
                                }
                                break;
                        case 59: makechar();
				break;
                        case 60:
                                getimage(500,100,507,107,&Greek[curchar][0]);
                                break;
		}
		if(!JustDeleted)
		putimage(xpos*10+20,ypos*10+20,buffer,
			 (chk[xpos][ypos] ? COPY_PUT : XOR_PUT));
		JustDeleted=0;
           }

	}
	closegraph();
	fclose(fp);
}

void
makechar()
{
	int i,j,cn=0,Size;
	unsigned int *gchar;

        Size = imagesize(0,0,7,7);

        for(j=0;j<128;j++)
        {
	fprintf(fp,"{");
	for(i=0;i<Size;i++)
	{
		if(!(i%8)) fprintf(fp,"\n");
                fprintf(fp,"0x%-4x,",Greek[j][i]);
	}
	fprintf(fp,"},\n");
        }
}
