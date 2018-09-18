/*
  auto_mod.h Part of TRACER

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

#ifndef TRACER_AUTO_MOD_H
#define TRACER_AUTO_MOD_H

#include <stdbool.h>

#include <util/type.h>


/*
 * There are different types of modules. Some may be loaded automatically by the kernel when it completed its
 * 	initialisation. This concerns for example hardware drivers;
 *
 * 	These modules will be stored in flash at a particular section. Macros defined below accomplish that procedure;
 */

struct auto_mod {

	//The module's name;
	const char *const name;

	//The module's initialisation function;
	bool (*const init)();


};



#define _KERNEL_EMBED_MODULE(name_l, section_name, init_f)\
	static struct auto_mod mod##name_l __attribute__((section (section_name), used)) = \
	{.name = (#name_l), .init = (init_f)};

/*
 * There are distinct type of modules, that rely on different pre-initialisations :
 * 	- Peripheral modules, that only rely on base kernel initialisation;
 * 	- System modules, TODO;
 * 	- User modules that can rely on peripheral modules and system modules;
 */

#define KERNEL_EMBED_PERIPHERAL_MODULE(name, init)\
	_KERNEL_EMBED_MODULE(name, ".kernel_peripheral_modules", init)


#define KERNEL_EMBED_SYSTEM_MODULE(name, init)\
	_KERNEL_EMBED_MODULE(name, ".kernel_system_modules", init)


#define KERNEL_EMBED_USER_MODULE(name, init)\
	_KERNEL_EMBED_MODULE(name, ".kernel_user_modules", init)



#endif //TRACER_AUTO_MOD_H
