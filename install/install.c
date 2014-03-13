/*===============================================
 *     Install.c          06-27-91  GH
 *
 *     Set of routines to handle configuration
 *     and installation of CHIRON (c) 1991
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
#include <io.h>
#include <errno.h>
#include <dos.h>
#include <string.h>

#include "hardware.h"
#include "video.h"
#include "install.h"

/* EXTERNALS */

struct CFG config;
int src_file_count = 3;
char *src_files[] = { "chiron.exe","epson_fx.fxf","install.exe" };

static char *src_path;
int  src_drv;
static char *dest_path;
int dest_drv;
extern struct text_info vi;
extern int rv_attr;
BOOL force=FALSE;
BOOL show=FALSE;

static char *title = "CHIRON (c) 1989,1990,1991,1992,1993,1994 \n installation and configuration.";

/* MACROS */

#define NORMAL          vi.attribute
#define SCR_HT          vi.screenheight
#define SCR_WD          vi.screenwidth
#define ROOT            "\\"
#define WINDOW_FULL     vi.winleft,vi.wintop,vi.winright,vi.winbottom
#define WINDOW1         15,5,65,20
#define WINDOW2         10,5,70,20
#define WINDOW3         5,2,75,23
#define WIN_HOME        1,1
#define ESC             27

/*  FUNCTIONS */

/*-----------------------------------------------
 *   CheckPath
 *
 *   Verify that a path exists and begins with a \
 *   return 0 for success, -1 for failure.
 *-----------------------------------------------
 */

int
CheckPath( int drive, char *path )
{
    int tmp_drv,res;
    char tmp_path[MAXPATH];

    tmp_drv = getdisk();
    getcurdir(0,tmp_path);
    if( path[0] != '\\' )
    {
        memmove(path+1,path,strlen(path));
        path[0] = '\\';
    }

    setdisk(drive);
    res  = chdir(path);
    setdisk(tmp_drv);
    chdir(ROOT);
    chdir(tmp_path);
    return(res);
}



/*-----------------------------------------------
 *   MakePath
 *
 *   Create path on drive
 *   return 0 for success, -1 for failure.
 *-----------------------------------------------
 */

int
MakePath( int drive, char *path )
{
    int tmp_drv,res=0,cd_res;
    char tmp_path[MAXPATH],parse_path[MAXPATH],*new_dir;

    tmp_drv = getdisk();
    getcurdir(0,tmp_path);
    setdisk(drive);
    chdir(ROOT);

    strcpy(parse_path,path);
    new_dir = strtok(parse_path,"\\");
    while( (new_dir != NULL) && (!res) )
    {
        if( chdir(new_dir ) < 0  )
        {
           res = mkdir(new_dir);
           cd_res = chdir(new_dir);
           if( cd_res < 0 )
              return(cd_res);
        }
        new_dir = strtok(NULL,"\\");
    }
    chdir(ROOT);
    setdisk(tmp_drv);
    chdir(ROOT);
    chdir(tmp_path);
    return(res);
}



/*-----------------------------------------------
 *   CheckSrcFiles
 *
 *   Verify CHIRON files exist in source directory
 *   return 0 for success, -1 for failure.
 *-----------------------------------------------
 */

int
CheckSrcFiles(int drive, char *path, char *files[], int file_count)
{
    int tmp_drv,i,res=0;
    char tmp_path[MAXPATH];
    struct ffblk file_info;

    tmp_drv = getdisk();
    getcurdir(0,tmp_path);
    setdisk(drive);
    chdir(path);

    for(i=0;i<file_count;i++)
    {
        if(( res = findfirst(files[i],&file_info,0)) == -1 )
           break;
    }
    setdisk(tmp_drv);
    chdir(ROOT);
    chdir(tmp_path);
    return(res);
}



/*-----------------------------------------------
 *   InstallIntroScr
 *
 *   Display the introductory screen for the
 *   install/configurationb program
 *   return 0 for "No", 1 for "YES".
 *-----------------------------------------------
 */

