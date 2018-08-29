/*
  process.h Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

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

#ifndef TRACER_THREADMANAGER_H
#define TRACER_THREADMANAGER_H


#include <core/type/list.h>
#include <kernel/mem.h>
#include "kernel/scheduler/tasks/stask.h"


/*
 * The process library provides functions create, reset and delete scheduler processes;
 */


/**
 * A process can be in different states, in the scheduler's point of view;
 */
enum prcs_state {

	//Execution required;
		PRCS_PENDING = 0,

	//Process stop required by a semaphore;
		PRCS_STOP_REQUIRED = 1,

	//Thread unregistered;
		PRCS_STOPPED = 2,

	//Execution done;
		PRCS_TERMINATION_REQUIRED = 3,

	//Execution done;
		PRCS_TERMINATED = 4,

};


//TODO;
struct prcs {

	//A list head;
	struct list_head head;

	//The program memory;
	struct prog_mem *prog_mem;

	//The process state;
	enum prcs_state state;

	//The task data;//TODO EXEC_ENV
	stask_t *task;

	//uint8_t counter;

	//TODO PRIO DATA;

};


//Create a process in the kernel heap with a stack of the required size;
struct prcs *prcs_create(size_t ram_size, uint8_t nb_stacks, size_t stack_size);

//Reset a process;
void prcs_reset(struct prcs *process);

//Delete a process, and free its stack;
void prcs_delete(struct prcs *process);

//Execute a process. If succeeds, doesn't return;
//void prcs_execute(struct prcs *process);


#endif //TRACER_THREADMANAGER_H
