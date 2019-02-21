/*
  syscl.h Part of TRACER

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
 * This file contains declarations for all khal dependant syscall functions;
 */

#ifndef TRACER_SYSCL_H
#define TRACER_SYSCL_H

#include <stdint.h>

#include <stddef.h>

//Set the priority of the syscall exception and enable it;
extern void __syscl_configure(uint8_t priority);

//Call the kernel;
extern size_t __syscl_trigger(size_t arg0, size_t arg1, size_t arg2, size_t syscall_id);


#endif //TRACER_SYSCL_H
