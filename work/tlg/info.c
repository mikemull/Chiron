/*===================================================================
 * $Header: C:\MIKE\TLG\CHIRON\RCS\info.c 2.0 1993/09/15 20:40:11 mwm Exp mwm $
 *
 * Information routines
 *
 * $Log: info.c $
 * Revision 2.0  1993/09/15  20:40:11  mwm
 * Start of new version
 * l
 *
 * Revision 1.2  92/03/07  14:36:35  ROOT_DOS
 * Added Canon info stuff.
 * 
 * Revision 1.1  91/12/01  21:53:32  ROOT_DOS
 * Initial revision
 * 
 *-------------------------------------------------------------------
 */
#include "tlg.h"
#include "win.h"
#include "chiron.h"
#include "notice.h"
#include "entry.h"
#include "info.h"
#include "citlow.h"
#include "citation.h"
#include "work.h"
#include "canon.h"
#include "init.h"
#include "tlgmem.h"

enum {CANONINFO,CITINFO};

/*-------------------------------------------------------------------
 *  Information              12.01.91 MM
 *
 *  Main information routine.  Uses the info_type argument to call
 *  appropriate routine.
 *-------------------------------------------------------------------
 */
int
Information( int info_type, char *info_string )
{
        switch (info_type)
        {
                case CANONINFO:
                        CanonInfo();
                        break;
                case CITINFO:
                        CitationInfo();
                        break;
        }
        return 1;
}/*
END of Information */


/*-------------------------------------------------------------------
 *  CanonInfo               12.01.91 MM
 *-------------------------------------------------------------------
 */
void
CanonInfo()
{
        int i;
        char **canon_biblio;
        char message[200];

        CanBibInit();

        if( WorkIsOpen() )
        {
           makeNotice("              TLG Canon Bibliography for Current Work                    ",
                      10,NOWAIT);
           canon_biblio = CanBibGetBiblio( Work.author_num, Work.work_num );
           i=0;
           while( ( i < 10 ) && ( canon_biblio[i] != NULL ) )
           {
             sprintf(message,"%s",canon_biblio[i] );
             addToNotice(message);
             i++;
           }
           WaitForKey();
           noticeDown();
           CanBibFree();
        }
        else
        {
           makeNotice("         No work opened yet        ",2,WAIT);
           noticeDown();
        }
        return;
}/*
END of CanonInfo */                             
/*-------------------------------------------------------------------
 *  CitationInfo            12.01.91 MM
 *-------------------------------------------------------------------
 */
void
CitationInfo()
{
        char message[70];
        citation_t *citation;

        if(WorkIsOpen())
        {
          citation = WorkCitStruct();

          sprintf(message,"Level a:  %-20s %-20s", CIT_DESC( CIT_AUTHOR ),
                                                   CIT_VALUE( CIT_AUTHOR ) );
          makeNotice( message, 10, NOWAIT );

          sprintf(message,"Level b:  %-20s %-20s", CIT_DESC( CIT_WORK ),
                                                   CIT_VALUE( CIT_WORK ) );
          addToNotice(message);

          sprintf(message,"Level c:  %-20s %-20s", CIT_DESC( CIT_ABBREV ),
                                                   CIT_VALUE( CIT_ABBREV ) );
          addToNotice(message);

          sprintf(message,"Level v:  %-20s %-20s", CIT_DESC( CIT_V ),
                                                   CIT_VALUE( CIT_V ) );
          addToNotice(message);

          sprintf(message,"Level w:  %-20s %-20s", CIT_DESC( CIT_W ),
                                                   CIT_VALUE( CIT_W ) );
          addToNotice(message);

          sprintf(message,"Level x:  %-20s %-20s", CIT_DESC( CIT_X ),
                                                   CIT_VALUE( CIT_X ) );
          addToNotice(message);

          sprintf(message,"Level y:  %-20s %-20s", CIT_DESC( CIT_Y ),
                                                   CIT_VALUE( CIT_Y ) );
          addToNotice(message);

          sprintf(message,"Level z:  %-20s %-20s", CIT_DESC( CIT_Z ),
                                                   CIT_VALUE( CIT_Z ) );
          addToNotice(message);

          WaitForKey();
          noticeDown();
        }
        else
        {
                makeNotice("         No work opened yet        ",2,WAIT);
                noticeDown();
        }
}/*
END of CitationInfo */

