//
// Created by root on 9/18/18.
//

#ifndef TRACER_KX_PIT_CHANNEL_DATA_H
#define TRACER_KX_PIT_CHANNEL_DATA_H

#include <if/timer.h>

struct channel_specs {

	//The timer if ref struct;
	const struct timer_if iface;

	//The channel's name;
	const char *name;

	//The channel's interrupt channel;
	const uint16_t interrupt_channel;

	//The channel's handler;
	void (*const handler)(void);

};

#endif //TRACER_KX_PIT_CHANNEL_DATA_H
