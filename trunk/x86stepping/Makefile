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

# FIXME: For 2.6 kernels, there is a proper way of building modules.  Also we
# cannot rely on being able to unload the module after use.  Perhaps there is a
# better way.

# Build flags.
CC = gcc
KERNEL = $(shell uname -r)
CFLAGS = -O2
CPPFLAGS = -I/lib/modules/$(KERNEL)/build/include
MODDIR = /lib/modules/$(KERNEL)/kernel/arch/i386/kernel

include ../Makefile.defs

# What to build/install.
OBJECT = x86stepping.o
MODULE = $(OBJECT).gz

.PHONY: all clean install

all: $(MODULE)

clean:
	-rm $(MODULE) $(OBJECT)

install: all
	$(INSTALL_DATA) $(MODULE) $(MODDIR)

$(MODULE): $(OBJECT)
	gzip -9 $<
