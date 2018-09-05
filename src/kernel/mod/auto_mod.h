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

	//The module's initialisation function;
	void (*const init)();

	//The module's exit function; Must free any resource owned by the module;
	void (*const exit)();

	//The module's name; has
	const char *const name;

};

#define KERNEL_EMBED_MODULE(name_litteral, init_function, exit_function)\
	static struct auto_mod mod __attribute__((section("embedded_modules"), used)) = \
	{.init = (init_function), .exit = (exit_function), .name = (name_litteral)};



#endif //TRACER_AUTO_MOD_H
