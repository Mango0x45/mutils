.POSIX:

targets	= ap pp

PREFIX	= /usr/local

all: ${targets}
${targets}: pp.c
	${CC} ${CFLAGS} -o pp $<
	${CC} ${CFLAGS} -DAPPEND -o ap $<

.PHONY: clean install uninstall
clean:
	rm -f ${targets}

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin ${DESTDIR}${PREFIX}/share/man/man1
	cp ${targets} ${DESTDIR}${PREFIX}/bin
	cp ap.1 pp.1 ${DESTDIR}${PREFIX}/share/man/man1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/ap ${DESTDIR}${PREFIX}/bin/pp \
		${DESTDIR}${PREFIX}/share/man/man1/ap.1 ${DESTDIR}${PREFIX}/share/man/man1/pp.1
