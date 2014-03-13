#
#
#  Makefile for utility program IDT2TXT
#
#
#
CC = bcc
MODEL=l
FLAGS= -v -f -m$(MODEL) -I..\..\work\include
TARGET = idt2txt
SRC = idt2txt.c
OBJS= idt2txt.obj
LIBS= graphics.lib ..\..\work\tlg\tlg.lib
ARC=$(TARGET).arc

$(TARGET): $(OBJS)
        $(CC) $(FLAGS) $(LIBS) $(OBJS)

.c.obj:
        $(CC) -c $(FLAGS) $<

clean:
        del *.obj
        del *.exe
        del *.map

archive:
        pkarc a $(ARC) *.c *.h makefile
        make clean
