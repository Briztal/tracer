//
// Created by root on 8/21/18.
//


#include "mem.h"

#include "heap.h"
#include "krnl.h"

//------------------------------------------------------- Externs ------------------------------------------------------

extern uint32_t _ram_lowest;
extern uint32_t _ram_highest;


//------------------------------------------------------- Globals ------------------------------------------------------

//The heap that will manage the ram block;
static struct heap_head *ram_heap = 0;


//------------------------------------------------------- RAM mgt ------------------------------------------------------

/**
 * ram_init : resets the heap that manages the RAM block.
 *
 * 	Executing this function will invalidate all allocated memory in the ram.
 *
 * 	It must be ran carefully;
 */

void ram_init() {

	//Initialise the heap;
	ram_heap = heap_create(&_ram_lowest, &_ram_highest - &_ram_lowest, &heap_fifo_insertion);

}

/**
 * ram_alloc : allocate a memory block in the RAM;
 *
 * @param size : the size of the memory block to allocate;
 *
 * @return the lowest address of the allocated block;
 */

void *ram_alloc(size_t size) {

	//Cache the heap;
	struct heap_head *heap = ram_heap;

	//If the heap hasn't been initialised :
	if (!heap) {

		//TODO KERNEL ERROR;
		return 0;

	}

	//Allocate and return some data in the heap;
	return heap_malloc(heap, size);

}


/**
 * ram_free : frees an allocated memory block, referenced by its lowest data address;
 *
 * @param ptr : the lowest address of the data block;
 */

void ram_free(void *ptr) {

	//Cache the heap;
	struct heap_head *heap = ram_heap;

	//If the heap hasn't been initialised :
	if (!heap) {

		//TODO KERNEL ERROR;
		return;

	}

	//Free the required block;
	heap_free(heap, ptr);

}

//------------------------------------------------------ prog_mem ------------------------------------------------------



/**
 * prog_mem_create : creates and initialises a program memory struct in the kernel heap;
 *
 * 	A RAM block will be reserved, and an empty heap will be initialised in it, its ownership being transferred
 * 	to the struct;
 *
 * 	A given number of stacks will be created in the heap, and related core_stack structs will be saved in
 * 	the @stacks array.
 *
 * 	The required number of stacks will be majored by the number of hardware stacks;
 *
 * @param ram_size : total size allocated to the program;
 * @param nb_stacks : the number of stacks to create;;
 * @param stacks_size : the size of each stack;
 * @return a ref to the initialised struct, located in the kernel heap;
 */

struct prog_mem *prog_mem_create(size_t ram_size, uint8_t nb_stacks, size_t stacks_size) {

	//Allocate some memory in the RAM to contain the heap;
	void *ram_block = ram_alloc(ram_size);

	//Create a heap owning the whole RAM block;
	struct heap_head *heap = heap_create(ram_block, ram_size, heap_fifo_insertion);//TODO SORTED INSERTION;

	//Major the number of stacks by its maximum;
	if (nb_stacks > NB_THREADS)
		nb_stacks = NB_THREADS;

	//Create the initializer for the prog_mem;
	struct prog_mem progm_init = {

		//Transfer the ownership of the RAM block, for later free;
		.ram_start = ram_block,

		//Transfer the heap ownership;
		.heap = heap,

		//Save the number of stacks;
		.nb_stacks = nb_stacks,
		
	};


	//Correct the stacks size;
	stacks_size = core_correct_size(stacks_size);


	//For each stack to create :
	for (size_t stack_id = nb_stacks; stack_id--;) {

		//Allocate some memory for the thread's stack in the newly created heap;
		void *thread_stack = heap_malloc(heap, stacks_size);

		//Determine the stack's highest address;
		void *stack_reset = (void *) ((uint8_t *)thread_stack + stacks_size);

		//Correct the stack's highest address;
		stack_reset = core_correct_stack_reset(stack_reset);

		//Create the core_stack initializer;
		struct core_stack cs_init = {

			//The stack bound, not corrected;
			.stack_limit = thread_stack,

			//The stack pointer, set to its reset value;
			.sp = stack_reset,

			//The stack reset value, corrected by the core lib;
			.sp_reset = stack_reset,

		};

		//Allocate, initialise and save the struct in the kernel heap;
		progm_init.stacks[stack_id] = kialloc(sizeof(struct core_stack), &cs_init);

	}

	//Create, initialise and return the program memory struct;
	return kialloc(sizeof(struct prog_mem), &progm_init);

}


/**
 * prog_mem_delete : deletes a program memory struct;
 *
 * @param mem : the program memory struct to delete;
 */

void prog_mem_delete(struct prog_mem *mem) {


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
