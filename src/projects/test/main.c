//
// Created by root on 10/13/18.
//
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <syscall.h>
#include <prc.h>
#include <stdio.h>
#include <if/gpio.h>
#include <if/timer.h>
#include <if/cmd.h>
#include <if/port.h>

#include <modules/pwm/pwm.h>
#include <std/app_hook.h>
#include <kernel/debug/printk.h>


/*
static void suus() {
	
	printk("GROS CHIBRE EN PLATRE");
	
};



static bool gpio_check() {
	
	printk("\n\n\n");
	printk("Querying led file : ");
	
	FILE * fd = sys_open("0");
	
	if (!fd) {
		
		printk("File doesn't exist. Aborting.");
		
		return false;
		
	}
	
	printkf("File : %h", fd);
	
	
	//Create the pin configuration structure;
	struct port_pin_config pin_config = {
		
		.mux_channel = 1,
		
		.direction = PIN_OUTPUT,
		
		.output_mode = PORT_HIGH_DRIVE,
		
		.slew_rate = PORT_HIGH_RATE,
		
	};
	
	printk("Initialising gpio : ");
	
	iop_init(fd, &pin_config, sizeof(pin_config));
	
	
	printk("Interfacing with gpio : ");
	
	struct gpio_if giface;
	
	bool s = iop_interface(fd, &giface, sizeof(giface));
	
	if (!s) {
		
		printk("Operation failed. Aborting");
		
		return false;
		
	}
	
	//sys_close(fd);
	
	printkf("success, %h", giface.pin_mask);
	
	
	while (1) {
		gpio_set(&giface, giface.pin_mask);
		debug_delay_ms(250);
		gpio_clear(&giface, giface.pin_mask);
		debug_delay_ms(750);
		printk("SUUS MA BITE");
	}
	
	return true;
	
}


static bool timer_check() {
	
	
	printk("\n\n\n");
	printk("Querying timer file : ");
	
	sys_list();
	
	FILE * fd = sys_open("pit_1");
	
	if (!fd) {
		
		printk("File doesn't exist. Aborting.");
		
		return false;
		
	}
	
	printkf("File : %h", fd);
	
	printk("Interfacing with timer : ");
	
	struct timer_if iface;
	
	bool success = iop_interface(fd, &iface, sizeof(iface));
	
	if (!success) {
		
		printk("Operation failed. Aborting");
		
		return false;
		
	}
	
	//sys_close(fd);
	
	printk("Initialising timer");
	
	uint32_t period = timer_convert_to_tics(&iface, 10000000, 2000000);
	
	timer_init(&iface, period, &suus);
	
	printk("Success");
	
}



static bool pwm_check_4() {
	
	printk("Querying pwms file : ");
	
	FILE * fd0 = sys_open("pwm_0");
	FILE * fd1 = sys_open("pwm_1");
	FILE * fd2 = sys_open("pwm_2");
	FILE * fd3 = sys_open("pwm_3");
	
	if (!(fd0 && fd1 && fd2 && fd3)) {
		
		printk("File doesn't exist. Aborting.");
		
		return false;
		
	}
	
	printk("Interfacing with pwms : ");
	
	struct cmd_if iface0;
	struct cmd_if iface1;
	struct cmd_if iface2;
	struct cmd_if iface3;
	
	bool success0 = iop_interface(fd0, &iface0, sizeof(iface0));
	bool success1 = iop_interface(fd1, &iface1, sizeof(iface1));
	bool success2 = iop_interface(fd2, &iface2, sizeof(iface2));
	bool success3 = iop_interface(fd3, &iface3, sizeof(iface3));
	
	if (!(success0 && success1 && success2 && success3)) {
		
		printk("Operation failed. Aborting");
		
		return false;
		
	}
	
	//sys_close(fd);
	
	printk("Starting channel pwm");
	
	
	while(1) {
		
		cmd_set(&iface0, 120000);
		cmd_set(&iface1, 240000);
		cmd_set(&iface2, 360000);
		cmd_set(&iface3, 480000);
		
		debug_delay_ms(1000);
		
		cmd_set(&iface0, 480000);
		cmd_set(&iface1, 120000);
		cmd_set(&iface2, 240000);
		cmd_set(&iface3, 360000);
		
		
		debug_delay_ms(1000);
		
		
		cmd_set(&iface0, 360000);
		cmd_set(&iface1, 480000);
		cmd_set(&iface2, 120000);
		cmd_set(&iface3, 240000);
		
		debug_delay_ms(1000);
		
		
		cmd_set(&iface0, 240000);
		cmd_set(&iface1, 360000);
		cmd_set(&iface2, 480000);
		cmd_set(&iface3, 120000);
		
		debug_delay_ms(1000);
		
		command_disable(&iface0);
		command_disable(&iface1);
		command_disable(&iface2);
		cmd_disable(&iface3);

		debug_delay_ms(1000);
	}
	
	printk("Success");
	
	return true;
	
}


static bool pwm_check() {
	
	
	printk("\n\n\n");
	
	//sys_list();
	
	printk("Querying pwm file : ");
	
	FILE * fd = sys_open("pwm_0");
	
	if (!fd) {
		
		printk("File doesn't exist. Aborting.");
		
		return false;
		
	}
	
	printkf("File : %h", fd);
	
	const char gpio_name[] = "0";
	const char timer_name[] = "pit_0";
	
	
	
	//Create the pin configuration structure;
	struct port_pin_config pin_config = {
		
		.mux_channel = 1,
		
		.direction = PIN_OUTPUT,
		
		.output_mode = PORT_HIGH_DRIVE,
		
		.slew_rate = PORT_HIGH_RATE,
		
	};
	
	
	struct pwm_config cfg = {
		
		.gpio_name = gpio_name,
		.gpio_config = &pin_config,
		.gpio_config_size = sizeof(pin_config),
		
		.timer_name = timer_name,
		.timer_config = 0,
		.timer_config_size = 0,
		
	};
	
	
	printk("Initialising pwm : ");
	
	bool success = iop_init(fd, &cfg, sizeof(cfg));
	
	if (!success) {
		
		printk("Operation failed. Aborting");
		
		return false;
	}
	
	printk("Interfacing with pwm : ");
	
	struct cmd_if iface;
	
	
	success = iop_interface(fd, &iface, sizeof(iface));
	
	if (!success) {
		
		printk("Operation failed. Aborting");
		
		return false;
		
	}
	
	//sys_close(fd);
	
	printk("Starting channel pwm");
	
	
	while(1) {
		
		cmd_set(&iface, 120000);
		
		debug_delay_ms(1000);
		
		cmd_set(&iface, 240000);
		
		debug_delay_ms(1000);
		
		cmd_set(&iface, 360000);
		
		debug_delay_ms(1000);
		
		cmd_set(&iface, 480000);
		
		debug_delay_ms(1000);
		
		cmd_set(&iface, 0);
		
		debug_delay_ms(1000);
		
	}
	
	printk("Success");
	
	return true;
	
}

void kernel_second_process(void *args, size_t args_size){
	
	
	
	uint8_t i = 3;
	
	while(1) {
		
		printk("BITE");
		
		i--;
		
		if (!i) {
			
			sys_exit();
			
		}
		
	}
	
}
 
 		*/



void main(void *args, size_t args_size) {
	
	//gpio_check();
	//pwm_check();
	
	/*
	struct prc_desc desc = {
		.function = &kernel_second_process,
		.args_size = 0,
		.args = 0,
	};
	
	struct prc_req req = {
		.stack_size = 2048,
		.activity_time = 3000,
		.ram_size = 4096
	};
	
	sys_exec(&desc, &req);
	*/
	while(1) {
		
		//kernel__printkf("SUUS", 0, 0);
		
		//debug_delay_ms(1000);
		
	}
	
}


struct prc_desc desc = {
	.function = &main,
	.args_size = 0,
	.args = 0,
};


struct prc_req req = {
	.stack_size = 2048,
	.activity_time = 3000,
	.ram_size = 4096
};


HOOK_APP(test, &req, &desc)
