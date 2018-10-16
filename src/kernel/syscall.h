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


#include <stddef.h>

//Call the kernel;
uint32_t __syscall_trigger(uint32_t syscall_id, uint32_t arg0, uint32_t arg1, uint32_t arg2);


#define kernel_syscall(a, b, c, d) __syscall_trigger(a, b, c, d);


#endif //TRACER_KERNEL_H
