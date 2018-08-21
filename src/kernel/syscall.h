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



//Update the error function;
void kernel_set_error_output(void (*error_log)(const char *));

//Raise an error, halt the code, log;
void kernel_error(const char *error_message);


//------------------------------------------- Stack management -------------------------------------------

//Allocate a stack for a process;
void kernel_stack_alloc(core_stack_t *stack, size_t stack_size,
	void (*function)(), void (*exit_loop)(), void *arg);



//Reset an allocated stack;
void kernel_stack_reset(core_stack_t *stack, void (*init_f)(), void (*exit_f)(), void *arg);

//Free an allocated stack;
void kernel_stack_free(core_stack_t *);


//------------------------------------------- Heap management -------------------------------------------

//The malloc function to use across the code;
void *syscall_malloc(size_t size);

//A shortcut to allocate and copy from a memory zone;
void *syscall_malloc_copy(size_t size, const void *initialiser);

//The realloc function to use across the code;
void *syscall_realloc(void *, size_t size);

//The free function to use across the code;
void syscall_free(void *);


#endif //TRACER_KERNEL_H
