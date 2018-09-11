/*
  mod.c Part of TRACER

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

#include "auto_mod.h"

#include <kernel/struct/nlist.h>

#include <kernel/proc.h>
#include <kernel/panic.h>
#include <kernel/debug/debug.h>
#include <kernel/log.h>


//--------------------------------------------------- Modules globals --------------------------------------------------

#define MOD_NAME_MAX_LENGTH 32

static struct nlist modules = {
	.elements = 0,
	.name_max_length = MOD_NAME_MAX_LENGTH,
};


//------------------------------------------------- Modules management -------------------------------------------------

/**
 * mod_add : creates the module struct, copies the name, adds it to the module list, and executes @init;
 *
 * @param name : the module's name; will be copied;
 * @param init : the module's init function; will be executed;
 * @param exit : the module's deinit function; will be executed when the module will be removed;
 */

void mod_add(const char *name, void (*init)(), void (*exit)) {

	//Add the exit function to the list;
	bool added = nlist_add(&modules, name, exit);

	//If the add succeeded :
	if (added) {

		//Call the initialisation function;
		(*init)();

	}

}


/**
 * mod_remove : serches for the required module, and if it exists, removes it and executes its exit function;
 *
 * @param name : the name to search for;
 */

void mod_remove(const char *const name) {

	//Remove the module from the list, and cache its exit function;
	bool (*exit)() = nlist_remove(&modules, name);

	//If the module was found and removed, and has a valid exit function :
	if (exit) {

		//Execute it;
		bool cleanup = (*exit)();

		//If the module didn't manage to clean all its resources :
		if (!cleanup) {

			//Reinsert the module, as it is still active;
			nlist_add(&modules, name, exit);

			//TODO LOG;

		}

	}


}


static void load_modules(const uint8_t *const mod_start, const uint8_t *const mod_end) {

	//Determine the byte length of the module array;
	const size_t module_array_size = mod_end - mod_start;

	//The module array should contain only auto_mod structs. If size validity check fails :
	if (module_array_size % sizeof(struct auto_mod)) {

		//Kernel panic, invalid module array size, probably caused by poor linking;
		kernel_panic("mod.c : mod_autoload : embedded modules array bounds invalid;");

	}

	//Cache the array to the right type;
	const struct auto_mod *auto_module = (const struct auto_mod *) mod_start;

	//If no modules are to load :
	if ((size_t) auto_module >= (size_t) mod_end) {

		//Log;
		kernel_log_("\tNo modules to load");

	}

		//For each module :
	while ((size_t) auto_module < (size_t) mod_end) {

		//Load the module;
		mod_add(auto_module->name, auto_module->init, auto_module->exit);

		//Log;
		kernel_log("\t%s module loaded", auto_module->name);

		//Focus on the next module;
		auto_module++;

	}

}


/**
 * mod_autoload : loads all modules placed in the embedded modules area;
 *
 * 	This section is defined in the unified link script and resides in flash;
 */

void mod_autoload() {

	//The start and end address of kernel peripheral, system and user modules structs in FLASH;
	extern const uint8_t _pmod_start, _pmod_end,  _smod_start, _smod_end, _umod_start, _umod_end;

	//Log;
	kernel_log_("detecting peripheral modules ...");

	//Load peripheral modules;
	load_modules(&_pmod_start, &_pmod_end);

	//Log;
	kernel_log_("detecting system modules ...");

	//Load system modules;
	load_modules(&_smod_start, &_smod_end);

	//Log;
	kernel_log_("detecting user modules ...");

	//Load user modules;
	load_modules(&_umod_start, &_umod_end);

}

