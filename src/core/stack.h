//
// Created by root on 8/21/18.
//

#ifndef TRACER_CORE_STACK_H
#define TRACER_CORE_STACK_H


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


#endif //TRACER_CORE_STACK_H
