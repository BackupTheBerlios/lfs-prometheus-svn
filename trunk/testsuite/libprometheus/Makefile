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
	setup_logging/tulu \
	log_echo/tyly_stdout \
	log_echo/tyly_stderr \
	log_echo/tyly_log \
	log_echo/tyln_stdout \
	log_echo/tyln_stderr \
	log_echo/tnly_stdout \
	log_echo/tnly_stderr \
	log_echo/tnly_log \
	log_echo/tnln_stdout \
	log_echo/tnln_stderr \
	log_warning/tyly_stdout \
	log_warning/tyly_stderr \
	log_warning/tyly_log \
	log_warning/tyln_stdout \
	log_warning/tyln_stderr \
	log_warning/tnly_stdout \
	log_warning/tnly_stderr \
	log_warning/tnly_log \
	log_warning/tnln_stdout \
	log_warning/tnln_stderr \
	log/tylycp_status \
	log/tylycp_stdout \
	log/tylycp_stderr \
	log/tylycp_log \
	log/tylycf_status \
	log/tylycf_stdout \
	log/tylycf_stderr \
	log/tylycf_log \
	log/tylncp_status \
	log/tylncp_stdout \
	log/tylncp_stderr \
	log/tylncf_status \
	log/tylncf_stdout \
	log/tylncf_stderr \
	log/tnlycp_status \
	log/tnlycp_stdout \
	log/tnlycp_stderr \
	log/tnlycp_log \
	log/tnlycf_status \
	log/tnlycf_stdout \
	log/tnlycf_stderr \
	log/tnlycf_log \
	log/tnlncp_status \
	log/tnlncp_stdout \
	log/tnlncp_stderr \
	log/tnlncf_status \
	log/tnlncf_stdout \
	log/tnlncf_stderr \
	choice/yes \
	choice/no \
	echo_array/echo_array \
	source_file/valid_status \
	source_file/valid_sourced \
	source_file/invalid_status \
	source_file/invalid_error \
	source_file/none_status \
	source_file/none_error

LOGS = $(addsuffix .log, $(TESTS))

.PHONY: all check clean

all:

%.log: %
	../runtest $<

check: $(LOGS)

clean:
	-rm $(LOGS) */output.txt */output.log
