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

# Default build flags.
CPPFLAGS := -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE_SOURCE \
            -I../libreroot
CXXFLAGS := -g -pipe -Wall -W -Werror -O2
LDFLAGS := -pipe -L../libreroot -lreroot

# What to build.
program := rerootd
objects := arguments.o \
           main.o \
           messageloop.o

.PHONY: all clean dep

all: $(program)

clean:
	rm -f $(program) $(objects) $(objects:.o=.d)

dep:

# Generate & include dependencies.
%.d: %.cc
	$(CXX) -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

# Don't generate & include dependencies just to delete them again.
ifneq ($(MAKECMDGOALS),clean)
 include $(objects:.o=.d)
endif

$(program): $(objects)
	$(CXX) $(LDFLAGS) -o $@ $^
