//
// Created by root on 9/19/18.
//

#ifndef TRACER_SERVO_CHANNEL_H
#define TRACER_SERVO_CHANNEL_H


#include <kernel/fs/inode.h>
#include <kernel/interface/gpio.h>
#include <kernel/interface/cmd.h>

struct channel_specs {

	//The command interface ref struct;
	const struct cmd_if iface;

	//The name of the gpio file;
	const char *gpio_name;

	//The name of the channel file;
	const char *channel_name;

	//The channel's maximal duration;
	const uint32_t max_duration;

};


#endif //TRACER_SERVO_CHANNEL_H
