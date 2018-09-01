//
// Created by root on 8/28/18.
//


#include "core.h"

#include <stdint.h>

#include <util/string.h>



//TODO REORDER
//TODO REORDER
//TODO REORDER
//TODO REORDER
//TODO REORDER
//TODO REORDER
//TODO REORDER
//TODO REORDER
//TODO REORDER
//TODO REORDER
//TODO REORDER
//TODO REORDER


//-------------------------------------------------- Stack provision ---------------------------------------------------


//The process stack provider; Initialised to the default provider;
void *(*core_stack_provider)(uint8_t, void *);


/*
 * core_set_stack_provider : updates the stack provider;
 */

void core_set_stack_provider(void *(*new_provider)(uint8_t, void *)) {

	//Update the new provider;
	core_stack_provider = new_provider;

}


//----------------------------------------------------- Execution ------------------------------------------------------

/**
 * core_error : sends the and halt;
 */

void _core_error(const char *msg) {

	ic_disable_interrupts();

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


//TODO LOG
//Convert an int to its string value and send it;
void core_log_hex(uint32_t integer) {

	//Cache an array of the appropriate length;
	char t[32];

	//Convert the integer;
	char *str = itoa(integer, t, 32, 16);

	//Log the value;
	core_log(str);

}


//-------------------------------------------------- Critical sections -------------------------------------------------

//The critical section counter;
static uint32_t critical_section_counter = 0;

/**
 * ic_enter_critical_section : called whenever any part of the code must execute a critical section;
 */

void ic_enter_critical_section() {

	//Disable interrupts;
	ic_disable_interrupts();

	//Increment the section counter;
	critical_section_counter++;

}


/**
 * ic_leave_critical_section : called whenever any part of the code leaves a critical section;
 */

void ic_leave_critical_section() {

	//To safely detect any code error, disable interrupts;
	ic_disable_interrupts();

	//If there was a code error (more leave called than enter);
	if (!critical_section_counter) {

		//TODO EXCEPTION;

	}

	//If we must update interrupts again :
	if (!(--critical_section_counter)) {

		//Enable interrupts;
		ic_enable_interrupts();

	}

}


/**
 * ic_force_critical_section_exit : resets the critical section counter and enables interrutps.
 * 	Highly unsafe, only the kernel must execute this function;
 */

void ic_force_critical_section_exit() {

	//Reset the critical section counter;
	critical_section_counter = 0;

	//Enable all interrupts;
	ic_enable_interrupts();

}