int
InstallIntroScr( void )
{
    static char *answer[] = { "YES","NO" };
    static char *desc[] = { " Continue with installation process. ",
                            " Do not continue with installation process. Exit now."
                          };
    static char *intro = "CHIRON (c) will now be copied to the drive and directory of your choice. Do you wish to continue with the installation process ?";

    textattr(NORMAL);
    window(WINDOW_FULL);
    clrscr();
    box_window();
    gotoxy(WIN_HOME);
    Description(2,title);
    Description(6,intro);
    return( !SelectStrV(10,answer,desc,2));
}




/*-----------------------------------------------
 *   GetSourcePath
 *
 *   Display screens and prompt user for the
 *   Drive and path to the original CHIRON
 *   files to be installed.
 *-----------------------------------------------
 */

void
GetSourcePath( void )
{
    extern struct HW hardware;
    static char *scr_name = "SOURCE DRIVE AND DIRECTORY SELECTION";
    static char *drv_desc = "Use the arrow keys to highlight the drive letter which contains the original CHIRON (c) files.";
    static char *path_desc = "Enter the directory path which contains the original CHIRON (c) files. The default is the root directory ( \\ ).";

    textattr(NORMAL);
    window(WINDOW_FULL);
    clrscr();
    box_window();
    gotoxy(WIN_HOME);
    Description(2,title);
    Description(6,scr_name);
    Description(15,drv_desc);
    src_drv = Select(10,hardware.drives);
    Description(15,path_desc);
    src_path = GetStrField(12,ROOT,52);
}



/*-----------------------------------------------
 *   GetDestPath
 *
 *   Display screens and prompt user for the
 *   Drive and path in which to install the
 *   CHIRON files.
 *-----------------------------------------------
 */

void
GetDestPath( void )
{
    extern struct HW hardware;
    static char *scr_name = "DESTINATION DRIVE AND DIRECTORY SELECTION";
    static char *drv_desc = "Use the arrow keys to highlight the drive letter on which to install the CHIRON (c) program.";
    static char *path_desc = "Enter the directory path in which to install CHIRON (c). The default is a \\CHIRON.  ";

    textattr(NORMAL);
    window(WINDOW_FULL);
    clrscr();
    box_window();
    gotoxy(WIN_HOME);
    Description(2,title);
    Description(6,scr_name);
    Description(15,drv_desc);
    dest_drv = Select(10,hardware.drives);
    Description(15,path_desc);
    dest_path = GetStrField(12,"\\CHIRON",52);
}

/*-----------------------------------------------
 *   CopyFiles
 *
 *   Copy CHIRON files from source location to
 *   Destination location
 *-----------------------------------------------
 */

void
CopyFiles( void )
{
    int i;
    char dos_command[100];
    static char *copy_str = "     ==>  COPYING FILES  <==    \n\r";
    static char *format1  = "copy %c:%s\\%s %c:%s";
    static char *format1a = "copy %c:%s %c:%s";
    static char *format2  = "ERROR COPYING %s ...  UNABLE TO CONTINUE.";

    window(WINDOW_FULL);
    textattr(NORMAL);
    clrscr();
    gotoxy(WIN_HOME);
    cputs(copy_str);
    for(i=0;i<src_file_count;i++)
    {
        if( strlen(src_path) > 1 )
           sprintf(dos_command,format1,'A'+src_drv,src_path,src_files[i],
                                       'A'+dest_drv,dest_path);
        else
           sprintf(dos_command,format1a,'A'+src_drv,src_files[i],
                                        'A'+dest_drv,dest_path);
        puts(dos_command);
        if( system(dos_command) < 0 )
        {
            cprintf(format2,src_files[i]);
            exit(0);
        }
    }
}

