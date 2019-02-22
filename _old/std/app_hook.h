/*
  auto_app.h Part of TRACER

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


#ifndef TRACER_AUTO_APP_H
#define TRACER_AUTO_APP_H

#include <stdbool.h>
#include <kernel/exec/prc.h>

/*
 * There are different types of modules. Some may be loaded automatically by the kernel when it completed its
 * 	initialisation. This concerns for example hardware drivers;
 *
 * 	These modules will be stored in flash at a particular section. Macros defined below accomplish that procedure;
 */

struct app_hook {
	
	//The application name;
	const char *const name;
	
	//The application's first process's hardware requirements;
	struct prc_req *req;
	
	//The application's first process's description;;
	struct prc_desc *desc;
	
};


#define APP_ST(x) #x


#define HOOK_APP(_name, _req, _desc)\
	static struct app_hook _name __attribute__((section (".kernel_apps"), used)) = \
	{.name = APP_ST(_name), .req = (_req), .desc = (_desc)};


#endif //TRACER_AUTO_APP_H
