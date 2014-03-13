/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\init.c 2.0 1993/09/18 16:28:11 mwm Exp mwm $
 *
 *   Initialization routine(s).
 *
 *   void Initialize()
 *   FILE *InitOpenTLG( char *file_name, char *notice )
 *   void SwapCINX( void )
 *
 *   $Log: init.c $
 * Revision 2.0  1993/09/18  16:28:11  mwm
 * Start of new version.
 *
 * Revision 1.4  92/04/09  18:42:03  ROOT_DOS
 * Various clean-up things
 * 
 * Revision 1.3  92/03/29  16:13:27  ROOT_DOS
 * Added configuration code
 * 
 * Revision 1.2  91/12/01  18:59:32  ROOT_DOS
 * Put in PingPrinter check
 * 
 * Revision 1.1  91/08/19  19:48:12  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include "tlg.h"
#include "init.h"
#include "win.h"
#include "notice.h"
#include "atab.h"
#include "tlgmem.h"
#include "print.h"
#include "file.h"
#include "intr.h"
#include "config.h"

/* GLOBALS */

struct CFG *chiron_cfg;         /* Global configuration info */

FILE *TLGwlist,*TLGwcnts,*TLGcantxt;

BYTE *LINXbuf;              	/* Buffer for WLINX.INX */
UL *CINXbuf;                    /* Buffer for WCINX.INX */
BYTE *AWLSTbuf;                 /* Buffer for AWLST.INX */
BYTE *CANIDTbuf;                /* BUffer for DOCCAN1.IDT */

/*-------------------------------------------------------------------
 *   Initialize              9.24.89 MM
 *
 *   Open files that are needed throughout the program since they
 *   take a long time.
 *   Malloc buffers and read data that is used all during the program.
 *   Swap bytes in count index buffer, set up various index arrays
 *   and init printer.
 *
 *   09.19.90 MM
 *-------------------------------------------------------------------
 */
void
Initialize()
{
    FILE *TLGwlinx,*TLGwcinx,*TLGawlst,*TLGcanidt;
    int i;

    /*
    ** Do configuration stuff first
    */
    chiron_cfg = (struct CFG *) TLGmalloc( sizeof(struct CFG) );
    if( !ReadConfig( chiron_cfg ) )
    {
       makeNotice("Can not find CHIRON configuration file",3,NOWAIT);
       addToNotice("Press any key to exit CHIRON");
       WaitForKey();
       noticeDown();
       closegraph();
       ClockShutdown();
       ExitMessage();
       exit(1);
    }

    SetCDROMLetter( chiron_cfg->cdrom_drive );

    LINXbuf = (BYTE *) TLGmalloc( LINXBUFSIZE*sizeof(BYTE) );
    CINXbuf = (UL *) TLGmalloc(CINXBUFSIZE*sizeof(UL));
    AWLSTbuf = (BYTE *) TLGmalloc( MAXSERIAL*3*sizeof(BYTE) );
    CANIDTbuf = (BYTE *) TLGmalloc(DOCCAN1SIZE*sizeof(BYTE) );


    makeNotice("      Initializing...Please Wait      ",9,NOWAIT);

    TLGwlist = InitOpenTLG("TLGWLIST.INX", "Word List File");
    TLGwcnts = InitOpenTLG("TLGWCNTS.INX", "Word Counts File");
    TLGwlinx = InitOpenTLG("TLGWLINX.INX", "Word List Index File");
    TLGwcinx = InitOpenTLG("TLGWCINX.INX", "Word Counts Index File");
    TLGawlst = InitOpenTLG("TLGAWLST.INX", "Author/Work Index File");
    TLGcanidt = InitOpenTLG("DOCCAN1.IDT", "Canon ID Table File");
    TLGcantxt = InitOpenTLG("DOCCAN1.TXT", "Canon Text File");

    CHECKREAD(LINXbuf, sizeof(BYTE), LINXBUFSIZE, TLGwlinx);
    CHECKREAD(CINXbuf, sizeof(UL), CINXBUFSIZE, TLGwcinx);
    CHECKREAD(AWLSTbuf, sizeof(BYTE), MAXSERIAL*3, TLGawlst);
    CHECKREAD(CANIDTbuf, sizeof(BYTE), DOCCAN1SIZE, TLGcanidt);

    fclose(TLGwlinx);
    fclose(TLGwcinx);
	fclose(TLGawlst);
    fclose(TLGcanidt);

	SwapCINX();
    MakeAtabIndex();
    SelectPrinter(epson_fx);

    if( PingPrinter(chiron_cfg->printer_port_id) )
       InitializePrinter(greek);

    noticeDown();
}/*
END of Initialize
*/

/*-------------------------------------------------------------------
 * InitOpenTLG
 *
 *  Handy function for opening the files on CD-ROM
 *-------------------------------------------------------------------
 */
FILE *
InitOpenTLG( char *file_name, char *notice )
{
	char TLGfile[40],TLGnotice[100];
	FILE *rfp;

	sprintf( TLGnotice, "Opening %s", notice);
	addToNotice( TLGnotice );
        if(!(rfp = OpenTLG( file_name )))
                SYSTEMCRASH(EXIT_BADOPEN);

	return(rfp);
}/*
END of OpenTLG
*/

/*-------------------------------------------------------------------
 *   SwapCINX                9.25.89 MM
 *
 *   Cheesey function to reverse bytes in CINXbuf longs.
 *
 *-------------------------------------------------------------------
 */
void
SwapCINX( void )
{
        BYTE t[4];
        BYTE *pt;
        unsigned long *pl;
        int i;

        pl = (unsigned long *) t;

        for(i=0;i<CINXBUFSIZE;i++)
        {
                pt = (BYTE *) &CINXbuf[i];
                t[3] = pt[0];
                t[2] = pt[1];
                t[1] = pt[2];
                t[0] = pt[3];
                CINXbuf[i] = *pl;
        }
}/*
END of SwapCINX
*/


