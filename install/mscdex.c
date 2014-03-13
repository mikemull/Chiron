#include <stdio.h>
#include <dos.h>

main()
{
    union REGS inregs;
    union REGS outregs;
    struct SREGS segregs;
    int i=0;

    inregs.x.ax = 0x150B;
    inregs.x.bx = 0x0000;
    inregs.h.ch = 0x00;
    inregs.h.cl = 0x00;  /* drive number A=0 */
    int86x(0x2f,&inregs,&outregs,&segregs);

    if( outregs.x.bx == 0xadad )
        puts("MSCDEX Detected !");
    else
        puts("No MSCDEX");

    for(i=0;i<5;i++)
    {
       inregs.x.ax = 0x150B;
       inregs.x.bx = 0x0000;
       inregs.h.ch = 0x00;
       inregs.h.cl = i;      /* drive number A=0 */
       int86x(0x2f,&inregs,&outregs,&segregs);
       if( outregs.x.ax != 0x0000 )
           printf("%c is a CD_ROM\n",'A'+i);
       else
           printf("%c is not a CD_ROM \n",'A'+i);
    }
}

