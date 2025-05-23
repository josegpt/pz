PREFIX?=	${DESTDIR}/var
CGIDIR=		${PREFIX}/www/cgi-bin
HTDOCSDIR=	${PREFIX}/www/htdocs

CFLAGS=		-O3 -W -Wall -Wextra -pedantic -ansi
LDFLAGS=	-static -pie

all: addr

.SUFFIXES: .c .o
.c.o:
	${CC} -c $< ${CFLAGS}

addr: addr.o cgi.o html.o
	${CC} -o $@ addr.o cgi.o html.o ${LDFLAGS}

clean:
	rm -f *.{o,core,out} addr

install:
	install -d ${HTDOCSDIR}
	install -m 0644 *.html ${HTDOCSDIR}
	cp -Rp static ${HTDOCSDIR}
	install -d ${CGIDIR}
	install -o www -g www -m 0500 addr ${CGIDIR}

uninstall:
	rm -f ${HTDOCSDIR}/*.html
	rm -f ${CGIDIR}/addr
	rm -rf ${HTDOCSDIR}/static

.PHONY: all clean install uninstall
