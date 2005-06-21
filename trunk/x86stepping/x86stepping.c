/*
 * x86stepping control kernel module
 * Copyright (C) 2003-2005 Oliver Brakmann <oliverbrakmann@users.berlios.de> &
 * Gareth Jones <gareth_jones@users.berlios.de>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * This kernel module allows you to change the reported CPU stepping of an x86
 * CPU.  It is useful for forcing a package to compile for an older CPU.  You
 * can also use it to compile for a newer CPU, if you really want to, though you
 * won't be able to run the resulting code on the compile host.
 *
 * Compile:
 * $ $CC -I/lib/modules/$(uname -r)/build/include -O2 -c x86stepping.c
 * where $CC is the compiler you used to compile your kernel.
 *
 * Usage:
 * # insmod /path/to/x86stepping.o CPU_stepping=$n
 * for an ${n}86 (3 <= $n <= 6).  CPU_stepping defaults to 3.
 *
 * This code is based on `uname_ix86.c' by <whygee@f-cpu.org> and
 * <JuBColding@yorkref.com>.  This version allows the CPU stepping to be
 * specified at load-time rather than compile-time.
 */

#define LICENCE	"GPL"
#define AUTHOR	"Gareth Jones <gareth_jones@users.berlios.de>"
#define DESCR	"x86 CPU stepping controller"

#define __KERNEL__
#define MODULE

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/utsname.h>

static char real_CPU_stepping,
	   *CPU_stepping = "3";

/*
 * CPU_stepping is a load-time parameter.
 */
MODULE_PARM (CPU_stepping, "s");
MODULE_PARM_DESC (CPU_stepping, "Desired CPU stepping");

/*
 * Change the CPU stepping reported by uname.  Save the real stepping.
 */
static int __init set_stepping (void) {
	char stepping = *CPU_stepping;

	/*
	 * First ensure stepping is valid.
	 */
	if (stepping < '3' || stepping > '6') {
		printk (KERN_ERR "x86stepping: bad stepping value: i%c86\n",
			stepping);
		return 1;
	}

	real_CPU_stepping = system_utsname.machine [1];
	system_utsname.machine [1] = stepping;

	return 0;
}

module_init (set_stepping);

/*
 * Restore the real CPU stepping.
 */
static void __exit restore_stepping (void) {
	system_utsname.machine [1] = real_CPU_stepping;
}

module_exit (restore_stepping);

/*
 * About this module...
 */
MODULE_LICENSE (LICENCE);
MODULE_AUTHOR (AUTHOR);
MODULE_DESCRIPTION (DESCR);
