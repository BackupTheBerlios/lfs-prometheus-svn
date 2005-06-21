# Prometheus package management tool set - libreroot
# Copyright (C) 2003-2004 Oliver Brakmann <oliverbrakmann@users.berlios.de> &
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

# Default build flags.
CFLAGS := -g -pipe -Wall -W -Werror --std=gnu99 -fpic
CPPFLAGS := -I.. -D_GNU_SOURCE
# Need -ldl & -lgcc.  -nostartfiles vs -nodefaultlibs vs -nostdlib?
LDFLAGS := -ldl -shared

# What to build.
library := libreroot.so
objects := absolute.o ipc.o libreroot.o memory.o reroot.o

.PHONY: all clean dep

all: $(library)

clean:
	rm -f $(library) $(objects) $(objects:.o=.d)

dep:

# Generate & include dependencies.
%.d: %.c
	$(CC) -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

# Don't generate & include dependencies just to delete them again.
ifneq ($(MAKECMDGOALS),clean)
 include $(objects:.o=.d)
endif

$(library): $(objects)
	$(CC) $(LDFLAGS) -o $@ $^
