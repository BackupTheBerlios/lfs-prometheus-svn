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

TESTS = finish/success.bash \
	finish/failure.bash \
	terminated/terminated.bash \
	pipestat/success.bash \
	pipestat/failure1.bash \
	pipestat/failure2.bash \
	pipestat/failure3.bash \
	warning/status.bash \
	failure/status.bash \
	error/status.bash \
	usage_error/status.bash \
	usage_error/stderr.bash \
	usage_error/stdout.bash \
	script_usage_err/status.bash \
	script_usage_err/stderr.bash \
	script_usage_err/stdout.bash \
	setup_logging/tylf.bash \
	setup_logging/tyln.bash \
	setup_logging/tylu.bash \
	setup_logging/tnlf.bash \
	setup_logging/tnln.bash \
	setup_logging/tnlu.bash \
	setup_logging/tulf.bash \
	setup_logging/tuln.bash \
	setup_logging/tulu.bash

PASS = ../runtest
FAIL = ../runtest !

.PHONY: all check clean

all: $(TESTS)

%: %.in
	sed -e 's|%LIBPROMETHEUS%|$(LIBDIR)/$(LIB)|' $< > $@
	chmod 0755 $@

check: all
	$(PASS) finish/success.bash "libprometheus/finish: successful exit"
	$(FAIL) finish/failure.bash "libprometheus/finish: unsuccessful exit"
	$(FAIL) terminated/terminated.bash \
		"libprometheus/terminated: forced exit"
	$(PASS) pipestat/success.bash \
		"libprometheus/pipestat: successful pipeline"
	$(FAIL) pipestat/failure1.bash \
		"libprometheus/pipestat: pipeline, failure at start"
	$(FAIL) pipestat/failure2.bash \
		"libprometheus/pipestat: pipeline, failure in middle"
	$(FAIL) pipestat/failure3.bash \
		"libprometheus/pipestat: pipeline, failure at end"
	$(PASS) warning/status.bash \
		"libprometheus/warning: exit status after warning"
	$(FAIL) failure/status.bash \
		"libprometheus/failure: exit status after failure"
	$(FAIL) error/status.bash "libprometheus/error: exit status after error"
	$(FAIL) usage_error/status.bash "libprometheus/usage_error: exit status"
	$(PASS) usage_error/stderr.bash \
		"libprometheus/usage_error: function usage error reporting"
	$(PASS) usage_error/stdout.bash \
		"libprometheus/usage_error: output only to stderr"
	$(FAIL) script_usage_err/status.bash \
		"libprometheus/script_usage_err: exit status"
	$(PASS) script_usage_err/stderr.bash \
		"libprometheus/script_usage_err: script usage error reporting"
	$(PASS) script_usage_err/stdout.bash \
		"libprometheus/script_usage_err: output only to stderr"
	$(PASS) setup_logging/tylf.bash \
		"libprometheus/setup_logging: terminal + file"
	$(PASS) setup_logging/tyln.bash \
		"libprometheus/setup_logging: terminal + no file"
	$(PASS) setup_logging/tylu.bash "libprometheus/setup_logging: terminal"
	$(PASS) setup_logging/tnlf.bash \
		"libprometheus/setup_logging: no terminal + file"
	$(PASS) setup_logging/tnln.bash \
		"libprometheus/setup_logging: no terminal + no file"
	$(PASS) setup_logging/tnlu.bash \
		"libprometheus/setup_logging: no terminal"
	$(PASS) setup_logging/tulf.bash "libprometheus/setup_logging: file"
	$(PASS) setup_logging/tuln.bash "libprometheus/setup_logging: no file"
	$(PASS) setup_logging/tulu.bash \
		"libprometheus/setup_logging: default settings"

clean:
	-rm $(TESTS) */output.txt */*.log
