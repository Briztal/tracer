/*
  csyscall.h Part of TRACER

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

#include "dmz/prc.h"

#define NB_SYSCALLS 7

#define SYS_OPEN 	0
#define SYS_CLOSE 	1
#define SYS_READ 	2
#define SYS_WRITE 	3
#define SYS_IFACE 	4
#define SYS_EXEC 	5
#define SYS_EXIT 	6



/*
 * File operations
 */

size_t sys_open(const char *pathname);

size_t sys_close(size_t fd);

size_t sys_read(size_t fd, void *buffer, size_t dst);

size_t sys_write(size_t fd, const void *buffer, size_t size);

size_t sys_interface(size_t fd, void *if_struct, size_t struct_size);


/*
 * Process operations
 */

void sys_exec(struct prc_desc *desc, struct prc_req *req);

void sys_exit();


#endif //TRACER_KERNEL_H