/*----------------------------------------------
 *   Abort
 *
 *   Abort config program.. clean up windows
 *-----------------------------------------------
 */
void
Abort( void )
{
    static char *abort = "\n\nCHIRON (c) configuration / installation program aborted at user request .....";

    textattr(NORMAL);
    window(WINDOW_FULL);
    clrscr();
    cputs(abort);
    exit(0);
}

/*-----------------------------------------------
 *   Install
 *
 *   Main routine for the installation portion of the
 *   Configure/Install program.
 *-----------------------------------------------
 */
void
Install( void )
{
    int result=0,srcok=-1,destok=-1,bpc=0;
    char *noyesabort[] = { "NO","YES","ABORT" };
    char *nya_desc[] = {  "Do not create path and let me enter a new path name",
                          "Create specified path",
                          "Do not create path and exit installation program"
                        };
    enum { NO,YES,ABORT };
    struct dfree dt;
    extern struct HW hardware;
    extern struct MC min_config;

    if( !InstallIntroScr() )
       Abort();
 
    while( srcok < 0  )
    {
        GetSourcePath();
        if((srcok = CheckPath( src_drv, src_path )) < 0  )
        {
            textattr(rv_attr);
            Description(20,"Invalid Path....  Press [ESC] to abort, any other key to re-try.");
            if( getch() == ESC )
                Abort();
            textattr(NORMAL);
        }
        else
        {
            if(( srcok = CheckSrcFiles( src_drv, src_path, src_files,src_file_count )) < 0 )
            {
                textattr(rv_attr);
                Description(21,"Unable to find CHIRON (c) files to install.  Press [ESC] to abort, any other key to re-try.");
                if( getch() == ESC )
                    Abort();
                textattr(NORMAL);
            }
        }
    }
    while( destok < 0  )
    {
        GetDestPath();
        getdfree(dest_drv+1,&dt);
        bpc = dt.df_bsec * dt.df_sclus;
        hardware.disk_free  = ( long )dt.df_avail * bpc;
        if( hardware.disk_free < min_config.diskfree )
        {
            textattr(rv_attr);
            Description(21,"Not enough diskspace on drive. Press ESC to abort, any other key to re-try ");
            if( getch() == ESC )
                Abort();
            textattr(NORMAL);
        }
        else
        {
            if( ( destok = CheckPath(dest_drv,dest_path)) < 0 )
            {
                textattr(rv_attr);
                Description(15,"Path does not exist .. Shall I create it for you?");
                window(WINDOW_FULL);
                if((result = SelectStrH(18,noyesabort,nya_desc,3)) == ABORT )
                    Abort();
                else if( result == YES )
                    destok = MakePath( dest_drv, dest_path );
            }
        }
    }
    config.default_drive = dest_drv;
    strcpy(config.bin_dir,dest_path);
    CopyFiles();
}


/*-------------------------------------------------------------------
 *        ConfigIntroScr
 *
 *    Display the introductory text for the configuration program
 *-------------------------------------------------------------------
 */
int
ConfigIntroScr( void )
{
    static char *intro = "The configuration file for CHIRON (c) will now be created.  You will need to know hardware information such as your printer type and CD-ROM drive letter to continue";
    static char *answer[] = { "YES","NO" };
    static char *desc[] = { " Continue with configuration process. ",
                            " Do not continue with configuration process. Exit now."
                          };

    textattr(NORMAL);
    window(WINDOW_FULL);
    clrscr();
    box_window();
    gotoxy(WIN_HOME);
    Description(2,title);
    Description(6,intro);
    return( SelectStrV(10,answer,desc,2));
}

 /*-----------------------------------------------
 *   ConfigBinPath
 *
 *   Display screens and prompt user for the
 *   Drive and path in which CHIRON has been installed
 *-----------------------------------------------
 */
