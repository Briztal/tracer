//
// Created by root on 8/9/18.
//

#ifndef TRACER_DRIVER_H
#define TRACER_DRIVER_H


struct kernel_driver {

	/**
	 * init :
	 * 	- initialises the peripheral hardware;
	 * 	- creates software interfaces;
	 * 	- registers software interfaces;
	 *
	 * @param driver : the driver to initialise;
	 * @param config : data required to initialise the driver properly;
	 */

	void (*const init)(struct kernel_driver *driver, void *config);


	/**
	 * exit :
	 * 	- un-registers software interfaces;
	 * 	- deletes software interfaces;
	 * 	- reset the peripheral hardware;
	 *
	 * @param driver : the driver to de-initialise;
	 */

	void (*const exit)(struct kernel_driver *driver);


	/**
	 * delete :
	 * 	- exits if initialised;
	 * 	- deletes all dynamic data;
	 *
	 * @param driver : the driver to delete;
	 */

	void (*const delete)(struct kernel_driver *driver);

};

#endif //TRACER_DRIVER_H
