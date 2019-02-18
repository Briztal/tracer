/*
  arm_v7m_syscl.c Part of TRACER

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


/**
 * __syscall_trigger : triggers the syscall exception;
 *
 * 	This function is pure assembly, and follows the ARM Calling Convention;
 *
 * @param syscall_id : the identifier of the system call to execute. Will be located on R0 at handler entry;
 */

__attribute__ ((naked)) size_t __syscl_trigger(size_t arg0, size_t arg1, size_t arg2, size_t syscall_id) {
	
	__asm__ __volatile (
	
		//Trigger the supervisor call; Arguments are already placed in R0, R1, R2 and R3;
		"SVC #0\n\t"
		
		//The supervisor call handler will have placed the return value in R0. We can return now;
		"bx lr\n\t"
	);
	
}


/*
 * __run_init_arg : this function will return the value of r12. If called by the process init function,
 *  it will be equal to the previous function's arg;
 */

__attribute__ ((naked)) void *__run_get_init_arg() {
	__asm__ __volatile__(\
        "mov 	r0, 	r12 	\n\t"
		"bx	 	lr 				\n\t"
	);
}
