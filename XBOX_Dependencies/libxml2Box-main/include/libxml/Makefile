# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4-p5 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ../..
prefix = c:/progra~1/libxml2
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/libxml2
pkglibdir = $(libdir)/libxml2
pkgincludedir = $(includedir)/libxml2

top_builddir = ../..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /bin/install.exe -cpD
INSTALL_PROGRAM = /bin/install.exe -cpsD $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = /bin/install.exe -cp
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = i386-pc-mingw32
host_triplet = i386-pc-mingw32
AS = @AS@
CATALOG_OBJ = catalog.o
CC = gcc
CFLAGS = -O2 -fnative-struct -fno-rtti -fno-exceptions -mcpu=pentium -march=i386 -Wall
CPP = gcc -E
DEBUG_OBJ = debugXML.o
DLLTOOL = @DLLTOOL@
DOCB_OBJ = DOCBparser.o
ECHO = echo
EXEEXT = .exe
FTP_OBJ = nanoftp.o
HAVE_ISINF = 
HAVE_ISNAN = 
HTML_DIR = $(prefix)/doc
HTML_OBJ = HTMLparser.o HTMLtree.o
HTTP_OBJ = nanohttp.o
ICONV_LIBS =  -liconv
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LIBXML_MAJOR_VERSION = 2
LIBXML_MICRO_VERSION = 12
LIBXML_MINOR_VERSION = 4
LIBXML_VERSION = 2.4.12
LIBXML_VERSION_INFO = 6:12:4
LIBXML_VERSION_NUMBER = 20412
LN_S = cp -fp
MAINT = #
MAKEINFO = makeinfo
MV = /usr/bin/mv
M_LIBS = 
OBJDUMP = @OBJDUMP@
OBJEXT = o
PACKAGE = libxml2
RANLIB = ranlib
RDL_LIBS = 
RM = /usr/bin/rm
STRIP = strip
TAR = /usr/bin/tar
TEST_THREADS = 
THREAD_CFLAGS = 
THREAD_LIBS = 
U = 
VERSION = 2.4.12
WITH_CATALOG = 1
WITH_DEBUG = 1
WITH_DOCB = 1
WITH_FTP = 1
WITH_HTML = 1
WITH_HTTP = 1
WITH_ICONV = 1
WITH_MEM_DEBUG = 0
WITH_THREADS = 0
WITH_TRIO = 0
WITH_XINCLUDE = 1
WITH_XPATH = 1
WITH_XPTR = 1
XINCLUDE_OBJ = xinclude.o
XML_CFLAGS = 
XML_INCLUDEDIR = -I${includedir}/libxml2/libxml -I${includedir}/libxml2
XML_LIBDIR = -L${libdir}
XML_LIBS = -lxml2 -lz  -liconv -lm -Wl,-s -lwsock32 -liberty -lMsup -lMstubs
XPATH_OBJ = xpath.o
XPTR_OBJ = xpointer.o
Z_CFLAGS = 
Z_LIBS = -lz

xmlincdir = $(includedir)/libxml2/libxml

xmlinc_HEADERS = \
		SAX.h \
		entities.h \
		encoding.h \
		parser.h \
		parserInternals.h \
		xmlerror.h \
		HTMLparser.h \
		HTMLtree.h \
		debugXML.h \
		tree.h \
		list.h \
		hash.h \
		xpath.h \
		xpathInternals.h \
		xpointer.h \
		xinclude.h \
		xmlIO.h \
		xmlmemory.h \
		nanohttp.h \
		nanoftp.h \
		uri.h \
		valid.h \
		xlink.h \
		xmlversion.h \
		xmlwin32version.h \
		DOCBparser.h \
		catalog.h \
		threads.h \
		globals.h


EXTRA_DIST = xmlversion.h.in xmlwin32version.h.in
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../config.h
CONFIG_CLEAN_FILES =  xmlversion.h xmlwin32version.h
HEADERS =  $(xmlinc_HEADERS)

DIST_COMMON =  Makefile.am Makefile.in xmlversion.h.in \
xmlwin32version.h.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

GZIP_ENV = --best
all: all-redirect
.SUFFIXES:
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps include/libxml/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status

xmlversion.h: $(top_builddir)/config.status xmlversion.h.in
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status
xmlwin32version.h: $(top_builddir)/config.status xmlwin32version.h.in
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status

install-xmlincHEADERS: $(xmlinc_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(xmlincdir)
	@list='$(xmlinc_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(xmlincdir)/$$p"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(xmlincdir)/$$p; \
	done

uninstall-xmlincHEADERS:
	@$(NORMAL_UNINSTALL)
	list='$(xmlinc_HEADERS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(xmlincdir)/$$p; \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = include/libxml

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am:
	@$(NORMAL_INSTALL)
	$(MAKE) $(AM_MAKEFLAGS) install-exec-hook
install-exec: install-exec-am

install-data-am: install-xmlincHEADERS
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-xmlincHEADERS
uninstall: uninstall-am
all-am: Makefile $(HEADERS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(xmlincdir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-tags mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-tags clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-tags distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-tags maintainer-clean-generic \
		distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: uninstall-xmlincHEADERS install-xmlincHEADERS tags \
mostlyclean-tags distclean-tags clean-tags maintainer-clean-tags \
distdir info-am info dvi-am dvi check check-am installcheck-am \
installcheck install-exec-am install-exec install-data-am install-data \
install-am install uninstall-am uninstall all-redirect all-am all \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


install-exec-hook:
	$(mkinstalldirs) $(DESTDIR)$(xmlincdir)

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
