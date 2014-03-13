#
#
#  Makefile for utility program TXTCIT
#
#
TARGET = txtcit
SRC = txtcit.c citation.c citlow.c bit.c tlgmem.c tclckall.c intr.c
OBJS= txtcit.obj citation.obj citlow.obj bit.obj tlgmem.obj tclckall.obj intr.obj
LIBS= graphics.lib
CC=bcc
MODEL=s
FLAGS= -v -f -m$(MODEL) -Ic:\dev\chiron\stable.112\include
ARC=$(TARGET).arc

$(TARGET): $(OBJS)
        $(CC) $(FLAGS) $(LIBS) $(OBJS)

txtcit.obj: txtcit.c
        $(CC) -c $(FLAGS) txtcit.c

citation.obj: c:\dev\chiron\stable.112\tlg\citation.c
        $(CC) -c $(FLAGS) c:\dev\chiron\stable.112\tlg\citation.c

citlow.obj: c:\dev\chiron\stable.112\tlg\citlow.c
        $(CC) -c $(FLAGS) c:\dev\chiron\stable.112\tlg\citlow.c

bit.obj: c:\dev\chiron\stable.112\tlg\bit.c
        $(CC) -c $(FLAGS) c:\dev\chiron\stable.112\tlg\bit.c

tlgmem.obj: c:\dev\chiron\stable.112\tlg\tlgmem.c
        $(CC) -c $(FLAGS) c:\dev\chiron\stable.112\tlg\tlgmem.c

tclckall.obj: c:\dev\chiron\stable.112\tlg\tclckall.c
        $(CC) -c $(FLAGS) c:\dev\chiron\stable.112\tlg\tclckall.c

intr.obj: c:\dev\chiron\stable.112\tlg\intr.c
        $(CC) -c $(FLAGS) c:\dev\chiron\stable.112\tlg\intr.c

.c.obj:
        $(CC) -c $(FLAGS) $<

clean:
        del *.obj
        del *.exe
        del *.map

archive:
        pkarc a $(ARC) *.c *.h makefile
        make clean
