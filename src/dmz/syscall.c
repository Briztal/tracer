/*
  csyscall.c Part of TRACER

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


#include "syscall.h"


//Call the kernel;
extern size_t __syscl_trigger(size_t arg0, size_t arg1, size_t arg2, size_t syscall_id);

/**
 * sys_x : triggers the appropriate syscall and return appropriate arguments;
 */


size_t sys_open(const char *pathname) {
	
	return __syscl_trigger((size_t) pathname, 0, 0, SYS_OPEN);
	
}

size_t sys_close(size_t fd) {
	
	return __syscl_trigger(fd, 0, 0, SYS_CLOSE);
	
}


size_t sys_read(size_t fd, void *buffer, size_t size) {
	
	return __syscl_trigger(fd, (size_t)buffer, size, SYS_READ);
	
}


size_t sys_write(size_t fd, const void *buffer, size_t size){
	
	return __syscl_trigger(fd, (size_t)buffer, size, SYS_WRITE);
	
}


size_t sys_interface(size_t fd, void *if_struct, size_t struct_size) {
	
	return __syscl_trigger(fd, (size_t)if_struct, struct_size, SYS_IFACE);
	
}


/*
 * Process operations
 */

void sys_exec(struct prc_desc *desc, struct prc_req *req) {
	
	__syscl_trigger((size_t) desc, (size_t) req, 0, SYS_EXEC);
	
}


void sys_exit() {
	
	__syscl_trigger(0, 0, 0, SYS_EXIT);
	
}


