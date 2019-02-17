/*
  ksyscall.c Part of TRACER

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

#include "panic.h"

#include "kernel/hard/impl_defined/hard_defined.h"

#include <std/syscall.h>

#include <kernel/exec/sched.h>
#include <kernel/debug/printk.h>
#include <kernel/debug/debug.h>


/**
 * sysh_x : handles the appropriate syscall;
 */


static size_t sysh_open(const char *pathname) {
	
	return 0;//TODO
}

size_t sysh_close(size_t fd) {
	
	return 0;//TODO
	
}


size_t sysh_read(size_t fd, void *buffer, size_t size) {
	
	return 0;//TODO
	
}


size_t sysh_write(size_t fd, const void *buffer, size_t size){
	
	printk("WRITE\n\r");
	
	debug_delay_ms(1000);
	
	return 0;
	
}


size_t sysh_interface(size_t fd, void *if_struct, size_t struct_size) {
	
	return 0;//TODO
	
	
}


/*
 * Process operations
 */

void sysh_exec(struct prc_desc *desc, struct prc_req *req) {
	
	sched_create_prc(desc, req);
}


void sysh_exit() {
	
	//Mark the process terminated;
	sched_terminate_prc();
	
	//Require a context switch, process will be terminated;
	__prmpt_trigger();
	
}



#define SCF_CAST (size_t (*)(size_t arg0, size_t arg1, size_t arg2))


static size_t(*syscall_table[NB_SYSCALLS])(size_t a , size_t b, size_t c) = {
	SCF_CAST &sysh_open,
	SCF_CAST &sysh_close,
	SCF_CAST &sysh_read,
	SCF_CAST &sysh_write,
	SCF_CAST &sysh_interface,
	SCF_CAST &sysh_exec,
	SCF_CAST &sysh_exit,
};


extern size_t __krnl_syscall_handler(size_t arg0, size_t arg1, size_t arg2, size_t syscall_id) {
	
	//If the syscall index is invalid :
	if (syscall_id >= NB_SYSCALLS) {
		
		//Panic, should not happen;
		kernel_panic("syscall_handler : invalid syscall index");
	
	}
	
	//Execute the syscall function and transmit its return value;
	return (*(syscall_table[syscall_id]))(arg0, arg1, arg2);
	
}




