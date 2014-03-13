#
#  Make file for CHIRON
#  09.23.90 MM
#
.AUTODEPEND

SRCS = callback.c help.c makelist.c inxfile.c wlist.c wcnts.c file.c \
init.c bit.c awlst.c citation.c citlow.c text.c canon.c atab.c strsrch.c   \
snlist.c info.c tlgmem.c regex.c re_fail.c ptree.c work.c \
author.c config.c savetext.c intr.c

OBJS = callback.obj help.obj makelist.obj inxfile.obj wlist.obj \
wcnts.obj file.obj init.obj bit.obj awlst.obj citation.obj citlow.obj \
text.obj canon.obj atab.obj strsrch.obj snlist.obj info.obj tlgmem.obj \
regex.obj re_fail.obj ptree.obj work.obj author.obj \
config.obj savetext.obj intr.obj tclckall.obj

MODEL = l

.c.obj:
        bcc -c -v -m$(MODEL) -I..\include $<

tlg.lib: $(OBJS)
        &tlib /c tlg -+$(?:.c=.obj)

