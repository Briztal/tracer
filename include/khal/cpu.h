/*
  cpu.h Part of TRACER

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

/*
 * This file contains declarations for all arch dependant arch related functions;
 */

#ifndef TRACER_CPU_H
#define TRACER_CPU_H

/*Determine the closest inferior address, that would respect alignment requirements;*/
/*TODO NO, JUST PROVIDE ALIGNMENT MAGNITUDE*/
extern void *__cpu_stack_align(void *stack_reset);

/*Get the initial arg;*/
extern void *__cpu_get_init_arg();

/*The hardware library must provide a function to create the general stack context;*/
extern void * __cpu_create_stack_context(void *sp_reset, void (*function)(), void (*exit_loop)(), void *arg);

/**
 * __cpu_enter_thread_mode : this function initialises threads in a safe state. It never returns.
 * 	The preemption environment must have been initialised before;
 *
 * 	- initialises all threads in their lowest privilege state;
 * 	- updates exception stacks;
 * 	- calls the provided function, that should trigger the preemption;
 * 	- make all threads run an infinite loop;
 * 	- execute the preemption call;
 * 	- enable interrupts;
 * 	- loop;
 *
 * @param exception_stacks : processor stacks that must be used in case of interrupt;
 */

extern void __cpu_enter_thread_mode(void *exception_sp);


#endif /*TRACER_CPU_H*/
