/*
  stack.h Part of TRACER

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


//Reset an allocated stack. Saves the arg in the stack;
void stack_reset(struct core_stack *stack, void (*init_f)(), void (*exit_f)(), void *arg);


//Get the arg saved in the stack;
void *stack_get_arg();


#endif //TRACER_CORE_STACK_H
