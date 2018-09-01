/*
  mem.c Part of TRACER

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


#include <core/core.h>
#include <core/debug.h>
#include "mem.h"

#include "core/mem/ram.h"

#include "krnl.h"

//------------------------------------------------------ prog_mem ------------------------------------------------------

/**
 * prog_mem_create_special : creates and initialises a program memory struct in the kernel heap;
 *
 * 	A RAM block will be reserved, and an empty heap will be initialised in it, its ownership being transferred
 * 	to the struct;
 *
 * 	If the current program memory to initialise is the kernel memory, kernel dynamic memory is not ready, and would
 * 	lead to manipulating the heap we currently initialise, all dynamic allocations will be made in the created heap;
 *
 * @param ram_size : total size allocated to the program;
 * @param self_referenced : if set, the struct will be allocated in the heap we created;
 * @return a ref to the initialised struct, located in the kernel heap;
 */

struct prog_mem *prog_mem_create(size_t ram_size, bool self_referenced) {

	//Allocate some memory in the RAM to contain the heap;
	void *ram_block = ram_alloc(ram_size);

	//Create a heap owning the whole RAM block;
	struct heap_head *heap = heap_create(ram_block, ram_size, heap_fifo_insertion);//TODO SORTED INSERTION;

	//Create the initializer for the prog_mem;
	struct prog_mem progm_init = {

		//Transfer the ownership of the RAM block, for later free;
		.ram_start = ram_block,

		//Transfer the heap ownership;
		.heap = heap,

		//Save the number of stacks;
		.nb_stacks = 0,

	};


	//If the progmem must contain its referential data :
	if (self_referenced) {

		//Initialise the progmem in the heap we created;
		return heap_ialloc(heap, sizeof(struct prog_mem), &progm_init);


	} else {

		//Create, initialise and return the program memory struct;
		return kialloc(sizeof(struct prog_mem), &progm_init);

	}

}


/**
 * prog_mem_init : clears the heap, and allocates the required number of stacks in it;
 *
 * 	A given number of stacks will be created in the heap, and related core_stack structs will be declared in the
 * 	kernel heap, and saved in the @stacks array;
 *
 * 	The required number of stacks will be majored by the number of hardware stacks;
 *
 * @param nb_stacks : the number of stacks to create;;
 * @param stacks_size : the size of each stack;
 * @param self_referenced : if set, all dynamic allocations will be made in the heap we created;
 * @return a ref to the initialised struct, located in the kernel heap;
 */


void prog_mem_create_stacks(struct prog_mem *mem, uint8_t nb_stacks, size_t stacks_size) {

	//Major the number of stacks by its maximum;
	if (nb_stacks > NB_THREADS)
		nb_stacks = NB_THREADS;


	//If the process already has stacks initialised :
	if (mem->nb_stacks) {

		//Error,
		kernel_panic("mem.c : prog_mem_create_stacks : stacks already created;");

	}

	//Save the number of stacks;
	mem->nb_stacks = nb_stacks;

	//Correct the stacks size;
	stacks_size = stack_correct_size(stacks_size);

	//Reset the heap;
	heap_reset(mem->heap);

	//For each stack to create :
	for (size_t stack_id = nb_stacks; stack_id--;) {

		//Allocate some memory for the thread's stack in the newly created heap;
		void *thread_stack = heap_malloc(mem->heap, stacks_size);

		//Determine the stack's highest address;
		void *stack_reset = (void *) ((uint8_t *) thread_stack + stacks_size);

		//Correct the stack's highest address;
		stack_reset = stack_correct_reset_ptr(stack_reset);

		//Create the core_stack initializer;
		struct core_stack cs_init = {

			//The stack bound, not corrected;
			.stack_limit = thread_stack,

			//The stack pointer, set to its reset value;
			.sp = stack_reset,

			//The stack reset value, corrected by the core lib;
			.sp_reset = stack_reset,

		};

		//Allocate, initialise and save the struct in the heap;
		mem->stacks[stack_id] = kialloc(sizeof(struct core_stack), &cs_init);

	}

}


/**
 * prog_mem_delete : deletes a program memory struct;
 *
 * @param mem : the program memory struct to delete;
 */

void prog_mem_delete_from_kernel_heap(struct prog_mem *mem) {

	//For each allocated stack :
	for (uint8_t stack_id = mem->nb_stacks; stack_id--;) {

		//Free the core_stack struct;
		kfree(mem->stacks[stack_id]);

	}

	//Free the RAM block;
	ram_free(mem->ram_start);

	//Free the prog_mem struct;
	kfree(mem);

}
