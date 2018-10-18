//
// Created by root on 9/17/18.
//

#include "cmd.h"

#include <mem.h>


//Define interfacing and neutralisation functions;
DEFINE_INTERFACE(cmd_if)

static void ds(uint32_t u) {}
static void dbl() {}

//The neutral command interface. Does nothing;
struct cmd_if neutral_cmd_if = {
	.sett = &ds,
	.disable = &dbl,
};