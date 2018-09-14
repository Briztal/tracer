//
// Created by root on 9/11/18.
//



#include <stdbool.h>
#include <kernel/log.h>
#include <kernel/fs/dfs.h>
#include <kernel/mod/auto_mod.h>
#include <kernel/interface/timer.h>
#include <kernel/panic.h>
#include <kernel/interface/gpio.h>
#include <kernel/debug/debug.h>


static void suus() {

	kernel_log_("GROS CHIBRE EN PLATRE");

};


static void check_init() {

	kernel_log_("\n\n\n");
	kernel_log_("Querying led file : ");

	struct dfs_file *f = dfs_open("led");

	if (!f) {

		kernel_log_("File doesn't exist. Aborting.");

		return;

	}

	kernel_log("File : %h", f);

	kernel_log_("Interfacing with gpio : ");

	struct gpio_interface giface;

	bool s = file_op_interface(f, &giface, sizeof(giface));

	if (!s) {

		kernel_log_("Operation failed. Aborting");

		return;

	}

	kernel_log("success, %h", giface.pin_mask);


	while(1) {
		gpio_set(&giface, giface.pin_mask);
		debug_delay_ms(500);
		gpio_clear(&giface, giface.pin_mask);
		debug_delay_ms(500);
	}

	return;

	kernel_log_("\n\n\n");
	kernel_log_("Querying timer file : ");

	struct dfs_file *file = dfs_open("pit_0");

	if (!file) {

		kernel_log_("File doesn't exist. Aborting.");

		return;

	}

	kernel_log("File : %h", file);

	kernel_log_("Interfacing with timer : ");

	struct timer_interface iface;

	bool success = file_op_interface(file, &iface, sizeof(iface));

	if (!success) {

		kernel_log_("Operation failed. Aborting");

		return;

	}

	dfs_close(file);

	kernel_log_("Initialising timer");

	timer_init(&iface, 1000, 2000, &suus);

	kernel_log_("Success");




}

static bool check_exit() {
	return true;

}

KERNEL_EMBED_USER_MODULE(timer_check, &check_init, &check_exit);
