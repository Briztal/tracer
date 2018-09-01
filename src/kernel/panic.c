/*
  panic.c Part of TRACER

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

#include "panic.h"


//------------------------------------------------------ Privates ------------------------------------------------------

//The panic log stack, a static array of char *;
static const char *panic_log_stack[KERNEL_LOG_STACK_SIZE];

//The insertion index in the log stack;
static size_t log_index;



//------------------------------------------------------ Log stack -----------------------------------------------------

/**
 * panic_push_log : pushes the log in the log stack so that it can be printed in case of kernel panic;
 *
 * 	The message is not copied, only its reference is stored.
 *
 * 	A call to @panic_pop_log must follow this call before @log context ends, to insure data intergrity.
 *
 * 	If the call is not made, the log stack won't empty itself until reset is called;
 *
 * @param log : the message to display if a kernel panic happens;
 */

void panic_push_log(const char *log) {

	//If we still can insert in the log stack :
	if (log_index < KERNEL_LOG_STACK_SIZE) {
		panic_log_stack[log_index] = log;
	}

	//Increment the log index;
	log_index++;

}


/**
 * panic_pop_log : pops a log message from the log stack;
 *
 * 	A call to @panic_push_log must have been made before calling this function, or the pop will fail.
 *
 * 	If so, a debug message will be sent.
 */

void panic_pop_log() {

	//If the log index can be decremented :
	if (log_index) {

		//Decrement it.
		log_index--;

	}

	//If not, there has been an error in the log stack management;
	//TODO DEBUG LOG;

}


/**
 * panic_reset_log : resets the log stack;
 *
 * 	This function should be called at beginning and end of every system call;
 *
 * 	If the log stack is not empty, a debug message will be sent;
 */

void panic_reset_log() {

	//If the log index is not null :
	if (log_index) {
		//TODO LOG : A RESET HAS BEEN CALLED, WHEN THE LOG STACK WAS NOT EMPTY. ERROR IN LOG STACK MANAGEMENT;
	}

	//Reset the log stack;
	log_index = 0;

}
