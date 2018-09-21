//
// Created by root on 9/17/18.
//

#include "command.h"

#include <util/string.h>


//Define interfacing and neutralisation functions;
DEFINE_INTERFACE(command_if);

static void ds(uint32_t u) {};
static void dbl() {};

//The neutral command interface. Does nothing;
struct command_if neutral_command_if = {
	.sett = &ds,
	.disable = &dbl,
};