PREFIX?=	${DESTDIR}/var
HTDOCS=		${PREFIX}/www/htdocs
CGIDIR=		${PREFIX}/www/cgi-bin

INCS=		-I/usr/local/include
LIBS=		-L/usr/local/lib -lsqlite3 -lm -lpthread

CFLAGS=		-g -W -Wall -Wextra -pedantic -ansi ${INCS}
LDFLAGS=	-static -pie ${LIBS}

all: main

.SUFFIXES: .c .o
.c.o:
	${CC} -c $< ${CFLAGS}

main: main.o pz.o
	${CC} -o $@ main.o pz.o ${LDFLAGS}

clean:
	rm -f *.{o,core,out} main

.PHONY: all clean
