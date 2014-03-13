/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\config.h 2.0 1993/09/18 16:43:48 mwm Exp mwm $
*/

#ifndef config_h
#define config_h

/*  Configuration structure */

#define MAXPATHLEN  80

struct CFG
{
    int default_drive;
    int cdrom_drive;
    int memory_size;
    int printer_port_id;
    char def_printer_type[20];
    char bin_dir[MAXPATHLEN];
    char doc_dir[MAXPATHLEN];
    char font_dir[MAXPATHLEN];
    char graph_dir[MAXPATHLEN];
    int output_drive;
    char output_dir[MAXPATHLEN];
    int max_graph_mode;
    int max_graph_driver;
};

/*  Prototypes */

int ReadConfig( struct CFG * );

#endif
