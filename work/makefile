#
#  Make file for CHIRON
#  09.23.90 MM
#
.AUTODEPEND

SRCS = chiron.c

OBJS = chiron.obj
LIBS = mwin\mwin.lib print\cprint.lib tlg\tlg.lib
MODEL = l

.c.obj:
        bcc -2 -c -v -N -m$(MODEL) -I.\include $<

chiron.exe: $(OBJS) $(LIBS)
        tlink @chiron.rsp

