# Prometheus package management tool set
# Copyright (C) 2003-2004 Oliver Brakmann <obrakmann@gmx.net> &
# Gareth Jones <gareth_jones@users.berlios.de>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA

include ../Makefile.defs

.PHONY: all clean install

all: $(PROGS) $(LIB)

%: %.in
	sed -e 's|%LIBPROMETHEUS%|$(LIBDIR)/$(LIB)|' $< > $@

$(LIB): $(LIB).in
	sed -e 's|%CONFFILE%|$(CONFDIR)/$(CONFIG)|' \
	    -e 's|%VERSION%|$(VERSION)|' \
	    -e 's|%RELEASE%|$(RELEASE)|' \
	    $< > $@

clean:
	-rm $(PROGS) $(LIB)

install: all
	$(INSTALL_DIR) $(DESTDIR)$(BINDIR) $(DESTDIR)$(SBINDIR) \
		       $(DESTDIR)$(LIBDIR)
	$(INSTALL_DATA) $(LIB) $(DESTDIR)$(LIBDIR)
	$(INSTALL_PROG) $(BINPROGS) $(DESTDIR)$(BINDIR)
	$(INSTALL_PROG) $(SBINPROGS) $(DESTDIR)$(SBINDIR)
