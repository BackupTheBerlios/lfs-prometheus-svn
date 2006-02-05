# Makefile for pkgtools

prefix	= /usr

sbindir	= $(prefix)/sbin
bindir	= $(prefix)/bin
libdir	= $(prefix)/lib
mandir	= $(prefix)/share/man
confdir = /etc/pkg
profdir	= $(confdir)/profiles

BINPRGS	= pkginfo pkgget	# pkgcompress compact
SBINPRGS= pkgadd pkgrm
PROGS   = $(BINPRGS) $(SBINPRGS)
LIB	= libprometheus.sh
CONFIG	= prometheus.conf
PROFILES= $(shell ls profiles/*.pkg)

INSTALL	= install

INSTALL_DATA	= $(INSTALL) -m644
INSTALL_PROGRAM	= $(INSTALL) -m755

.SILENT:

.PHONY: clean
.PHONY: all
.PHONY: install
.PHONY: update

all: $(CONFIG) $(PROGS) $(LIB)

clean:
	-rm -f $(CONFIG) $(PROGS) $(LIB)
	-find . -type f -name '*.orig' -exec rm -f {} \;

prometheus.conf:
	sed -e 's#%PROFDIR%#$(profdir)#' template.conf > $(CONFIG)

pkginfo: 
	sed -e 's#%LIBPKG%#$(libdir)/$(LIB)#' pkginfo.sh > pkginfo

pkgget: 
	sed -e 's#%LIBPKG%#$(libdir)/$(LIB)#' pkgget.sh > pkgget

#pkgcompress: 
#	sed -e 's#%LIBPKG%#$(libdir)/$(LIB)#' pkgcompress.sh > pkgcompress

#compact: 
#	sed -e 's#%LIBPKG%#$(libdir)/$(LIB)#' compact.sh > compact

pkgadd: 
	sed -e 's#%LIBPKG%#$(libdir)/$(LIB)#' pkgadd.sh > pkgadd

pkgrm: 
	sed -e 's#%LIBPKG%#$(libdir)/$(LIB)#' pkgrm.sh > pkgrm

libprometheus.sh:
	sed -e 's#%CONFFILE%#$(confdir)/$(CONFIG)#' libprometheus > $(LIB)

update: $(PROGS) $(LIB)
	# Man pages
	-mkdir -p $(DESTDIR)$(mandir)/man1 $(DESTDIR)$(mandir)/man8
	$(INSTALL_DATA) $(addprefix doc/,$(addsuffix .1,$(BINPRGS))) \
		$(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) $(addprefix doc/,$(addsuffix .8,$(SBINPRGS))) \
		$(DESTDIR)$(mandir)/man8
	$(INSTALL_DATA) doc/pkginstall.8 $(DESTDIR)$(mandir)/man8

	# Support functions library
	-mkdir -p $(DESTDIR)$(libdir)
	$(INSTALL_DATA) $(LIB) $(DESTDIR)$(libdir)

	# Core programs
	-mkdir -p $(DESTDIR)$(bindir) $(DESTDIR)$(sbindir)
	$(INSTALL_PROGRAM) $(BINPRGS) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) $(SBINPRGS) $(DESTDIR)$(sbindir)
	ln -sf pkgadd $(DESTDIR)$(sbindir)/pkginstall

	# x86 cross compilation kernel module
	# not yet done

install: $(CONFIG) update
	# Configuration and profiles

	# Adjusting hard-coded paths to pkgadd in LFS build profiles.
	for p in $(shell grep -l '%PKGADD%' $(PROFILES)); do \
		cp "$$p"{,.orig}; \
		sed -e "s@%PKGADD%@$(sbindir)/pkgadd@" "$$p.orig" > "$$p"; \
	done

	-mkdir -p $(DESTDIR)$(confdir) $(DESTDIR)$(profdir)
	$(INSTALL_DATA) $(CONFIG) $(DESTDIR)$(confdir)
	$(INSTALL_DATA) $(PROFILES) $(DESTDIR)$(profdir)

