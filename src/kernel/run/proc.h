/*
  proc.h Part of TRACER

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

#ifndef TRACER_CORE_H
#define TRACER_CORE_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


//-------------------------------------------------------- Stack -------------------------------------------------------

/**
 * The proc stack provides the two pointers that define a descending stack;
 */

struct proc_stack {

	//The lowest bound of the stack pointer, also used to free the stack, constant;
	void *const stack_limit;

	//The current stack pointer, mutable;
	void *sp;

	//The highest value the stack pointer can take, constant;
	void *const sp_reset;

};


//--------------------------------------------------- Stack creation ---------------------------------------------------

//Determine the closest inferior address, that would respect alignment requirements;
extern void *proc_stack_align(void *stack_reset);

//Initialise the stack for initialisation. Implemented by the run lib;
void proc_init_stack(struct proc_stack *stack, void (*function)(), void (*end_loop)(), void *init_arg);

//A module can register a special function that adds a stack context header, for an FPU for ex;
bool register_stack_header_creator(void (*new_creator)(struct proc_stack *));

//Reset a previously registered stack header creator;
void reset_stack_header_creator();


//-------------------------------------------------- Execution control -------------------------------------------------

//Stars the execution of the first process;
void proc_start_execution();

//Get the initial arg;
extern void *proc_get_init_arg();



#endif //TRACER_CORE_H
