# Prometheus package management tool set
# Copyright (C) 2003-2004 Oliver Brakmann <obrakmann@gmx.net> &
# Gareth Jones <moderob@lineone.net>
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

include Makefile.defs

.PHONY: all clean install update

all:
	$(MAKE) -C etc
	$(MAKE) -C man
	$(MAKE) -C profiles
	$(MAKE) -C src
#	$(MAKE) -C x86stepping

clean:
	$(MAKE) -C etc clean
	$(MAKE) -C man clean
	$(MAKE) -C profiles clean
	$(MAKE) -C src clean
#	$(MAKE) -C x86stepping clean

install: all update
	$(MAKE) -C etc install
	$(MAKE) -C profiles install

update: all
	$(MAKE) -C man install
	$(MAKE) -C src install
#	$(MAKE) -C x86stepping install
