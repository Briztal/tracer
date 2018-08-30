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

	//The required period between two preemptions;
	uint16_t activity_time;

};


struct prc {

	//The process descriptor;
	struct prc_desc desc;

	//The program memory;
	struct prog_mem *prog_mem;

};


//Create a process in the kernel heap with a stack of the required size;
struct prc *prc_create(const struct prc_desc *desc);

//Delete a process, and free its stack;
void prc_delete(struct prc *process);

#endif //TRACER_THREADMANAGER_H
