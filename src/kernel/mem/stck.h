//
// Created by root on 10/16/18.
//

#ifndef TRACER_STACK_H
#define TRACER_STACK_H

/**
 * The stack provides three pointers that define a descending stack;
 */

struct stck {
	
	//The current stack pointer, mutable;
	void *sp;
	
	//The lowest bound of the stack pointer, also used to free the stack, constant;
	void *const stack_limit;
	
	//The highest value the stack pointer can take, constant;
	void *const sp_reset;
	
};


#endif //TRACER_STACK_H
