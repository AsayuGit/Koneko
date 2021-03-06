## Process this file with automake to produce Makefile.in

# The name of the module.
DOC_MODULE=libxml2-$(VERSION)

# The top-level SGML file.
DOC_MAIN_SGML_FILE=gnome-xml.sgml

# The directory containing the source code (if it contains documentation).
DOC_SOURCE_DIR=..

HTML_DIR=@HTML_DIR@

TARGET_DIR=$(HTML_DIR)/$(DOC_MODULE)/html
PAGES= architecture.html bugs.html contribs.html docs.html DOM.html \
    downloads.html entities.html example.html help.html index.html \
    interface.html intro.html library.html namespaces.html news.html \
    tree.html xmldtd.html XML.html XSLT.html

man_MANS = xmllint.1 xmlcatalog.1

all: $(PAGES)

$(PAGES): xml.html site.xsl
	@(if [ -x /usr/bin/xsltproc ] ; then \
	  /usr/bin/xsltproc --html site.xsl xml.html > index.html ; fi );

scan:
	gtkdoc-scan --module=libxml --source-dir=$(DOC_SOURCE_DIR) --ignore-headers="acconfig.h config.h xmlwin32version.h win32config.h trio.h strio.h triop.h"

templates: scan
	gtkdoc-mktmpl --module=libxml

sgml:
	gtkdoc-mkdb --module=libxml --source-dir=$(DOC_SOURCE_DIR)

html:
	if ! test -d html ; then mkdir html ; fi
	-cd html && gtkdoc-mkhtml libxml ../$(DOC_MAIN_SGML_FILE)

clean-local:
	rm -f *~ *.bak *.hierarchy *.signals *-unused.txt

maintainer-clean-local: clean
	rm -rf sgml html libxml-decl-list.txt libxml-decl.txt 

libxml-decl-list.txt : templates

libxml-sections.txt : scan
	cp libxml-decl-list.txt libxml-sections.txt

rebuild: libxml-sections.txt templates sgml html

install-data-local: 
	$(mkinstalldirs) $(DESTDIR)$(TARGET_DIR)
	-@INSTALL@ -m 0644 $(srcdir)/xml.html $(srcdir)/encoding.html $(srcdir)/FAQ.html $(srcdir)/structure.gif $(srcdir)/DOM.gif $(DESTDIR)$(TARGET_DIR)
	-@INSTALL@ -m 0644 $(srcdir)/html/*.html $(DESTDIR)$(TARGET_DIR)
	-@INSTALL@ -m 0644 $(srcdir)/html/index.sgml $(DESTDIR)$(TARGET_DIR)
	-(cd $(DESTDIR); gtkdoc-fixxref --module=libxml --html-dir=$(HTML_DIR))

dist-hook:
	(cd $(srcdir) ; tar cvf - *.1 site.xsl *.html *.gif html/*.html html/*.sgml) | (cd $(distdir); tar xf -)

.PHONY : html sgml templates scan
