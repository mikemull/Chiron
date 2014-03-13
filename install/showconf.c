#include <stdio.h>
#include <stdlib.h>
#include "config.h"

main()
{
    static char drives[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static char *graph_drivers[] = { "DETECT","CGA","MCGA","EGA",
                                    "EGA64","EGAMONO","IBM8514",
                                    "HERCMONO","ATT400","VGA","PC3270"
                                  };
    struct CFG config;
    int result;


    result = ReadConfig( &config );
    printf("Configuration File Read : [ %i ]\n",result );
    printf("--------------------------------\n");
    printf("Default Drive      : %c:\n",drives[config.default_drive]);
    printf("  drive num        : %i\n",config.default_drive);
    printf("CDROM Drive        : %c:\n",drives[config.cdrom_drive] );
    printf("  drive num        : %i\n",config.cdrom_drive);
    printf("Core Memory        : %i\n",config.memory_size );
    printf("Printer Port ID    : %i\n",config.printer_port_id );
    printf("Default Printer    : %s\n",config.def_printer_type );
    printf("Executable Path    : %s\n",config.bin_dir );
    printf("Document Path      : %s\n",config.doc_dir );
    printf("Font Path          : %s\n",config.font_dir );
    printf("Graphic Path       : %s\n",config.graph_dir );
    printf("Output Drive       : %c:\n",drives[config.output_drive]);
    printf("  drive num        : %i\n",config.output_drive);
    printf("Output Path        : %s\n",config.output_dir );
    printf("Max Graphic Mode   : %i\n",config.max_graph_mode );
    printf("Max Graphic Driver : %s\n",graph_drivers[config.max_graph_driver] );
    printf("--------------------------------\n");
}
