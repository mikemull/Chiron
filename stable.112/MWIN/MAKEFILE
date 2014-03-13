#
#  Make file for MWIN library
#  08.28.93 MM
#
.AUTODEPEND

SRCS = win.c button.c list.c entry.c txtwin.c notice.c greek.c

OBJS = win.obj button.obj list.obj entry.obj txtwin.obj notice.obj greek.obj \
cga.obj

MODEL = l

.c.obj:
        bcc -2 -c -v -N -m$(MODEL) -I../include $<

mwin.lib: $(OBJS)
        &tlib mwin -+$(?:.c=.obj)