void
ConfigBinPath( void )
{
    int bin_drv,ok=-1;
    char *bin_path;
    extern struct HW hardware;
    char *exe_files[] = {"chiron.exe"};
    static char *scr_name = "INSTALLED DRIVE AND DIRECTORY SELECTION";
    static char *drv_desc = "Use the arrow keys to highlight the drive letter on which CHIRON (c) has been installed.";
    static char *path_desc = "Enter the directory path in which CHIRON (c) is installed. The default is the root directory ( \\ ).";

    while( ok < 0 )
    {
       textattr(NORMAL);
       window(WINDOW_FULL);
       clrscr();
       box_window();
       gotoxy(WIN_HOME);
       Description(2,title);
       Description(6,scr_name);
       Description(15,drv_desc);
       bin_drv = Select(10,hardware.drives);
       Description(15,path_desc);
       bin_path = GetStrField(12,ROOT,52);
       if((ok = CheckPath(bin_drv,bin_path )) < 0 )
       {
           textattr(rv_attr);
           Description(20,"Invalid Path....  Press [ESC] to abort, any other key to re-try.");
           if( getch() == ESC )
               Abort();
           textattr(NORMAL);
        }
        else
        {
           if(( ok=CheckSrcFiles( bin_drv, bin_path, exe_files,1 )) < 0 )
           {
              textattr(rv_attr);
              Description(21,"CHIRON (c) not installed on specified drive and path.  Press [ESC] to abort, any other key to re-try.");
              if( getch() == ESC )
                  Abort();
              textattr(NORMAL);
           }
        }
    }
    config.default_drive = bin_drv;
    strncpy( config.bin_dir,bin_path,MAXPATHLEN );
    free(bin_path);
}

/*-----------------------------------------------
 *   ConfigDataPath
 *
 *   Display screens and prompt user for the
 *   Drive and path in which CHIRON should write data
 *-----------------------------------------------
 */

void
ConfigDataPath( void )
{

    int data_drv,ok=-1,result=0;
    char *noyesabort[] = { "NO","YES","ABORT" };
    enum { NO,YES,ABORT };
    char *nya_desc[] = {  "Do not create path and let me enter a new path name",
                          "Create specified path",
                          "Do not create path and exit installation program"
                        };
    char *data_path;
    extern struct HW hardware;
    static char *scr_name = "DATA DRIVE AND DIRECTORY SELECTION";
    static char *drv_desc = "Use the arrow keys to highlight the drive letter on which CHIRON (c) should write data.";
    static char *path_desc = "Enter the directory path in which CHIRON (c) should write output. The default is the root directory ( \\ ).";

    while( ok < 0 )
    {
       textattr(NORMAL);
       window(WINDOW_FULL);
       clrscr();
       box_window();
       gotoxy(WIN_HOME);
       Description(2,title);
       Description(6,scr_name);
       Description(15,drv_desc);
       data_drv = Select(10,hardware.drives);
       Description(15,path_desc);
       if(data_drv == config.default_drive )
          data_path = GetStrField(12,config.bin_dir,52);
       else
          data_path = GetStrField(12,ROOT,52);
       if((ok = CheckPath( data_drv, data_path )) < 0  )
       {
          textattr(rv_attr);
          Description(15,"Path does not exist..Shall I create it for you?");
          window(WINDOW_FULL);
          if((result = SelectStrH(18,noyesabort,nya_desc,3)) == ABORT )
             Abort();
          else if( result == YES )
             ok = MakePath( data_drv, data_path );
       }
    }
    window(WINDOW_FULL);
    config.output_drive = data_drv;
    strncpy( config.output_dir,data_path,MAXPATHLEN );
    free(data_path);
}

/*-------------------------------------------------------------------
 *        ConfigPrinter
 *
 *    Allow user to configure printer
 *-------------------------------------------------------------------
 */

