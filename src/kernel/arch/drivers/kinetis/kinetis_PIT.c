//
// Created by root on 6/15/18.
//


#include "kinetis_PIT.h"

#include <kernel/kernel.h>

#include <kernel/drivers/timer.h>

#include <kernel/debug.h>


//--------------------------------------------------- Driver structs ---------------------------------------------------

/*
 * The kinetis Periodic Interrupt Timer memory struct; Maps the memory structure of a hardware PIT;
 */

struct __attribute__ ((packed)) kinetis_PIT_registers {

	//The Load Value;
	volatile uint32_t LDVAL;

	//Current Value;
	volatile uint32_t CVAL;

	//Timer Control Register
	volatile uint32_t TCTRL;

	//Timer Flag;
	volatile uint32_t TFLG;

};



//TODO BASE FREQUENCY;

//--------------------------------------------------- Driver methods ---------------------------------------------------


//Initialise the driver, providing the configuration;
void kinetis_PIT_init(struct kinetis_PIT_driver *driver, struct kinetis_PIT_config *config);

//De-initialise the driver. Timer interface will be deleted;
void kinetis_PIT_exit(struct kinetis_PIT_driver *driver);

//Delete a kinetis PIT driver;
void kinetis_PIT_delete(struct kinetis_PIT_driver *driver);



//--------------------------------------------------- Timer interface --------------------------------------------------

/*
 * The kinetis PIT timer interface is composed of a timer interface and registers for timer manipulation;
 */

struct kinetis_PIT_timer_interface {

	//The timer interface;
	struct timer_interface iface;

	//The registers zone reference;
	struct kinetis_PIT_registers *const registers;



	//The ratio tics/period;
	const float period_to_tics;

	//The reload interrupt channel;
	const uint8_t reload_int_channel;

};

//Delete the kinetis PIT timer interface;
static void kinetis_PIT_timer_interface_delete(struct kinetis_PIT_timer_interface *iface);


//------------------------------------------- PITs timer interface functions -------------------------------------------

//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!
//TODO MAKE A HANDLER!!!!!!!!!!!

static void kinetis_PIT_set_base_frequency(uint32_t frequency) {

	//Determine the tics/period ratio;
	float period_to_tics = ((float) driver->hw_specs->clock_frequency) / ((float) config->base_frequency);

	//Determine the maximal period;
	float max_period = ((float) (uint32_t) -1) / period_to_tics;
}

//Enable the timer;
static void kinetis_PIT_enable(struct kinetis_PIT_timer_interface *data) {
	//Set bit 0 of TCTRL;
	data->registers->TCTRL |= PIT_TCTRL_TEN;
}

//Disable the timer;
static void kinetis_PIT_disable(struct kinetis_PIT_timer_interface *data) {
	//Clear bit 0 of TCTRL;
	data->registers->TCTRL &= ~PIT_TCTRL_TEN;
}

//Is the timer enabled ?
static bool kinetis_PIT_enabled(struct kinetis_PIT_timer_interface *data) {
	//Is bit 0 of TCTRL set ?
	return (bool) (data->registers->TCTRL & PIT_TCTRL_TEN);
}


//Update the reload value;
static void kinetis_PIT_set_count(struct kinetis_PIT_timer_interface *data, float count) {
	data->registers->CVAL = (uint32_t) (data->period_to_tics * count);
}

//Get the current value;
static float kinetis_PIT_get_count(struct kinetis_PIT_timer_interface *data) {
	return (((float) data->registers->CVAL) / data->period_to_tics);
}


//Update the reload value;
static void kinetis_PIT_set_reload(struct kinetis_PIT_timer_interface *data, float count) {
	data->registers->LDVAL = (uint32_t) (data->period_to_tics * count);
}

//Get the reload value;
static float kinetis_PIT_get_reload(struct kinetis_PIT_timer_interface *data) {
	return (((float) data->registers->LDVAL) / data->period_to_tics);
}


//Enable the interrupt;
static void kinetis_PIT_enable_int(struct kinetis_PIT_timer_interface *data) {
	//Set bit 1 of TCTRL;
	data->registers->TCTRL |= PIT_TCTRL_TIE;

}

//Disable the interrupt;
static void kinetis_PIT_disable_int(struct kinetis_PIT_timer_interface *data) {
	//Clear bit 1 of TCTRL;
	data->registers->TCTRL &= ~PIT_TCTRL_TIE;
}

