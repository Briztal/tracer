//
// Created by root on 8/21/18.
//

#ifndef TRACER_CORE_STACK_H
#define TRACER_CORE_STACK_H

#include <stddef.h>

/**
 * The core stack provides the two pointers that define a descending stack;
 */

struct core_stack {

	//The lowest bound of the stack pointer, also used to free the stack, constant;
	void *const stack_limit;

	//The current stack pointer, mutable;
	void *sp;

	//The highest value the stack pointer can take, constant;
	void *const sp_reset;

};


//The stack alignment, a const global power of 2 defined in the arch file;
extern size_t core_stack_alignment;

//Determine the closest inferior stack size, that would respect alignment requirements;
static inline size_t stack_correct_size(size_t stack_size) {

	//@core_stack_alignment is a power of 2, decrement and we obtain the mask of bits to reset;
	return stack_size & ~(core_stack_alignment - 1);

}

//Determine the closest inferior address, that would respect alignment requirements;
static inline void *stack_correct_reset_ptr(void *stack_reset) {

	//@core_stack_alignment is a power of 2, decrement and we obtain the mask of bits to reset;
	//TODO REMOVE THE OFFSET;
	return (void *) ((((size_t) stack_reset) & ~(core_stack_alignment - 1)) - 16);
}


#endif //TRACER_CORE_STACK_H
