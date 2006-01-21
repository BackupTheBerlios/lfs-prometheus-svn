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

# Version.
base := libreroot
major := 0
minor := 0
patchlevel := 0

# Filenames.
name := $(base).so
soname := $(name).$(major)
library := $(soname).$(minor).$(patchlevel)

# Default build flags.
CPPFLAGS := -DLIBREROOT -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
CXXFLAGS := -g -pipe -Wall -W -Werror -O2 -fpic -fvisibility=hidden \
            -fvisibility-inlines-hidden
LDFLAGS := -pipe -shared -Wl,-soname,$(soname) -lrt

# What to build.
objects := inbox.o \
           ipcerror.o \
           message.o \
           messagequeue.o \
           multiinbox.o \
           outbox.o \
           packet.o

.PHONY: all clean dep

all: $(library) $(soname) $(name)

clean:
	rm -f $(name) $(soname) $(library) $(objects) $(objects:.o=.d)

dep:

# Generate & include dependencies.
%.d: %.cc
	$(CXX) -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

# Don't generate & include dependencies just to delete them again.
ifneq ($(MAKECMDGOALS),clean)
 include $(objects:.o=.d)
endif

$(library): $(objects)
	$(CXX) $(LDFLAGS) -o $@ $^

$(soname): $(library)
	ln -s $< $@

$(name): $(soname)
	ln -s $< $@
