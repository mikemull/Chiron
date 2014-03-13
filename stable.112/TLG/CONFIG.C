/*===================================================================
 *  $Header: C:\MIKE\TLG\CHIRON\RCS\config.c 2.0 1993/09/15 20:41:27 mwm Exp mwm $
 *
 *  READ CONFIGURATION FILE
 *
 *  $Log: config.c $
 * Revision 2.0  1993/09/15  20:41:27  mwm
 * Start of new version.
 *
 * Revision 1.1  92/03/28  17:05:56  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */

#include <stdio.h>
#include "config.h"

/*-------------------------------------------------------------------
 *  ReadConfig               03.28.92 GH
 *
 *  Read the Chiron configuration file
 *-------------------------------------------------------------------
 */
int
ReadConfig( struct CFG *config )
{
    FILE *cfg_file;
    int expected = sizeof(*config),read=0;

    cfg_file = fopen("chiron.cfg","r");
    if( !cfg_file )
        return( 0 );
    read = fread(config,1,sizeof(struct CFG ),cfg_file);
    fclose(cfg_file);
    if( read != expected )
        return( 0 );
    else
        return( 1 );
}
