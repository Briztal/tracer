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

#include "pmem.h"

#include <mem.h>

#include <kernel/hard.h>

#include <kernel/mem/ram.h>


//------------------------------------------------------ pmem ------------------------------------------------------

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

void prc_mem_create_heap(struct pmem *mem, size_t ram_size) {
	
	//Allocate some memory in the RAM to contain the heap;
	void *ram_block = ram_alloc(ram_size);
	
	//Create a heap owning the whole RAM block;
	struct heap_head *heap = heap_create(ram_block, ram_size, heap_fifo_insertion);//TODO SORTED INSERTION;
	
	//Create the initializer for the pmem;
	struct pmem progm_init = {
		
		//Transfer the ownership of the RAM block, for later free;
		.ram_start = ram_block,
		
		//Transfer the heap ownership;
		.heap = heap,
		
	};
	
	//Initialise the program memory;
	memcpy(mem, &progm_init, sizeof(struct pmem));
	
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

void prc_mem_reset(struct pmem *mem, size_t stacks_size) {
	
	//Reset the heap;
	heap_reset(mem->heap);
	
	//Allocate some memory for the thread's stack_data in the heap;
	void *thread_stack = heap_malloc(mem->heap, stacks_size);
	
	//Determine the stack_data's highest address;
	void *stack_reset = (void *) ((uint8_t *) thread_stack + stacks_size);
	
	//Correct the stack_data's highest address for proper alignment;
	stack_reset = __proc_stack_align(stack_reset);
	
	//Create the stack_data initializer;
	struct stck ps_init = {
		
		//The stack_data bound, not corrected;
		.stack_limit = thread_stack,
		
		//The stack_data pointer, set to its reset value;
		.sp = stack_reset,
		
		//The stack_data reset value, corrected by the core lib;
		.sp_reset = stack_reset,
		
	};
	
	//Initialise the stack_data;
	memcpy(&mem->stack, &ps_init, sizeof(struct stck));
	
}


/**
 * prog_mem_delete : deletes a program memory struct;
 *
 * @param mem : the program memory struct to delete;
 */

void prc_mem_clean(struct pmem *mem) {
	
	//Free the RAM block;
	ram_free(mem->ram_start);
	
}
