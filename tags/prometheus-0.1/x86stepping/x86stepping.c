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
 * $ insmod /path/to/x86stepping.o CPU_stepping=$n
 * for an ${n}86 (3 <= $n <= 6).  CPU_stepping defaults to 3.
 *
 * This code is GPLed and based on `uname_ix86.c' by whygee@f-cpu.org and
 * JuBColding@yorkref.com.  This version allows the CPU stepping to be specified
 * at load-time rather than compile-time.
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
