/*===============================================
 *  hardware.c
 *
 *  Turbo C routines to detect hardware for
 *  Installation / configuration purposes.
 *
 *      Some routines based on Pascal
 *      source code for InfoPlus:
 *              InfoPlus
 *            version 1.47
 *            August 2,1991
 *          by Andrew Rossmann
 *  Based on SYSID 4.44, by Steve Grant
 *    Released to the Public Domain
 *===============================================
*/
#include <dos.h>
#include <dir.h>
#include <graphics.h>

#include "install.h"
#include "hardware.h"

/* EXTERNALS */

struct HW hardware;
static char *driver_names[] = { "ILLEGAL","CGA","MCGA",
                                "EGA","EGA64","EGAMONO",
                                "IBM8514","HERCMONO","ATT400",
                                "VGA","PC3270"
                              };
struct MC min_config = { 640, 3, 3, 2, 1, CGA, CGAHI, 350000};
/*
struct MC min_config = { 640, 4, 4, 5, 3, VGA, VGAHI, 20000000 };
*/



/*-----------------------------------------------
 *
 *   DosVer
 *
 *  Get Dos Version Number
 *
 *-----------------------------------------------
 */
void
DosVer()
{
    extern unsigned char _osmajor;
    extern unsigned char _osminor;
    hardware.osmajor = _osmajor;
    hardware.osminor = _osminor;
}


/*-----------------------------------------------
 *
 *   BasicInfo
 *
 *  Get basic info
 *
 *-----------------------------------------------
 */
void
BasicInfo()
{
    struct dfree dt;
    int bpc=0;

    hardware.equip = biosequip();
    hardware.dosmem = biosmemory();
    hardware.currdrv = getdisk();
    getcurdir(hardware.currdrv+1,hardware.cwd);
    getdfree(hardware.currdrv+1,&dt);
    bpc = dt.df_bsec * dt.df_sclus;
    hardware.disk_total = ( long )dt.df_total * bpc;
    hardware.disk_free  = ( long )dt.df_avail * bpc;
}

/*-----------------------------------------------
 *    lastdrv
 *
 *    determines last allowable drive
 *    Set by lastdrive config.sys parameter
 *-----------------------------------------------
*/
void
Lastdrv()
{
    union REGS inregs;
    union REGS outregs;
    struct SREGS segregs;

    inregs.h.ah = 0x52;
    intdosx(&inregs,&outregs,&segregs);
    hardware.lastdrv = peekb(segregs.es,outregs.x.bx + 0x0021);
}

/*-----------------------------------------------
 *    Logdrives
 *
 *    Determines actual number of drives
 *    installed :
 *
 *-----------------------------------------------
*/
void
LogDrives()
{
    union REGS inregs;
    union REGS outregs;
    struct SREGS segregs;
    int drivechar=0,i=0;

    for(drivechar='A';drivechar<='Z';drivechar++)
    {
       inregs.h.ah = 0x0e;
       inregs.h.dl = (drivechar - 'A');
       intdosx(&inregs,&outregs,&segregs);
       inregs.h.ah = 0x19;
       intdosx(&inregs,&outregs,&segregs);
       if(outregs.h.al == outregs.h.dl )
       {
          hardware.drives[i++] = drivechar;
          hardware.drivecount++;
       }
    }
    inregs.h.ah = 0x0e;
    inregs.h.dl = hardware.currdrv;
    intdosx(&inregs,&outregs,&segregs);
}

/*-----------------------------------------------
 *  GetDrives
 *
 *  ** CHEESY ** Simple replacement for LastDrv and
 *  LogDrives routines ( based on INFOPLUS ) because they
 *  weren't detecting the CD-ROM on Barney
 *
 *-----------------------------------------------
 */
void
GetDrives( void )
{
    int max_drive,i=0;
    char drivechar;

    hardware.drivecount=0;
    hardware.lastdrv = setdisk(hardware.currdrv);
    for(drivechar='A';drivechar<'A'+hardware.lastdrv;drivechar++)
    {
       hardware.drives[i++] = drivechar;
       hardware.drivecount++;
    }
}

/*-----------------------------------------------
 *
 *   Diskette
 *
 *  Count Number of diskette drives attached.
 *
 *-----------------------------------------------
 */
void
Diskette()
{
    if( ( hardware.equip & 0x0001 ) == 0x0001 )
       hardware.diskette = 1 + (( hardware.equip & 0x00C0 ) >> 6 );
    else
       hardware.diskette = 0;
}

/*-----------------------------------------------
 *
 *   PrinterInfo
 *
 *   Get Printer / Parallel port info
 *
 *-----------------------------------------------
 */
void
PrinterInfo()
{
    int i=0;

    hardware.parallel = ( hardware.equip & 0xC000 ) >> 14;

    for(i=0;i<hardware.parallel;i++)
    {
       hardware.p_status[i] = biosprint(2,0,i);
    }
}

/*-----------------------------------------------
 *
 *   SerialInfo
 *
 *   Count Serial ports
 *-----------------------------------------------
 */
void
SerialInfo()
{
    hardware.serial = (( hardware.equip & 0x0E00 ) >> 9 );
}

/*-----------------------------------------------
 *
 *   Detcdrom
 *
 *   Determines if CD-ROM exists
 *
 *-----------------------------------------------
 */
