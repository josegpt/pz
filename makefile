PREFIX?=	${DESTDIR}/var
HTDOCS=		${PREFIX}/www/htdocs
CGIDIR=		${PREFIX}/www/cgi-bin

INCS=		-I/usr/local/include
LIBS=		-L/usr/local/lib -lsqlite3 -lm -lpthread

CFLAGS=		-g -W -Wall -Wextra -pedantic -ansi ${INCS}
LDFLAGS=	-static -pie ${LIBS}

all: index addr

.SUFFIXES: .c .o
.c.o:
	${CC} -c $< ${CFLAGS}

index: index.o pz.o html.o
	${CC} -o $@ index.o pz.o html.o ${LDFLAGS}

addr: addr.o pz.o
	${CC} -o $@ addr.o pz.o ${LDFLAGS}

clean:
	rm -f *.{o,core,out} index addr

.PHONY: all clean
