#
#
#  Makefile for Chiron configuration/installation program
#
#
INSTALLSRC = install.c hardware.c video.c
INSTALLOBJS= install.obj hardware.obj video.obj
SHOWCONFOBJS = showconf.obj config.obj
LIBS= graphics.lib
MODEL=s
FLAGS= -v -f -m$(MODEL)
ARC=install.arc

install: $(INSTALLOBJS)
        bcc $(FLAGS) $(LIBS) $(INSTALLOBJS)

showconf: $(SHOWCONFOBJS)
        bcc $(FLAGS) $(LIBS) $(SHOWCONFOBJS)
#############################################################
.c.obj:
        bcc -c $(FLAGS) $<

clean:
        del *.obj
        del *.exe
        del *.map

archive:
        pkarc a $(ARC) *.c *.h makefile
        make clean
