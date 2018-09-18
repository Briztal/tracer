//
// Created by root on 9/18/18.
//

#ifndef TRACER_KX_PIT_CHANNEL_DATA_H
#define TRACER_KX_PIT_CHANNEL_DATA_H

#include <kernel/interface/timer.h>

struct kx_pit_channel_data {

	//The timer interface ref struct;
	struct timer_if_ref ref;

	//The channel's name;
	const char *name;

	//The channel's interrupt channel;
	const uint16_t interrupt_channel;

	//The channel's handler;
	void (*const handler)(void);

};

#endif //TRACER_KX_PIT_CHANNEL_DATA_H
