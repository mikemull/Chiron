/*===================================================================
 *   $Header: C:\MIKE\TLG\CHIRON\RCS\file.c 2.0 1993/09/18 16:17:20 mwm Exp mwm $
 *
 *   General routines for TLG file manipulation
 *
 *   $Log: file.c $
 * Revision 2.0  1993/09/18  16:17:20  mwm
 * Start of new version.
 *
 *
 *-------------------------------------------------------------------
 */

#include "tlg.h"
#include "file.h"
#include "dir.h"

static char CD_ROM_drive_letter;

/*-------------------------------------------------------------------
 *   SetCDROMLetter          04.09.92 MM
 *
 *   Set the letter of the CD-ROM drive.  This is called only by
 *   init, which gets the drive letter from the config structure.
 *-------------------------------------------------------------------
 */
void
SetCDROMLetter( int drive )
{
        CD_ROM_drive_letter = (char) (drive + 'A');
}/*
END of SetCDROMLetter */


/*-------------------------------------------------------------------
 *  OpenTLG                  04.09.92 MM
 *
 *  Open a file on the CD ROM.
 *-------------------------------------------------------------------
 */
 FILE *
 OpenTLG( char *file_name )
 {
        char path[MAXPATH];
        FILE *rfp;

        if(strlen(file_name) > (MAXPATH - MAXDRIVE))
                rfp = (FILE *) NULL;
        else
        {
                sprintf(path,"%c:%s",CD_ROM_drive_letter,file_name);
                rfp = fopen(path,"rb");
        }
        return rfp;
}/*
END of OpenTLG */



/*-------------------------------------------------------------------
 *   MoveToBlock             9.24.89 MM
 *
 *   Moves the file pointer to the beginning of the specified
 *   8K block.
 *
 *-------------------------------------------------------------------
 */

void
MoveToBlock( FILE *fp, unsigned long Block )
{
        fseek( fp, Block*8192, SEEK_SET);
}/*
END of MoveToBlock
*/