//Is the interrupt enabled ?
static bool kinetis_PIT_int_enabled(struct kinetis_PIT_timer_interface *data) {
	//Is bit 1 of TCTRL set ?
	return (bool) (data->registers->TCTRL & PIT_TCTRL_TIE);
}


//Update the handler;
void kinetis_PIT_update_handler(struct kinetis_PIT_timer_interface *data, void (*handler)()) {

	//Cache the interrupt channel;
	uint8_t interrupt_channel = data->reload_int_channel;

	//If the handler is not null :
	if (handler) {

		//Update the handler;
		core_IC_set_handler(interrupt_channel, handler);

		//Enable the interrupt;
		core_IC_enable(interrupt_channel);

	} else {

		//If the handler is null :

		//Disable the interrupt;
		core_IC_disable(interrupt_channel);

		//Reset the default handler;
		core_IC_set_default_handler(interrupt_channel);

	}

}


//Is the flag set ?
static bool kinetis_PIT_flag_set(struct kinetis_PIT_timer_interface *data) {
	//Is bit 0 of TFLG set ?
	return (bool) (data->registers->TFLG);
}


//Clear the interrupt flag;
static void kinetis_PIT_clear_flag(struct kinetis_PIT_timer_interface *data) {
	//Set bit 0 of TFLG;
	data->registers->TFLG = 1;
}


/**
 * kinetis_PIT_timer_interface_delete : deletes the struct.
 *
 * The timer interface doesn't store any dynamic data, Only the struct must be deleted;
 *
 * @param iface : the timer interface to destruct;
 */

static void kinetis_PIT_timer_interface_delete(struct kinetis_PIT_timer_interface *iface) {
	kernel_free(iface);
}







//--------------------------------------------- Driver Creation - Deletion ---------------------------------------------

/**
 * kinetis_PIT_create : creates a driver to interact with the kinetis PIT timer;
 *
 * @param specs : the set of hardware specifications;
 *
 * @return the initialised driver;
 */

struct kernel_driver *kinetis_PIT_create(struct kinetis_PIT_specs *specs) {

	//Cache the number of PITs;
	uint8_t nb_PITs = specs->nb_PITs;


	//Duplicate the interrupt channels array;
	uint8_t *const int_channels = kernel_malloc_copy(nb_PITs * sizeof(uint8_t), specs->int_channels);


	//Allocate and initialise the interfaces array;
	struct timer_interface ** ifaces = kernel_malloc(sizeof(struct timer_interface *));

	//Reset all pointers;
	for (uint8_t PIT_id = nb_PITs; PIT_id--;) {ifaces[PIT_id] = 0;}


	//Create the driver initializer;
	struct kinetis_PIT_driver driver_init = {

		//Transmit driver functions;
		.driver = {
			.init = (void (*)(struct kernel_driver *, void *)) kinetis_PIT_init,
			.exit = (void (*)(struct kernel_driver *)) kinetis_PIT_exit,
			.delete = (void (*)(struct kernel_driver *)) kinetis_PIT_delete,
		},

		//Transmit the ownership of the specs struct;
		.hw_specs = {
			.MCR = specs->MCR,
			.nb_PITs = specs->nb_PITs,
			.first_area = specs->first_area,
			.spacing = specs->spacing,
			.int_channels = int_channels,
			.clock_frequency = specs->clock_frequency,
		},

		//The interface will be initialised at startup;
		.ifaces = ifaces,

	};

	//Allocate, initialise and return the driver;
	return kernel_malloc_copy(sizeof(struct kinetis_PIT_driver), &driver_init);

}

/**
 * kinetis_PIT_delete : calls exit, and delete the driver struct;
 *
 * @param driver : the driver to delete;
 */
void kinetis_PIT_delete(struct kinetis_PIT_driver *const driver) {

	//Eventually stop the driver;
	kinetis_PIT_exit(driver);

	//free the driver:
	kernel_free(driver);

}


//---------------------------------------------------- Start - Stop ----------------------------------------------------

