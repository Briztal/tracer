/*
  driver.h Part of TRACER

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

#ifndef TRACER_DRIVER_H
#define TRACER_DRIVER_H

#include <stdbool.h>


struct kernel_driver {


	//Initialised : set at the beginning of init, reset at the beginning of prempt;
	bool initialised;

	/**
	 * init :
	 * 	- initialises the peripheral hardware;
	 * 	- creates software interfaces;
	 * 	- registers software interfaces;
	 *
	 * @param driver : the driver to initialise;
	 * @param config : data required to initialise the driver properly;
	 * @return true if the initialisation is a success;
	 */

	bool (*const init)(struct kernel_driver *driver, void *config);


	/**
	 * exit :
	 * 	- un-registers software interfaces;
	 * 	- deletes software interfaces;
	 * 	- reset the peripheral hardware;
	 *
	 * @param driver : the driver to de-initialise;
	 * @return true is the de-initialisation is a success;
	 */

	bool (*const exit)(struct kernel_driver *driver);


	/**
	 * delete :
	 * 	- exits if initialised;
	 * 	- deletes all dynamic data;
	 *
	 * @param driver : the driver to delete;
	 */

	void (*const delete)(struct kernel_driver *driver);

};


/**
 * driver_init : shortcut for driver initialisation. Checks that the driver is not already initialised;
 * @param driver : the driver to initialise;
 * @param config : the config to transmit to the initialisation function;
 */

static inline void driver_init(struct kernel_driver *driver, void *config) {

	//If the driver is already initialised, nothing to do;
	if (driver->initialised) {
		return;
	}

	//Initialise the driver, set the init flag if success;
	driver->initialised = (*(driver->init))(driver, config);

}


/**
 * driver_exit : shortcut for driver de-initialisation. Checks that the driver is initialised;
 * @param driver : the driver to initialise;
 */

static inline void driver_exit(struct kernel_driver *driver) {

	if (!driver->initialised) {
		return;
	}

	//De-initialise the driver, reset the init flag if success;
	driver->initialised = !(*(driver->exit))(driver);


}


#endif //TRACER_DRIVER_H
