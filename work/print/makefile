#
#  Make file for MWIN library
#  08.28.93 MM
#
.AUTODEPEND

SRCS = print.c epson_fx.c

OBJS = print.obj epson_fx.obj

MODEL = l

.c.obj:
        bcc -2 -c -v -N -m$(MODEL) -I../include $<

cprint.lib: $(OBJS)
        &tlib cprint -+$(?:.c=.obj)


