include config.mk

# Macros for source files and object files
SRCS = bujoshell.c draw.c init.c input.c update.c util.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: bujoshell

bujoshell: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

desktop_file:
	mkdir -p ${DESTDIR}${APPPREFIX}
	mkdir -p ${DESTDIR}${PREFIX}/share/bujoshell/icons
	sed -i "s|Icon=.*|Icon=${DESTDIR}${PREFIX}/share/bujoshell/icons/bujoshell.svg|" bujoshell.desktop
	sudo cp -f icons/bujoshell.svg ${DESTDIR}${PREFIX}/share/bujoshell/icons
	cp -f bujoshell.desktop ${DESTDIR}${APPPREFIX}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	mkdir -p ${DESTDIR}${PREFIX}/share/bujoshell
	cp -f bujoshell ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/bujoshell

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/bujoshell
	rm -rf ${DESTDIR}${PREFIX}/share/bujoshell
	rm -f ${DESTDIR}${APPPREFIX}/bujoshell.desktop

clean:
	rm -rf bujoshell *.o *.d
