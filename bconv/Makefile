.POSIX:

target	= bconv

LEX	= flex

CFLAGS	= -O3
LDFLAGS	= -lgmp -lfl
LFLAGS	= --warn

PREFIX	= /usr/local

all: ${target}
${target}: ${target}.c
${target}.c: ${target}.l

.PHONY: clean install uninstall
clean:
	rm -f ${target} ${target}.c

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin ${DESTDIR}${PREFIX}/man/man1
	cp ${target} ${DESTDIR}${PREFIX}/bin
	cp ${target}.1 ${DESTDIR}${PREFIX}/share/man/man1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${target} ${DESTDIR}${PREFIX}/share/man/man1/${target}.1*
