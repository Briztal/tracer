//
// Created by root on 9/17/18.
//

#ifndef TRACER_CMD_H
#define TRACER_CMD_H

#include <stdint.h>
#include <stdbool.h>
#include <util/string.h>
#include "iface.h"

//The command interface references a single value that can be adjusted;
struct command_if {

	//Set the value;
	void (*set)(struct command_if *, uint32_t);

};

//Declare interfacing and neutralisation functions;
DECLARE_INTERFACE(command_if);

//The neutral command interface. Does nothing;
extern struct command_if neutral_command_if;

#endif //TRACER_CMD_H
