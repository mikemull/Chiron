#
#
#  Makefile for utility program GETAUTHS
#
#
#  tcc -v -ml -I..\work getauths.c ..\work\
#
TARGET =  gotocit
SRC = gotocit.c
OBJS= gotocit.obj
CC=bcc
LIBS=
MODEL=l
FLAGS= -v -f -m$(MODEL) -I..\work
ARC=$(TARGET).arc

$(TARGET): $(OBJS)
        tcc $(FLAGS) $(LIBS) $(OBJS)

.c.obj:
        tcc -c $(FLAGS) $<

clean:
        del *.obj
        del *.exe
        del *.map

archive:
        pkarc a $(ARC) *.c *.h makefile
        make clean
