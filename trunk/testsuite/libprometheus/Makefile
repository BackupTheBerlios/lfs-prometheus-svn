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

TESTS = finish/success.bash finish/failure.bash terminated/terminated.bash \
	pipestat/success.bash pipestat/failure1.bash pipestat/failure2.bash \
	pipestat/failure3.bash warning/status.bash failure/status.bash \
	error/status.bash usage_error/usage_error.bash \
	script_usage_err/script_usage_err.bash

PASS = ../runtest
FAIL = ../runtest !

.PHONY: all check clean

all: $(TESTS)

%: %.in
	sed -e 's|%LIBPROMETHEUS%|$(LIBDIR)/$(LIB)|' $< > $@
	chmod 0755 $@

check: all
	$(PASS) finish/success.bash "successful exit"
	$(FAIL) finish/failure.bash "unsuccessful exit"
	$(FAIL) terminated/terminated.bash "forced exit"
	$(PASS) pipestat/success.bash "successful pipeline"
	$(FAIL) pipestat/failure1.bash "pipeline, failure at start"
	$(FAIL) pipestat/failure2.bash "pipeline, failure in middle"
	$(FAIL) pipestat/failure3.bash "pipeline, failure at end"
	$(PASS) warning/status.bash "exit status after warning"
	$(FAIL) failure/status.bash "exit status after failure"
	$(FAIL) error/status.bash "exit status after error"
	$(PASS) usage_error/usage_error.bash "function usage error reporting"
	$(PASS) script_usage_err/script_usage_err.bash \
		"script usage error reporting"

clean:
	rm $(TESTS)
