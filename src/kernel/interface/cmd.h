//
// Created by root on 9/17/18.
//

#ifndef TRACER_CMD_H
#define TRACER_CMD_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "iface.h"


//The command interface references a single value that can be adjusted;
struct cmd_if {

	/*
	 * Constants;
	 */

	//The minimal value the command can take;
	const uint32_t min;

	//The maximal value the command can take;
	const uint32_t max;

	//The minimal value expressed in another dimension, related to the command; Used for conversion;
	const uint32_t hl_min;

	//The maximal value expressed in another dimension, related to the command; Used for conversion;
	const uint32_t hl_max;

	/*
	 * Functions;
	 */

	//Set to a certain value;
	void (*const sett)(uint32_t);

	//Disable;
	void (*const disable)();

};

//Declare interfacing and neutralisation functions;
DECLARE_INTERFACE(cmd_if)

//The neutral command interface. Does nothing;
extern struct cmd_if neutral_cmd_if;


static inline void cmd_set(const struct cmd_if *iface, uint32_t value) {
	(*(iface->sett))(value);
}


static inline void cmd_disable(const struct cmd_if *iface) {
	(*(iface->disable))();
}

#endif //TRACER_CMD_H
