#
#
#  Makefile for utility program TXTCIT
#
#
TARGET = caninfo
SRC = caninfo.c canon.c citation.c citlow.c bit.c tlgmem.c Tclckall.c
OBJS= caninfo.obj canon.obj citation.obj citlow.obj bit.obj tlgmem.obj tclckall.obj
LIBS= graphics.lib
MODEL=s
FLAGS= -v -f -m$(MODEL) -Ic:\dev\chiron\work
ARC=$(TARGET).arc

$(TARGET): $(OBJS)
        tcc $(FLAGS) $(LIBS) $(OBJS)
caninfo.obj: caninfo.c
        tcc -c $(FLAGS) caninfo.c

canon.obj: c:\dev\chiron\work\canon.c
        tcc -c $(FLAGS) c:\dev\chiron\work\canon.c

citation.obj: c:\dev\chiron\work\citation.c
        tcc -c $(FLAGS) c:\dev\chiron\work\citation.c

citlow.obj: c:\dev\chiron\work\citlow.c
        tcc -c $(FLAGS) c:\dev\chiron\work\citlow.c

bit.obj: c:\dev\chiron\work\bit.c
        tcc -c $(FLAGS) c:\dev\chiron\work\bit.c

tlgmem.obj: c:\dev\chiron\work\tlgmem.c
        tcc -c $(FLAGS) c:\dev\chiron\work\tlgmem.c

tclckall.obj: c:\dev\chiron\work\tclckall.c
        tcc -c $(FLAGS) c:\dev\chiron\work\tclckall.c
.c.obj:
        tcc -c $(FLAGS) $<

clean:
        del *.obj
        del *.exe
        del *.map

archive:
        pkarc a $(ARC) *.c *.h makefile
        make clean