int
ConfigPrinter( void )
{
    static char *scr_name = "PRINTER CONFIGURATION";
    static char *text = "Do you wish to configure CHIRON (c) to use an EPSON FX80 (c) or 100% compatable printer?";
    char *yesno[] = {"      NO","      YES"};
    char *yn_desc[] = { "Do not configure CHIRON (c) to use a printer.",
                        "Configure CHIRON (c) to use a printer."
                      };
    int selection=0;

    textattr(NORMAL);
    window(WINDOW_FULL);
    clrscr();
    box_window();
    gotoxy(WIN_HOME);
    Description(2,title);
    Description(6,scr_name);
    Description(10,text);
    window(WINDOW_FULL);
    selection = SelectStrH(15,yesno,yn_desc,2);
    return( selection );
}

/*-------------------------------------------------------------------
 *        ConfigPPort
 *
 *    Allow user to select printer port for printer
 *-------------------------------------------------------------------
 */
int
ConfigPPort( void )
{
    extern struct HW hardware;
    static char *scr_name = "PRINTER PORT SELECTION";
    static char *multi_port = "Use the arrow keys to select the parallel port to which your printer is connected.      ";
    static char *mono_port  = "Printer being installed on LPT1 ( Only one Parallel port detected ).      ";
    static char *strike = "Strike any key to continue..";
    char *lpt[5];
    int i,selection=0,lpt_cnt=0;

    lpt_cnt = ( hardware.parallel < 10 ) ? hardware.parallel : 10;
    for(i=0;i<lpt_cnt;i++)
    {
        lpt[i] = (char *) calloc(6,sizeof(char));
        sprintf(lpt[i],"LPT%i",i);
    }
    textattr(NORMAL);
    window(WINDOW_FULL);
    clrscr();
    box_window();
    gotoxy(WIN_HOME);
    Description(2,title);
    Description(6,scr_name);
    if(hardware.parallel > 1 )
    {
        Description(15,multi_port);
        window(WINDOW_FULL);
        selection = SelectStrH(10,lpt,NULL,i);
    }
    else
    {
        Description(15,mono_port);
        Description(20,strike);
        getch();
        selection = 0;
    }
    return( selection );
}

/*-------------------------------------------------------------------
 *        SelectCD
 *
 *    Allow user to select drive desiginator for CD-ROM drive
 *-------------------------------------------------------------------
 */
int
SelectCD( void )
{
    extern struct HW hardware;
    static char *scr_name = "CD-ROM DRIVE SELECTION";
    static char *cd_drv_desc = "Use the arrow keys to highlight the drive letter of the installed CD-ROM drive.";
    int select_drive;

    textattr(NORMAL);
    window(WINDOW_FULL);
    clrscr();
    box_window();
    gotoxy(WIN_HOME);
    Description(2,title);
    Description(6,scr_name);
    Description(15,cd_drv_desc);
    select_drive = Select(10,hardware.drives );
    /* Now index into hardware drives and store the actual drive
       letter, not just it's offset. i.e for hardware.drives == "abe"
       selecting 'e' returns a value of 2, the index, but what we really
       need is the drive letter,4 .
    */
    return( ( (int ) hardware.drives[select_drive] ) - 'A' );

    /* use the values of hardware.drives for now since
       select returns an Int and if it doesn't get all
       the drives it returns an incorrect value

    if( !strlen(hardware.CD_ROM_drives) )
       return(Select(10,hardware.drives) );
    else
       return(Select(10,hardware.CD_ROM_drives));
     */
}

/*-------------------------------------------------------------------
 *        WriteConfig
 *
 *    Write the config structure to a file
 *-------------------------------------------------------------------
 */
int
WriteConfig( void )
{
    FILE *config_file;
    int expected = sizeof(config),written=0;
    char config_path[MAXPATH];
    char *writing = "Writing CHIRON (c) configuration file..";

    textattr(NORMAL);
    window(WINDOW_FULL);
    clrscr();
    box_window();
    Description(2,title);
    Description(10,writing);
    sprintf(config_path,"%c:%s\\chiron.cfg",'A'+config.default_drive,
                                            config.bin_dir);
    config_file = fopen(config_path,"w");
    written = fwrite(&config,1,sizeof(config),config_file);
    if(written != expected )
        written = 0;
    else
        written = 1;
    fclose(config_file);
    return(written);
}

