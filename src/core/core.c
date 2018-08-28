//
// Created by root on 8/28/18.
//


#include "core.h"

#include <stdint.h>

/**
 * core_error : sends the and halt;
 */

void _core_error(const char *msg) {

	while(1) {

		//Send BREAK;
		core_log(msg);

		//Wait for one second;
		core_delay_ms((uint32_t)1000);

	}

}


