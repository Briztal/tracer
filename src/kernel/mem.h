//
// Created by root on 8/21/18.
//

#ifndef TRACER_MEMORY_H
#define TRACER_MEMORY_H


#include <stdint.h>

#include <stddef.h>

#include <core/stack.h>

#include "heap.h"

/*
 * The memory manager manages the available RAM memory.
 *
 * 	It uses variables defined in the unified linker script to manage the whole available RAM block;
 */


//------------------------------------------------------- RAM mgt ------------------------------------------------------


//Initialise memory management values. Will invalidate all previously reserved memory;
void ram_init();

//Reserve some memory in RAM;
void *ram_alloc(size_t);

//Release some reserved memory;
void ram_free(void *);


//---------------------------------------------------- Progmem ---------------------------------------------------

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
	const uint8_t nb_stacks;

	//The stack references array;
	struct core_stack *stacks[NB_THREADS];

};


//Create a process memory struct in the kernel heap;
struct prog_mem *prog_mem_create(size_t ram_size, uint8_t nb_stacks, size_t stack_size);

//Delete a process memory struct in the kernel heap;
void prog_mem_delete(struct prog_mem *);


#endif //TRACER_MEMORY_H
