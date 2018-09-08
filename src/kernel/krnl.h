/*
  krnl.h Part of TRACER

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

#ifndef TRACER_KRNL_H
#define TRACER_KRNL_H

#include <stdint.h>

#include <stddef.h>


//----------------------------------------------------- Kernel Heap ----------------------------------------------------

//receive a new thread stack, providing the core number and the current thread stack pointer;
void *kernel_switch_thread_stack(volatile uint8_t thread_id, void *volatile sp);


//----------------------------------------------------- Kernel Heap ----------------------------------------------------

//Allocate some data in the kernel heap;
void *kmalloc(size_t);

//Allocate and zero-initialise some data in the kernel heap;
void *kcalloc(size_t);

//Allocate and initialise some data in the kernel heap and initialise them;;
void *kialloc(size_t, const void *);

//Free some data in the kernel heap;
void kfree(void *);





#endif //TRACER_KRNL_H
