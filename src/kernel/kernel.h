/*
  Kernel.h Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

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

#ifndef TRACER_KERNEL_H
#define TRACER_KERNEL_H

#include <stdint.h>

#include <stddef.h>

#include <kernel/arch/arch.h>

//------------------------------------------- Entry Point -------------------------------------------

/*
 * start : this function is called once, by main only. It is the project's entry point.
 *
 *  It will call initialise_hardware, and then call iterate indefinitely.
 */

void kernel_start();

//Raise an error, halt the code, log;
void kernel_halt(uint16_t blink_delay);

//Raise an error, halt the code, log;
void kernel_error(const char *log_message);


//------------------------------------------- Stack management -------------------------------------------

//Allocate a stack for a process;
void kernel_stack_alloc(stack_t *stack, size_t stack_size, 
	void (*function)(), void (*exit_loop)(), void *arg);



//Reset an allocated stack;
void kernel_stack_reset(stack_t *stack, void (*init_f)(), void (*exit_f)(), void *arg);

//Free an allocated stack;
void kernel_stack_free(stack_t *);


//------------------------------------------- Heap management -------------------------------------------

//The malloc function to use across the code;
void *kernel_malloc(size_t size);

//A shortcut to allocate and copy from a memory zone;
void *kernel_malloc_copy(size_t size, const void *initialiser);

//The realloc function to use across the code;
void *kernel_realloc(void *, size_t size);

//The free function to use across the code;
void kernel_free(void *);

//------------------------------------------- Entry Point -------------------------------------------

//Enter a critical section;
void kernel_enter_critical_section();

//Leave a critical section;
void kernel_leave_critical_section();


#endif //TRACER_KERNEL_H
