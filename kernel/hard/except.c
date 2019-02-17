/*
  except.c Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/


//-------------------------------------------------- Critical sections -------------------------------------------------

#include <kernel/hard/except.h>

#include <kernel/panic.h>

//The critical section counter;
static uint32_t critical_section_counter = 0;


/**
 * ic_enter_critical_section : called whenever any part of the code must execute a critical section;
 */

void critical_section_enter() {

	//Disable exceptions;
	__xcpt_disable();

	//Increment the section counter;
	critical_section_counter++;

}


/**
 * ic_leave_critical_section : called whenever any part of the code leaves a critical section;
 */

void critical_section_leave() {

	//To safely detect any code error, disable interrupts;
	__xcpt_disable();

	//If there was a code error (more leave called than enter);
	if (!critical_section_counter) {

		//Trigger a kernel panic, this should not have happened;
		kernel_panic("except.c : called critical_section_leave while not in critical section;");

	}

	//If we must update interrupts again :
	if (!(--critical_section_counter)) {

		//Enable interrupts;
        __xcpt_enable();

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
    __xcpt_enable();

}
