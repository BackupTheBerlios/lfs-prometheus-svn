# Prometheus package management tool set
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

include ../../Makefile.defs

TESTS = finish/success \
	finish/failure \
	terminated/terminated \
	pipestat/success \
	pipestat/failure1 \
	pipestat/failure2 \
	pipestat/failure3 \
	warning/status \
	warning/stdout \
	warning/stderr \
	failure/status \
	failure/stdout \
	failure/stderr \
	error/status \
	error/stdout \
	error/stderr \
	usage_error/status \
	usage_error/stdout \
	usage_error/stderr \
	script_usage_err/status \
	script_usage_err/stdout \
	script_usage_err/stderr \
	setup_logging/tylf \
	setup_logging/tyln \
	setup_logging/tylu \
	setup_logging/tnlf \
	setup_logging/tnln \
	setup_logging/tnlu \
	setup_logging/tulf \
	setup_logging/tuln \
	setup_logging/tulu

LOGS = $(addsuffix .log, $(TESTS))

.PHONY: all check clean

all:

%.log: %
	../runtest $<

check: $(LOGS)

clean:
	-rm $(LOGS) */output.txt