//Start the driver, providing the configuration;
void kinetis_PIT_start(struct kinetis_PIT_driver *driver, void *unused) {

	//If the driver is already started :
	if (driver->ifaces) {

		//Error, miss-use;
		kernel_error("kinetis_PIT.c : kinetis_PIT_start : attempted to start the dirver twice");

	}

	//Create the timer interface initializer;
	struct timer_interface timer_init = {

		.start = (void (*)(struct timer_interface *)) &kinetis_PIT_enable,
		.stop = (void (*)(struct timer_interface *)) &kinetis_PIT_disable,
		.started = (bool (*)(struct timer_interface *)) &kinetis_PIT_enabled,

		.set_count = (void (*)(struct timer_interface *, float)) &kinetis_PIT_set_count,
		.get_count = (float (*)(struct timer_interface *)) &kinetis_PIT_get_count,

		.set_ovf_value = (void (*)(struct timer_interface *, float)) &kinetis_PIT_set_reload,
		.get_ovf_value = (float (*)(struct timer_interface *)) &kinetis_PIT_get_reload,

		.enable_int = (void (*)(struct timer_interface *)) &kinetis_PIT_enable_int,
		.disable_int = (void (*)(struct timer_interface *)) &kinetis_PIT_disable_int,
		.int_enabled = (bool (*)(struct timer_interface *)) &kinetis_PIT_int_enabled,
		.update_handler = (void (*)(struct timer_interface *, void (*)())) &kinetis_PIT_update_handler,

		.int_flag_set = (bool (*)(struct timer_interface *)) &kinetis_PIT_flag_set,
		.clr_int_flag = (void (*)(struct timer_interface *)) &kinetis_PIT_clear_flag,

		.deleter = (void (*)(const struct timer_interface *)) kinetis_PIT_timer_interface_delete,

	};

	//Cache the specs address;
	const struct kinetis_PIT_specs *const specs = &driver->hw_specs;

	//Initialise the clocking;
	*(specs->MCR) = 0;


	//Cache the first registers area address;
	volatile uint8_t *const first_registers_area = specs->first_area;

	//Cache the register area spacing;
	const size_t area_spacing = specs->spacing;


	/*
	 * Initialise all timers;
	 */

	//For each PIT instance :
	for (uint8_t PIT_id = specs->nb_PITs; PIT_id--;) {

		//Determine the register area address;
		volatile void *registers_area = first_registers_area + area_spacing * PIT_id;

		//Create the PIT timer interface initializer;
		struct kinetis_PIT_timer_interface iface_init = {

			//Transmit function pointers;
			.iface = timer_init,

			//Transmit the reference of the registers zone;
			.registers = registers_area,

			//Copy the reload interrupt chanel;
			.reload_int_channel = (specs->int_channels)[PIT_id],

		};


		//Allocate, initialise and cache the timer interface;
		struct kinetis_PIT_timer_interface *iface =
			kernel_malloc_copy(sizeof(struct kinetis_PIT_timer_interface), &iface_init);



		/*
		 * Initialise the PIT;
		 */

		//1KHz base frequency;
		const uint32_t base_frequency = 1000;

		//The period will be of one unit;
		const float period = 1;

		//Set the


		//Reset the count;
		kinetis_PIT_set_count(&iface_init, period);

		//Reset the reload value to a high one;
		kinetis_PIT_set_reload(&iface_init, period);

		//Clear the interrupt flag;
		kinetis_PIT_clear_flag(&iface_init);

		//Set the interrupt priority;
		core_IC_set_priority(channel, DRIVER_STARUS_INTERRUPT_PRIORITY);

		//Set the default handler;
		core_IC_set_default_handler(channel);



		//Register the timer interface;
		driver->ifaces[PIT_id] = (struct timer_interface *) iface;

	}

}


//Stop the driver. Timer interface will be deleted;
void kinetis_PIT_stop(struct kinetis_PIT_driver *driver) {

	//Cache the interface;
	struct kinetis_PIT_timer_interface *iface = (struct kinetis_PIT_timer_interface *) driver->ifaces;

	if (driver->ifaces) {

		/*
		 * Reset the timer;
		 */

		//Disable the interrupt;
		kinetis_PIT_disable_int(iface);

		//Stop the timer;
		kinetis_PIT_disable(iface);

		//Reset the handler and disable the interrupt;
		kinetis_PIT_update_handler(iface, 0);


		//Get the number of tics;
		float period = iface->iface.maximal_period - 1;

		//Reset the count;
		kinetis_PIT_set_count(iface, period);

		//Reset the reload value to a high one;
		kinetis_PIT_set_reload(iface, period);


		//Destruct and delete the timer interface;
		timer_destruct(driver->ifaces);
		kernel_free(driver->ifaces);

		//Deinit the driver;
		driver->ifaces = 0;

	}

}
