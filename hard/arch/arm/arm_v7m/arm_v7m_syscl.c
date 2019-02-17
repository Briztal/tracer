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


#include "arm_v7m.h"

//Set the priority of the syscall exception and enable it;
void __syscl_configure(uint8_t priority) {
    armv7m_set_svcall_priority(priority);
}


/**
 * syscl_handler : this function handles the SVC exception. It performs the following operations :
 * 	- preparing syscall arguments, namely syscall_id, arg0, arg1 and arg2 for the kernel syscall handler;
 * 	- calling kernel_syscall_handler;
 * 	- Modifying the stacked version of R0 so that the kernel syscall handler's return value
 * 		arrives to __syscall_trigger;
 */

__attribute__ ((naked)) static void syscl_handler() {
    __asm__ __volatile__ (""

        "push 	{lr} 	\n\r"

        //R0-R3 have not been altered. We can directly call the kernel syscall handler;
        "bl 	__krnl_syscall_handler\n\r"

        "pop 	{lr} 	\n\r"

        //Cache PSP in R5;
        "mrs	r1, 	psp	\n\r"

        //Save R0 in the psp; The psp points to the stacked version of R0;
        "str	r0, 	[r1]\n\r"

        //Exit from exception;
        "bx 	lr			\n\r"

    );
}
