DESTDIR?=
PREFIX?=/usr/local
LOCALBASE?=${PREFIX}

CFLAGS=-g -Wall -I${LOCALBASE}/include
LIBS=-L${LOCALBASE}/lib -lbsdconv

all: khmerconv

khmerconv: khmerconv.c
	$(CC) ${CFLAGS} -o khmerconv khmerconv.c ${LIBS}

install:
	install -m 555 khmerconv ${DESTDIR}${PREFIX}/bin

fetch:
	@mkdir -p tmp
	@if [ ! -e tmp/fontdata.xml ]; then \
		wget -O tmp/KhmerConverter.tgz "http://downloads.sourceforge.net/sourceforge/khmer/KhmerConverter_1.5.1.tar.gz";	\
		cd tmp/ && tar zxvf KhmerConverter.tgz; \
		cp KhmerConverter-1.5.1/modules/fontdata.xml ./; \
		rm -rf KhmerConverter*; \
	fi ;

gen: fetch
	python tools/gen.py tmp/fontdata.xml

clean:
	rm -f khmerconv
