//
// Created by root on 8/9/18.
//


#ifndef TRACER_DRIVER_H
#define TRACER_DRIVER_H

#include <stdbool.h>


struct kernel_driver {

	//Initialised : set at the beginning of init, reset at the beginning of exit;
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