/*-------------------------------------------------------------------
 *        Config
 *
 *    Main routine for configuration part of program
 *-------------------------------------------------------------------
 */
void
Config( void )
{
    extern struct HW hardware;
    char *write_error = "Error encountered while writing configuration file... CHIRON (c) may be incorrectly configured!";
    char *success = "CHIRON (c) has been successfully installed and configured. !!";
    if( ConfigIntroScr() )
       Abort();;

    if( strlen( dest_path ) )
    {
       config.default_drive = dest_drv;
       strcpy(config.bin_dir,dest_path);
    }
    else
       ConfigBinPath();
    ConfigDataPath();
    config.memory_size = hardware.dosmem;
    if( ConfigPrinter() )
    {
        strcpy(config.def_printer_type,"EPSONFX80");
        config.printer_port_id = ConfigPPort();
    }
    else
    {
        strcpy(config.def_printer_type,"NONE");
        config.printer_port_id = NO_PRINTER;
    }
    config.cdrom_drive = SelectCD();
    if( !WriteConfig())
    {
        Description(10,write_error);
        Description(15,"Strike any key to exit");
        getch();
    }
    else
    {
        Description(10,success);
        Description(15,"Strike any key to continue");
        getch();
    }
}

/*-----------------------------------------------
 *   IntroScr
 *
 *   Display the introductory screen for the
 *   install/configurationb program
 *
 *-----------------------------------------------
 */

int
IntroScr( void )
{
    static char *answer[] = { "BOTH","INSTALL ONLY","CONFIGURE ONLY","QUIT NOW" };
    static char *desc[] = { "Perform both installation and configuration. Select this if this is a first time installation",
                     "Perform installation only. You must run the configuration portion before CHIRON (c) will function properly",
                     "Perform configuration only. You must have previously installed CHIRON (c).",
                     "Quit now. Do not install nor configure CHIRON (c)."
                     };

    textattr(NORMAL);
    window(WINDOW_FULL);
    clrscr();
    box_window();
    gotoxy(WIN_HOME);
    Description(2,title);
    return( SelectStrV(6,answer,desc,4) );
}


/*-------------------------------------------------------------------
 *    MAIN
 *
 *    Main routine for configuration and installation
 *-------------------------------------------------------------------
 */

main( int argc, char *argv[] )
{
    extern struct HW hardware;
    extern struct text_info vi;
    int result=0,action=0,i=0;
    enum {BOTH,INSTALL,CONFIG,QUIT};
    static char *error1 = "CHIRON (c) not installed ( insufficient hardware ) ....\n To force installation anyway type install -F";
    static char *success1 = "CHIRON (c) Installed and Configured";
    static char *force1 = "Installation forced with -F option.  CHIRON (c) may not work properly.";

    if( argc )
    {
        for(i=1;i<=argc;i++)
        {
            if( !stricmp("-F",argv[i]) )
               force = TRUE;
            if( !stricmp("-S",argv[i]) )
               show = TRUE;
        }
    }
    GetHardware();
    if( show )
    {
       Print_hardware();
       exit(1);
    }

    result = CheckHardware();

    if( !result && !force )
    {
       puts(error1);
       exit(0);
    }
    VideoInit();

    action = IntroScr();
    if( action == QUIT || action < 0 )
       Abort();
    else if( action == BOTH )
    {
        Install();
        Config();
    }
    else if( action == INSTALL )
    {
        Install();
    }
    else if ( action == CONFIG )
    {
        Config();
    }
    window(WINDOW_FULL);
    clrscr();
    puts(success1);
    if( force )
       puts(force1);
    exit(0);
}
