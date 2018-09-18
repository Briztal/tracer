//
// Created by root on 9/17/18.
//

#include "command.h"

//Define interfacing and neutralisation functions;
DEFINE_INTERFACE(command_if);

static void ds(uint32_t u) {};

//The neutral command interface. Does nothing;
struct command_if neutral_command_if = {
	.set = &ds,
};