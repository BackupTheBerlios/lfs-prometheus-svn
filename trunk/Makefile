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

include Makefile.defs

.PHONY: all check clean install update

all:
	$(MAKE) -C etc
	$(MAKE) -C man
	$(MAKE) -C profiles
	$(MAKE) -C scripts
	$(MAKE) -C testsuite
#	$(MAKE) -C x86stepping

check:
	$(MAKE) -C testsuite check
#	$(MAKE) -C x86stepping check

clean:
	$(MAKE) -C etc clean
	$(MAKE) -C man clean
	$(MAKE) -C profiles clean
	$(MAKE) -C scripts clean
	$(MAKE) -C testsuite clean
#	$(MAKE) -C x86stepping clean

user:
	groupadd -f prometheus
	if ! getent passwd prometheus; then \
		useradd -d $(HOMEDIR) -g prometheus -s /bin/false prometheus \
	fi

install: all update
	if ! (getent group prometheus && getent passwd prometheus); then \
		echo The prometheus user and/or group is/are missing! \
		false \
	fi
	$(INSTALL_DIR_HOME) $(HOMEDIR)/backup
	$(INSTALL_DIR_HOME) $(HOMEDIR)/build
	$(INSTALL_DIR_HOME) $(HOMEDIR)/installed/indices
	$(INSTALL_DIR_HOME) $(HOMEDIR)/installed/profiles
	$(INSTALL_DIR_HOME) $(HOMEDIR)/profiles
	$(INSTALL_DIR_HOME) $(HOMEDIR)/sources
	$(MAKE) -C etc install
	$(MAKE) -C profiles install

update: all
	$(MAKE) -C man install
	$(MAKE) -C scripts install
#	$(MAKE) -C x86stepping install