void
Detcdrom()
{
    union REGS inregs;
    union REGS outregs;
    struct SREGS segregs;
    int i=0,cdi=0;

    hardware.CD_ROM_count = 0;
    hardware.MSext = 0;
    inregs.x.ax = 0x150B;
    inregs.x.bx = 0x0000;
    inregs.h.ch = 0x00;
    inregs.h.cl = 0x00;  /* drive number A=0 */
    int86x(0x2f,&inregs,&outregs,&segregs);
    if( outregs.x.bx == 0xADAD )
    {
        hardware.MSext = 1;
        for(i=0;i<hardware.drivecount;i++)
        {
           inregs.x.ax = 0x150B;
           inregs.x.bx = 0x0000;
           inregs.h.ch = 0x00;
           inregs.h.cl = i;      /* drive number A=0 */
           int86x(0x2f,&inregs,&outregs,&segregs);
           if( outregs.x.ax != 0x0000 )
           {
              hardware.CD_ROM_count++;
              hardware.CD_ROM_drives[cdi++] = 'A'+i;
           }
        }
    }
}

/*-----------------------------------------------
 *   GetVideo
 *
 *   Determine Video mode
 *-----------------------------------------------
 */
void
GetVideo()
{
    extern struct CFG config;
    int graphmode,graphdriver;

    graphdriver = min_config.graphdriver;
    graphmode = min_config.graphmode;
    detectgraph(&graphdriver,&graphmode);
    if( ( graphdriver == CGA ) || ( graphdriver == MCGA ) ||
        ( graphdriver == EGA ) || ( graphdriver == VGA  ) )
        hardware.graphdriver = min_config.graphdriver;
    else
        hardware.graphdriver = graphdriver;
    config.max_graph_mode = graphmode;
    config.max_graph_driver = graphdriver;
}

/*-----------------------------------------------
 *   Print_hardware
 *
 *   Print the hardware structure
 *-----------------------------------------------
 */
void
Print_hardware()
{
    int i = 0;

    clrscr();
    puts("=============================================");
    printf("Version = %i.%i \n",hardware.osmajor,hardware.osminor);
    puts("Basic Info :");
    printf("equip = %i, dosmem = %i currdrv = %i \n",
            hardware.equip,hardware.dosmem,hardware.currdrv);
    printf("   currdrv = %i, cwd = %s \n",hardware.currdrv,hardware.cwd);
    printf("   total disk on drive %i = %li, free = %li \n",
                             hardware.currdrv,
                             hardware.disk_total,
                             hardware.disk_free);
    printf("Lastdrv returns %i \n",hardware.lastdrv);
    printf("LogDrives returns %i \n",hardware.drivecount);
    printf("Diskettes = %i \n",hardware.diskette);
    puts("Valid Drives are : ");
    for(i=0;i<hardware.drivecount;i++)
       printf(" %c: ",hardware.drives[i]);
    printf("\n");
    printf("Parallel ports = %i \n",hardware.parallel );
    for(i=0;i<hardware.parallel;i++)
        printf("   port LPT%i status = %i \n",i+1,hardware.p_status[i]);
    printf("Serial ports = %i \n",hardware.serial);
    printf("At least CGA detected : %s \n",( hardware.graphdriver ? "yes" : "no"));
    puts("=======================================================");
}

/*-----------------------------------------------
 *   GetHardware
 *
 *   Main hardware detection routine
 *-----------------------------------------------
 */
void
GetHardware()
{
    DosVer();
    BasicInfo();
    Lastdrv();
    LogDrives();
    Diskette();
    PrinterInfo();
    SerialInfo();
    Detcdrom();
    GetVideo();
}

/*-----------------------------------------------
 *    CheckHardware
 *
 *    Routine to verify minimum hardware
 *    requirements are met
 *-----------------------------------------------
 */

int
CheckHardware()
{
    int def[] = {0,0,0,0,0,0};

    if( hardware.dosmem < min_config.dosmem )
       def[0] = 1;

    if(( hardware.osmajor < min_config.osmajor ) ||
       (( hardware.osmajor == min_config.osmajor ) &&
        ( hardware.osminor < min_config.osminor )))
       def[1] = 1;

    if(( hardware.drivecount < min_config.drivecount ))
       def[2] = 1;
    if(( hardware.diskette < min_config.diskette ))
       def[3] = 1;

    if( hardware.graphdriver != min_config.graphdriver )
       def[4] = hardware.graphdriver;

    if( !hardware.MSext )
       def[5] = 1;

    if( def[0] || def[1] || def[2] || def[3] || def[4] || def[5] )
    {
       HardwareFail( def );
       return(0);
    }
    else
       return(1);
}

void
HardwareFail( int *def )
{
    clrscr();
    puts("\n");
    puts("The CHIRON (c) installation program has determined");
    puts("that this computer does not meet the minimum");
    puts("configuration required by CHIRON (c)");
    puts("\n\n");
    puts("Current Config   ------      CHIRON requires");
    puts("------------------------------------------------------");
    if( def[5] )
       puts(" No Microsoft               Microsoft CD-ROM extensions");
       puts(" Extensions");

    if( def[0] )
       printf(" RAM = %li                RAM > %li\n",hardware.dosmem,
                                                     min_config.dosmem);
    if( def[1] )
       printf(" DOS %i.%i                DOS %i.%i or above\n",
                                         hardware.osmajor,hardware.osminor,
                                         min_config.osmajor,min_config.osminor);
    if( def[2] )
       printf(" Drives = %i              At least %i total drives\n",
                                         hardware.drivecount,
                                         min_config.drivecount);
    if( def[3] )
       printf(" Diskettes = %i           At least %i diskettes\n",
                                         hardware.diskette,
                                         min_config.diskette);
    if( def[4] )
       printf(" %s                       At least %s\n",
                     driver_names[hardware.graphdriver],
                     driver_names[min_config.graphdriver] );
    puts("------------------------------------------------------");
    puts("");
}
