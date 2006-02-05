# Prometheus package management tool set
# Copyright (C) 2003-2006 Oliver Brakmann <oliverbrakmann@users.berlios.de> &
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

all: $(CONFIG)

$(CONFIG): $(CONFIG).in
	sed -e 's|%PROFDIR%|$(PROFDIR)|' \
	    -e 's|%INSTALL_LOG_RC%|$(INSTALL_LOG_RC)|' \
	    $< > $@

clean:
	-rm $(CONFIG)

install: all
	$(INSTALL_DIR) $(DESTDIR)$(CONFDIR) $(DESTDIR)$(PROFDIR)
	$(INSTALL_DATA) $(CONFIG) $(DESTDIR)$(CONFDIR)
	$(INSTALL_DATA) $(REFERENCE_PROFILE) $(DESTDIR)$(PROFDIR)
