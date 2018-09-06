//
// Created by root on 9/4/18.
//

#include "auto_mod.h"

#include <kernel/struct/nlist.h>

#include <core/core.h>


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


//The start address of kernel embedded modules structs in FLASH;
extern uint32_t _emod_start;

//The end address of kernel embedded modules structs in FLASH;
extern uint32_t _emod_end;


/**
 * mod_autoload : loads all modules placed in the embedded modules area;
 *
 * 	This section is defined in the unified link script and resides in flash;
 */

void mod_autoload() {

	//The [&_emod_start : &_emod_end] should contain only auto_mod structs. If size validity check fails :
	if ((&_emod_end - &_emod_start) % sizeof (struct auto_mod)) {

		//Core error;
		core_error("mod.c : mod_autoload : embedded modules array bounds invalid;");

	}

	//Cache the array to the right type;
	struct auto_mod *auto_module = (struct auto_mod *)&_emod_start;

	//For each module :
	while ((void *)auto_module < &_emod_end) {

		//Load the module;
		mod_add(auto_module->name, auto_module->init, auto_module->exit);

		//Focus on the next module;
		auto_module++;

	}

}
