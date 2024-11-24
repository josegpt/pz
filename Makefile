PREFIX?=	${DESTDIR}/var
HTDOCSDIR=	${PREFIX}/www/htdocs
DESIGNDIR=	${HTDOCSDIR}/design

install:
	install -d ${HTDOCSDIR}
	install -m 0644 *.html ${HTDOCSDIR}
	cp -Rp static ${HTDOCSDIR}
	cp -Rp design ${DESIGNDIR}

uninstall:
	rm -f ${HTDOCSDIR}/*.html
	rm -rf ${HTDOCSDIR}/static ${DESIGNDIR}

.PHONY: install uninstall