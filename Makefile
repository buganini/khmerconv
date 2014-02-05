DESTDIR?=
PREFIX?=/usr/local
LOCALBASE?=${PREFIX}

CFLAGS=-g -Wall -I${LOCALBASE}/include -O2
LIBS=-L${LOCALBASE}/lib -lbsdconv

TODO_CODECS=
TODO_CODECS+=inter/KHMER-REORDER
TODO_CODECS+=inter/KHMER-ABC
TODO_CODECS+=inter/KHMER-ABC2
TODO_CODECS+=inter/KHMER-ABC3
TODO_CODECS+=inter/KHMER-ABC-ZWSP
TODO_CODECS+=inter/KHMER-BAIDOK
TODO_CODECS+=inter/KHMER-FK
TODO_CODECS+=inter/KHMER-KAOHKONG
TODO_CODECS+=inter/KHMER-KHEK
TODO_CODECS+=inter/KHMER-LIMON
TODO_CODECS+=inter/KHMER-RASMEI
TODO_CODECS+=inter/KHMER-TRUTH


all: khmerconv codecs

builddir:
	mkdir -p build/bin
	mkdir -p build/share/bsdconv/inter

khmerconv: builddir khmerconv.c
	$(CC) ${CFLAGS} -o khmerconv khmerconv.c ${LIBS}

codecs: builddir
	for item in ${TODO_CODECS} ; do \
		bsdconv-mktable modules/$${item}.txt ./build/share/bsdconv/$${item} ; \
		if [ -e modules/$${item}.man ]; then cp modules/$${item}.man ./build/share/bsdconv/$${item}.man ; fi ; \
		if [ -e modules/$${item}.c ]; then echo Build $${item}.so; $(CC) ${CFLAGS} -D_BSDCONV_INTERNAL modules/$${item}.c -L./build/lib/ -fPIC -shared -o ./build/share/bsdconv/$${item}.so -lbsdconv ${LIBS} ; fi ; \
	done

install: install_main install_codecs

install_main:
	install -m 555 khmerconv ${DESTDIR}${PREFIX}/bin

install_codecs:
	for item in ${TODO_CODECS} ; do \
		install -m 444 build/share/bsdconv/$${item} ${DESTDIR}${PREFIX}/share/bsdconv/$${item} ; \
		if [ -e build/share/bsdconv/$${item}.man ]; then install -m 444 build/share/bsdconv/$${item}.man ${DESTDIR}${PREFIX}/share/bsdconv/$${item}.man ; fi ; \
		if [ -e build/share/bsdconv/$${item}.so ]; then install -m 444 build/share/bsdconv/$${item}.so ${DESTDIR}${PREFIX}/share/bsdconv/$${item}.so ; fi ; \
	done

fetch:
	@mkdir -p tmp
	@if [ ! -e tmp/fontdata.xml ]; then \
		wget -O tmp/KhmerConverter.tgz "http://downloads.sourceforge.net/sourceforge/khmer/KhmerConverter_1.5.1.tar.gz";	\
		cd tmp/; \
		tar zxf KhmerConverter.tgz; \
		cp KhmerConverter-1.5.1/modules/fontdata.xml ./; \
		rm -rf KhmerConverter*; \
	fi ;

gen: fetch
	python tools/gen.py tmp/fontdata.xml

clean:
	rm -f khmerconv
