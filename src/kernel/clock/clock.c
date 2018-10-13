/*
  clock.c Part of TRACER

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

#include <kernel/struct/nlist.h>
#include <kernel/panic.h>
#include <kernel/debug/log.h>

#include "clock.h"


//-------------------------------------------------- Static variables --------------------------------------------------

//The clock tuner, receives a config struct and its size;
static void (*clock_tuner)(void *config, size_t);

//The clock tuner, receives a config struct and its size;
static void (*clock_auto_tuner)(uint32_t);


/*
 * The clocks list;
 */

struct nlist clocks_list = {
	.name_max_length = 20,
	.elements = 0,
};


//---------------------------------------------- Clock environment tuning ----------------------------------------------

/*
 * Update the clock tuning environment;
 * 	tuner : the clock tuner, receives a config struct and its size;
 * 	auto_tuner : the clock auto_tuner tuner, receives a frequency and finds a config;
 */

void clock_register_tuner(void (*new_tuner)(void *, size_t)) {
	
	//If the clock environment is not reset :
	if (clock_tuner) {
		kernel_panic("clock_register_tuners : tuning environment is not reset");
	}
	
	//Update the clocking environment;
	clock_tuner = new_tuner;
	
}

void clock_register_auto_tuner(void (*new_auto_tuner)(uint32_t)) {
	
	//If the clock environment is not reset :
	if (clock_auto_tuner) {
		kernel_panic("clock_register_tuners : tuning environment is not reset");
	}
	
	//Update the clocking environment;
	clock_auto_tuner = new_auto_tuner;
	
}


/*
 * Reset the auto tuner to 0;
 */

void clock_release_tuner() {
	
	//Update the clocking environment;
	clock_tuner = 0;
	
}

/*
 * Reset the auto tuner to 0;
 */

void clock_release_auto_tuner() {
	
	//Update the clocking environment;
	clock_auto_tuner = 0;
	
}


//---------------------------------------------------- Clock Tuning ----------------------------------------------------

//Tune the clock, providing a config struct and its size;
void clock_tune(void *config, size_t size) {
	
	//Cache the tuner;
	void (*tuner)(void *, size_t) = clock_tuner;
	
	//If the tuner is not null :
	if (tuner) {
		
		//Call the tuner;
		(*tuner)(config, size);
		
	} else {
		
		//If no tuner is registered, log;
		kernel_log_("clock_tune : no tuner is registered;");
	}
	
}

//Find automatically the closest configuration for the clock;
void clock_auto_tune(uint32_t target_frequency) {
	
	//Cache the tuner;
	void (*auto_tuner)(uint32_t) = clock_auto_tuner;
	
	//If the tuner is not null :
	if (auto_tuner) {
		
		//Call the tuner;
		(*auto_tuner)(target_frequency);
		
	} else {
		
		//If no tuner is registered, log;
		kernel_log_("clock_auto_tune : no auto_tuner is registered;");
	}
	
}


//-------------------------------------------------- Clock Reference ---------------------------------------------------

//Register a clock in the clock reference;
bool clock_register(const char *const name, const uint32_t value) {
	
	//Add the clock;
	return nlist_add(&clocks_list, name, (void *) value);
	
}


//Update the value of a clock in the clock reference
void clock_set(const char *name, uint32_t value) {
	
	//Set the clock;
	nlist_set(&clocks_list, name, (void *) value);
	
}


//Update the value of a clock in the clock reference
uint32_t clock_get(const char *name) {
	
	//Get the clock frequency;
	return (uint32_t) nlist_get(&clocks_list, name);
	
}


//Remove a clock in the clock reference;
void clock_remove(const char *name) {
	
	//Remove the clock;
	nlist_remove(&clocks_list, name);
	
}

