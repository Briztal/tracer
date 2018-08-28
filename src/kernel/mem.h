//
// Created by root on 8/21/18.
//

#ifndef TRACER_MEMORY_H
#define TRACER_MEMORY_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include <core/type/stack.h>

#include "core/type/heap.h"


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




//Create a process memory struct. Referential data can be allocated in itself;
struct prog_mem *prog_mem_create_special(size_t ram_size, uint8_t nb_stacks, size_t stack_size, bool self_referenced);

//Create a process memory struct in the kernel heap;
static inline struct prog_mem *prog_mem_create(size_t ram_size, uint8_t nb_stacks, size_t stack_size) {
	prog_mem_create_special(ram_size, nb_stacks, stack_size, false);
}

//Delete a process memory struct in the kernel heap;
void prog_mem_delete(struct prog_mem *);


#endif //TRACER_MEMORY_H
