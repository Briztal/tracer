/*
  run.c Part of TRACER

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

#include <stddef.h>
#include "syscall.h"

void *__run_get_init_arg();


/*
 * prc_exec : the process execution function;
 *
 * 	Executes the process function, with the process args, and returns;
 */


void run_exec() {
	
	//Cache the execution data, saved in the stack;
	volatile struct prc_desc *volatile desc = __run_get_init_arg();
	
	//Execute the function, passing args;
	(*(desc->function))((void *) desc->args, (size_t) desc->args_size);
	
	
	sys_exit();
	
}


/**
 * prc_exit : the process exit function. Called automatically when prc_entry returns;
 *
 * 	Marks the process terminated and triggers the preemption;
 *
 * 	If preemption fails, an internal error has occurred, and a kernel panic is generated;
 */

void run_exit() {
	
	while (1) {}
	
}

