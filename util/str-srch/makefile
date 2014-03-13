#
#  Makefile for building the string search routines and
#  the strtest test progrem.
#
#
.AUTODEPEND
#
CC=bcc
LIBSRC=bmsrch.c bygsrch.c regex.c re_fail.c bitprint.c
LIBOBJS=bmsrch.obj bygsrch.obj regex.obj re_fail.obj bitprint.obj
MODEL=l
#FLAGS= -Ox -v -f -m$(MODEL) -I. -I..\..\work\include           # Optimized
FLAGS= -v -f -m$(MODEL) -I. -I..\..\work\include               # standard
#FLAGS= -3 -d -f287 -G -O2 -m$(MODEL) -I. -I..\..\work\include   # wizbang
ARC=$(TARGET).arc

all : strsrch.lib strtest

strsrch.lib : $(LIBOBJS)
        &tlib strsrch -+$(?:.c=.obj)

strtest : strtest.obj strsrch.lib
        bcc $(FLAGS) strtest.obj strsrch.lib

.c.obj:
        bcc -c $(FLAGS) $<

clean:
        del *.obj
        del *.exe
        del *.map

archive:
        pkarc a $(ARC) *.c *.h makefile
        make clean

#
# Dependencies
#
bmsrch.c        : bmsrch.h
bygsrch.c       : bygsrch.h
regex.c         : regex.h
re_fail.c       : regex.h
