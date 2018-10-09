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

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include <kernel/proc/proc.h>

#include "heap.h"


/*
 * The program memory environment reflects the memory structure a program can access. It is a contiguous memory block
 * containing primarily a heap.
 *
 * 	From this heap, several stacks can be allocated, their references will be saved in the @stacks array;
 */

//TODO MULTITHREADING PATCH, DEFINE IN ARCH MAKEFILE;
#define NB_THREADS 1

struct prog_mem {

	//The lowest address of the block (for memory free purposes);
	void *const ram_start;

	//The heap reference;
	struct heap_head *heap;

	//The number of stacks available for the program;
	uint8_t nb_stacks;

	//The stack references array;
	struct proc_stack *stacks[NB_THREADS];

};


//Create a process memory struct, containing only a heap;
struct prog_mem *prog_mem_create(size_t ram_size);

//Initialise the process memory : reset the heap, and create as many stacks as required;
void prog_mem_create_stacks(struct prog_mem *mem, uint8_t nb_stacks, size_t stack_size);

//Delete the process memory. All references must be contained in the kernel heap;
void prog_mem_delete_from_kernel_heap(struct prog_mem *);



#endif //TRACER_MEMORY_H
