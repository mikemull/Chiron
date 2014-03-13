/*
** $Header: C:\MIKE\TLG\CHIRON\RCS\chiron.h 2.0 1993/09/18 16:43:29 mwm Exp mwm $
*/

#ifndef chiron_h
#define chiron_h

/* int main(int argc, char **argv); */
int ExitProc(void);
int CatchSignal( void );
void Debug(char *format, ... );
void ExitMessage( void );

extern Handle help_button,search_button,text_button,info_button,quit_button,
print_button;
extern Handle author_entry,work_entry,word_entry,cit_entry,context_entry,
sfile_entry;
extern Handle txtwin;
extern FILE *TLGtxt,*TLGidt;

#endif

