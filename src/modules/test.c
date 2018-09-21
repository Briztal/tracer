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

	struct gpio_if giface;

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

	uint32_t period = timer_convert_to_tics(&iface, 10000000, 2000000);

	timer_init(&iface, period, &suus);

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

	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!
	//TODO TEST SERVO DEACTIVATION !!!!!!!!!!

	while(1) {

		command_set(&iface, 5000);

		debug_delay_ms(1000);

		command_set(&iface, 10000);

		debug_delay_ms(1000);

		command_set(&iface, 15000);

		debug_delay_ms(1000);

		command_set(&iface, 20000);

		debug_delay_ms(1000);


		//TODO TEST!!!!!!!!!!
		//TODO TEST!!!!!!!!!!
		//TODO TEST!!!!!!!!!!
		//TODO TEST!!!!!!!!!!
		command_set(&iface, 0);

		debug_delay_ms(1000);

	}

	kernel_log_("Success");

	return true;

}


static bool pwm_check() {

	kernel_log_("\n\n\n");
	kernel_log_("Querying pwm file : ");

	file_descriptor fd = fs_open("pwm_0");

	if (!fd) {

		kernel_log_("File doesn't exist. Aborting.");

		return false;

	}

	kernel_log("File : %h", fd);

	kernel_log_("Interfacing with pwm : ");

	struct command_if iface;

	bool success = inode_interface(fd, &iface, sizeof(iface));

	if (!success) {

		kernel_log_("Operation failed. Aborting");

		return false;

	}

	//fs_close(fd);

	kernel_log_("Starting channel pwm");

	ic_enable_interrupts();

	while(1) {

		command_set(&iface, 120000);

		debug_delay_ms(1000);

		command_set(&iface, 240000);

		debug_delay_ms(1000);

		command_set(&iface, 360000);

		debug_delay_ms(1000);

		command_set(&iface, 480000);

		debug_delay_ms(1000);

		command_set(&iface, 0);

		debug_delay_ms(1000);

	}

	kernel_log_("Success");

	return true;

}

static bool pwm_check_4() {

	kernel_log_("Querying pwms file : ");

	file_descriptor fd0 = fs_open("pwm_0");
	file_descriptor fd1 = fs_open("pwm_1");
	file_descriptor fd2 = fs_open("pwm_2");
	file_descriptor fd3 = fs_open("pwm_3");

	if (!(fd0 && fd1 && fd2 && fd3)) {

		kernel_log_("File doesn't exist. Aborting.");

		return false;

	}

	kernel_log_("Interfacing with pwms : ");

	struct command_if iface0;
	struct command_if iface1;
	struct command_if iface2;
	struct command_if iface3;

	bool success0 = inode_interface(fd0, &iface0, sizeof(iface0));
	bool success1 = inode_interface(fd1, &iface1, sizeof(iface1));
	bool success2 = inode_interface(fd2, &iface2, sizeof(iface2));
	bool success3 = inode_interface(fd3, &iface3, sizeof(iface3));

	if (!(success0 && success1 && success2 && success3)) {

		kernel_log_("Operation failed. Aborting");

		return false;

	}

	//fs_close(fd);

	kernel_log_("Starting channel pwm");

	ic_enable_interrupts();

	while(1) {

		command_set(&iface0, 120000);
		command_set(&iface1, 120000);
		command_set(&iface2, 120000);
		command_set(&iface3, 120000);

		debug_delay_ms(1000);

		command_set(&iface0, 240000);
		command_set(&iface1, 240000);
		command_set(&iface2, 240000);
		command_set(&iface3, 240000);


		debug_delay_ms(1000);


		command_set(&iface0, 360000);
		command_set(&iface1, 360000);
		command_set(&iface2, 360000);
		command_set(&iface3, 360000);

		debug_delay_ms(1000);


		command_set(&iface0, 480000);
		command_set(&iface1, 480000);
		command_set(&iface2, 480000);
		command_set(&iface3, 480000);

		debug_delay_ms(1000);


		command_set(&iface0, 0);
		command_set(&iface1, 0);
		command_set(&iface2, 0);
		command_set(&iface3, 0);

		debug_delay_ms(1000);

	}

	kernel_log_("Success");

	return true;

}

static bool check_init() {

	return pwm_check_4();

}


KERNEL_EMBED_USER_MODULE(timer_check, &check_init);
