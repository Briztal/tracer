/*
  panic.h Part of TRACER

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

#ifndef TRACER_ERROR_H
#define TRACER_ERROR_H

#include <stddef.h>

/*
 * The kernel manic library offers a log system for debug purposes, and a trigger for kernel panics.
 *
 * 	A kernel panic will halt any current code, put the kernel in safe mode, and display the log stack in the debug
 * 	interface;
 *
 * 	The log stack is static, and does not require any initialisation, so that it can be used in case of error, and
 * 	without dynamic memory;
 */

#define KERNEL_LOG_STACK_SIZE 10


//------------------------------------------------------ Log stack -----------------------------------------------------

//Push a log in the panic log stack;
void panic_push_log(const char *log);

//Pop a log from the panic log stack;
void panic_pop_log();

//Reset the log stack;
void panic_reset_log();

//Get the log stack and its size for debug purposes;
const char **panic_get_log_stack(size_t *stack_size);

#endif //TRACER_ERROR_H
