//
// Created by root on 10/16/18.
//

#ifndef TRACER_STACK_H
#define TRACER_STACK_H

/**
 * The stack provides the two pointers that define a descending stack;
 */

struct stack_data {
	
	//The current stack_data pointer, mutable;
	void *sp;
	
	//The lowest bound of the stack_data pointer, also used to free the stack_data, constant;
	void *const stack_limit;
	
	//The highest value the stack_data pointer can take, constant;
	void *const sp_reset;
	
};


#endif //TRACER_STACK_H
