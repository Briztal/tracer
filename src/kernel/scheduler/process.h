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
enum prc_state {

	//Execution required;
		PRC_PENDING = 0,

	//Process stop required by a semaphore;
		PRC_STOP_REQUIRED = 1,

	//Thread unregistered;
		PRC_STOPPED = 2,

	//Execution done;
		PRC_TERMINATION_REQUIRED = 3,

	//Execution done;
		PRC_TERMINATED = 4,

};


struct prc_desc {

	//The task's function;
	void (*function)(void *args);

	//The function's arguments;
	const void *args;

	//The arguments size;
	size_t args_size;

	//The required ram size;
	size_t ram_size;

	//The required number of threads;
	uint8_t nb_threads;
	
	//The size available for each stack;
	size_t stack_size;

	//The period between two preemptions for this task;
	uint16_t activity_time;

};


struct prc {

	//A list head;
	struct list_head head;

	//The process descriptor;
	struct prc_desc desc;

	//The program memory;
	struct prog_mem *prog_mem;

	//The process state;
	enum prc_state state;

};


//Create a process in the kernel heap with a stack of the required size;
struct prc *prc_create(const struct prc_desc *desc);

//Delete a process, and free its stack;
void prc_delete(struct prc *process);

#endif //TRACER_THREADMANAGER_H
