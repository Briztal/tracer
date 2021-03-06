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
#include <kernel/exec/stck.h>


/*--------------------------------------------------- Stack creation ---------------------------------------------------*/

/*Initialise the stack_data for initialisation. Implemented by the run lib;*/
void proc_init_stack(struct stck *stack, void (*function)(), void (*end_loop)(), void *init_arg);

/*-------------------------------------------------- Execution control -------------------------------------------------*/

/*Stars the execution of the first process;*/
void proc_start_execution();




#endif /*TRACER_CORE_H*/
