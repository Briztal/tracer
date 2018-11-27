//
// Created by root on 9/19/18.
//

#ifndef TRACER_SERVO_CHANNEL_H
#define TRACER_SERVO_CHANNEL_H


#include <fs/iinode.h>
#include <interface/gpio.h>
#include <interface/cmd.h>


struct channel_specs {

	//The command if ref struct;
	const struct cmd_if iface;

	//The name of the gpio file;
	const char *gpio_name;

	//The name of the channel file;
	const char *channel_name;

	//The channel's maximal duration;
	const uint32_t period;

};


#endif //TRACER_SERVO_CHANNEL_H
