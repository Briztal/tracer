//
// Created by root on 8/20/18.
//

#include "ic.h"

#include <stdint.h>>


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