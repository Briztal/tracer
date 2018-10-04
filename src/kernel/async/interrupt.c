//
// Created by root on 9/8/18.
//

#include "interrupt.h"

//-------------------------------------------------- Critical sections -------------------------------------------------

//The critical section counter;
static uint32_t critical_section_counter = 0;


/**
 * ic_enter_critical_section : called whenever any part of the code must execute a critical section;
 */

void critical_section_enter() {

	//Disable interrupts;
	exceptions_disable();

	//Increment the section counter;
	critical_section_counter++;

}


/**
 * ic_leave_critical_section : called whenever any part of the code leaves a critical section;
 */

void critical_section_leave() {

	//To safely detect any code error, disable interrupts;
	exceptions_disable();

	//If there was a code error (more leave called than enter);
	if (!critical_section_counter) {

		//TODO EXCEPTION;

	}

	//If we must update interrupts again :
	if (!(--critical_section_counter)) {

		//Enable interrupts;
		exceptions_enable();

	}

}


/**
 * ic_force_critical_section_exit : resets the critical section counter and enables interrutps.
 * 	Highly unsafe, only the kernel must execute this function;
 */

void critical_section_force_exit() {

	//Reset the critical section counter;
	critical_section_counter = 0;

	//Enable all interrupts;
	exceptions_enable();

}
