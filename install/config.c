/*====
 *  READ CONFIGURATION FILE
 */
#include <stdio.h>
#include "config.h"

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
