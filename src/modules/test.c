//
// Created by root on 9/11/18.
//



#include <stdbool.h>
#include <kernel/log.h>
#include <kernel/fs/inode.h>
#include <kernel/mod/auto_mod.h>
#include <kernel/interface/timer.h>
#include <kernel/panic.h>
#include <kernel/interface/gpio.h>
#include <kernel/debug/debug.h>
#include <kernel/interface/command.h>
#include <kernel/ic.h>


static void suus() {

	kernel_log_("GROS CHIBRE EN PLATRE");

};



static bool gpio_check() {

	kernel_log_("\n\n\n");
	kernel_log_("Querying led file : ");

	file_descriptor fd = fs_open("led");

	if (!fd) {

		kernel_log_("File doesn't exist. Aborting.");

		return false;

	}

	kernel_log("File : %h", fd);

	kernel_log_("Interfacing with gpio : ");

	struct gpio_interface giface;

	bool s = inode_interface(fd, &giface, sizeof(giface));

	if (!s) {

		kernel_log_("Operation failed. Aborting");

		return false;

	}

	fs_close(fd);

	kernel_log("success, %h", giface.pin_mask);


	while (1) {
		gpio_set(&giface, giface.pin_mask);
		debug_delay_ms(250);
		gpio_clear(&giface, giface.pin_mask);
		debug_delay_ms(750);
		kernel_log_("SUUS MA BITE");
	}

	return true;

}


static bool timer_check() {


	kernel_log_("\n\n\n");
	kernel_log_("Querying timer file : ");

	fs_list();

	file_descriptor fd = fs_open("pit_1");

	if (!fd) {

		kernel_log_("File doesn't exist. Aborting.");

		return false;

	}

	kernel_log("File : %h", fd);

	kernel_log_("Interfacing with timer : ");

	struct timer_if iface;

	bool success = inode_interface(fd, &iface, sizeof(iface));

	if (!success) {

		kernel_log_("Operation failed. Aborting");

		return false;

	}

	//fs_close(fd);

	kernel_log_("Initialising timer");

	timer_init(&iface, 1000000, 2000000, &suus);

	kernel_log_("Success");

}


static bool servo_check() {

	kernel_log_("\n\n\n");
	kernel_log_("Querying servo file : ");

	file_descriptor fd = fs_open("servo_0");

	if (!fd) {

		kernel_log_("File doesn't exist. Aborting.");

		return false;

	}

	kernel_log("File : %h", fd);

	kernel_log_("Interfacing with servo : ");

	struct command_if iface;

	bool success = inode_interface(fd, &iface, sizeof(iface));

	if (!success) {

		kernel_log_("Operation failed. Aborting");

		return false;

	}

	//fs_close(fd);

	kernel_log_("Starting channel servo");

	ic_enable_interrupts();

	while(1) {

		command_set(&iface, 5000);

		debug_delay_ms(1000);

		command_set(&iface, 10000);

		debug_delay_ms(1000);

		command_set(&iface, 15000);

		debug_delay_ms(1000);

		command_set(&iface, 20000);

		debug_delay_ms(1000);

	}

	kernel_log_("Success");

	return true;

}


static bool check_init() {

	return servo_check();

}


KERNEL_EMBED_USER_MODULE(timer_check, &check_init);
