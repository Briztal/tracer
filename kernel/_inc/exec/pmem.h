/*
  mem.h Part of TRACER

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

#ifndef TRACER_MEMORY_H
#define TRACER_MEMORY_H


/*------------------------------------------------------- Includes ------------------------------------------------------*/

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include "../../../heap.h"

#include <kernel/exec/stck.h>

#include <macro/incr_call.h>

#include "kernel/res/coproc.h"


/*------------------------------------------------- Make Parameters ----------------------------------------------------*/

/*
 * The program memory environment reflects the memory structure a program can access. It is a contiguous memory block
 * containing primarily a heap.
 *
 * 	From this heap, several stacks can be allocated, their references will be saved in the @stacks array;
 */

struct pmem {

	/*The lowest address of the block (for memory free purposes);*/
	void *const ram_start;

	/*The heap reference;*/
	struct heap_head *heap;

	/*The stack references array;*/
	struct stck stack;

	/*The struct to contain coprocessors contexts;*/
	struct coprocs_contexts contexts;

};


/*Create a process memory struct, containing only a heap;*/
void prc_mem_create_heap(struct pmem *mem, size_t ram_size);

/*Initialise the process memory : reset the heap, and create as many stacks as required;*/
void prc_mem_reset(struct pmem *mem, size_t stacks_size);

/*Delete the process memory. All references must be contained in the kernel heap;*/
void prc_mem_clean(struct pmem *mem);



#endif /*TRACER_MEMORY_H*/
