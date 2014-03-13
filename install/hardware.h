#ifndef hardware_h
#define hardware_h

/*===============================================
 *  hardware.h
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

/* Structures */

struct HW
{
                    int     equip;
                    long    dosmem;
                    int     currdrv;
                    int     osmajor;
                    int     osminor;
                    long    disk_total;
                    long    disk_free;
                    int     lastdrv;
                    int     drivecount;
                    char    drives[30];
                    int     diskette;
                    int     parallel;
                    int     p_status[10];
                    int     serial;
                    int     MSext;
                    int     CD_ROM_count;
                    char    CD_ROM_drives[30];
                    int     graphdriver;
                    char    cwd[MAXPATH];
};

struct MC
{
    long    dosmem;
    int     osmajor;
    int     osminor;
    int     drivecount;
    int     diskette;
    int     graphdriver;
    int     graphmode;
    long    diskfree;
};

/*  Prototypes */

void DosVer();
void BasicInfo();
void Lastdrv();
void LogDrives();
void GetDrives();
void Diskette();
void PrinterInfo();
void SerialInfo();
void Detcdrom();
void DetMSext();
void GetVideo();
void Print_hardware();
void GetHardware();
int CheckHardware();
void HardwareFail( int * );

#endif
