/*
  syscall.h Part of TRACER

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

#ifndef TRACER_KERNEL_H
#define TRACER_KERNEL_H

#include <stdint.h>

#include <stddef.h>

#include <kernel/arch/arch.h>



//------------------------------------------- Heap management -------------------------------------------



//------------------------------------------- Heap management -------------------------------------------

//The malloc function to use across the code;
void *syscall_malloc(size_t size);

//A shortcut to allocate and copy from a memory zone;
void *syscall_ialloc(size_t size, const void *initialiser);

//The realloc function to use across the code;
void *syscall_realloc(void *, size_t size);

//The free function to use across the code;
void syscall_free(void *);


#endif //TRACER_KERNEL_H
