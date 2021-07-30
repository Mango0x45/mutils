.POSIX:

target	= center

CFLAGS	= -O3 -pedantic -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wno-unused-result
PREFIX	= /usr/local

all: ${target}
${target}: ${target}.c

.PHONY: clean install uninstall
clean:
	rm -f ${target}

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin ${DESTDIR}${PREFIX}/share/man/man1
	cp ${target} ${DESTDIR}${PREFIX}/bin
	cp ${target}.1 ${DESTDIR}${PREFIX}/share/man/man1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${target} ${DESTDIR}${PREFIX}/share/man/man1/${target}.1
