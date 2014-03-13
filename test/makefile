#
#
#  Makefile for utility program GETAUTHS
#
#
#  tcc -v -ml -I..\work getauths.c ..\work\
#
TARGET =  gotocit.exe
SRC = gotocit.c
OBJS= gotocit.obj ..\work\tlg\citlow.obj ..\work\tlg\bit.obj ..\work\tlg\tlgmem.obj ..\work\tlg\citation.obj
CC=bcc
LIBS=graphics.lib
MODEL=l
FLAGS= -v -f -m$(MODEL) -I..\work\include
ARC=$(TARGET).arc

$(TARGET): $(OBJS)
        tlink @gotocit.rsp
.c.obj:
        $(CC) -c $(FLAGS) $<

clean:
        del *.obj
        del *.exe
        del *.map

archive:
        pkarc a $(ARC) *.c *.h makefile
        make clean
