//
// Created by root on 8/28/18.
//


#include "core.h"

#include <stdint.h>

#include <core/util/string.h>


//-------------------------------------------------- Stack provision ---------------------------------------------------


//The default stack provider. Simply return 1ms for process time and does not change the stack pointer;
static void *default_stack_provider(void *stack_pointer) {}

//The process stack provider; Initialised to the default provider;
void *(*core_stack_provider)(void *) = &default_stack_provider;




//----------------------------------------------------- Execution ------------------------------------------------------

/*
 * core_set_process_provider : updates the stack provider;
 */

void core_set_process_provider(void *(*new_provider)(void *)) {

	//Update the new provider;
	core_stack_provider = new_provider;

}


/*
 * core_reset_process_provider : resets the stack provider;
 */

void core_reset_process_provider() {

	//Update the new provider;
	core_stack_provider = &default_stack_provider;

}


//----------------------------------------------------- Execution ------------------------------------------------------

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


//TODO LOG
//Convert an int to its string value and send it;
void core_log_int(uint32_t integer) {

	//Cache an array of the appropriate length;
	char t[32];

	//Convert the integer;
	char *str = itoa(integer, t, 32, 10);

	//Log the value;
	core_log(str);

}


