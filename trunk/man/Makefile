# Prometheus package management tool set
# Copyright (C) 2003-2005 Oliver Brakmann <oliverbrakmann@users.berlios.de> &
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

MAN1 = $(addsuffix .1, $(BINPROGS))
MAN5 = $(CONFIG).5
MAN8 = $(addsuffix .8, $(SBINPROGS))
MANPAGES = $(MAN1) $(MAN5) $(MAN8)

.PHONY: all clean install

all: $(MANPAGES)

%: %.in
	sed -e 's|%VERSION%|$(VERSION)|' \
	    -e 's|%RELEASE%|$(RELEASE)|' \
	    -e 's|%CONFDIR%|$(CONFDIR)|' \
	    -e 's|%CONFIG%|$(CONFIG)|' \
	    $< > $@

clean:
	-rm $(MANPAGES)

install: all
	$(INSTALL_DIR) $(DESTDIR)$(MANDIR)/man1 $(DESTDIR)$(MANDIR)/man5 \
		       $(DESTDIR)$(MANDIR)/man8
	$(INSTALL_DATA) $(MAN1) $(DESTDIR)$(MANDIR)/man1
	$(INSTALL_DATA) $(MAN5) $(DESTDIR)$(MANDIR)/man5
	$(INSTALL_DATA) $(MAN8) $(DESTDIR)$(MANDIR)/man8
