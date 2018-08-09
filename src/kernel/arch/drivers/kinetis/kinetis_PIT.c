//
// Created by root on 6/15/18.
//


#include "kinetis_PIT.h"

#include <kernel/kernel.h>

#include <kernel/if/timer.h>

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
void kinetis_PIT_init(struct kinetis_PIT_driver *driver, void *unused);

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
	float period_to_tics;

	//The maximal period;
	float max_period;

	//The clock frequency (Hz);
	const uint32_t clock_frequency;

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


static void kinetis_PIT_set_base_frequency(struct kinetis_PIT_timer_interface *iface, uint32_t frequency) {

	//Determine the tics/period ratio;
	float period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);

	//Determine the maximal period;
	float max_period = ((float) (uint32_t) -1) / period_to_tics;

	//Save both constants;
	iface->period_to_tics = period_to_tics;
	iface->max_period = max_period;

}

//Enable the timer;
static void kinetis_PIT_enable(struct kinetis_PIT_timer_interface *iface) {
	//Set bit 0 of TCTRL;
	iface->registers->TCTRL |= PIT_TCTRL_TEN;
}

//Disable the timer;
static void kinetis_PIT_disable(struct kinetis_PIT_timer_interface *iface) {
	//Clear bit 0 of TCTRL;
	iface->registers->TCTRL &= ~PIT_TCTRL_TEN;
}

//Is the timer enabled ?
static bool kinetis_PIT_enabled(struct kinetis_PIT_timer_interface *iface) {
	//Is bit 0 of TCTRL set ?
	return (bool) (iface->registers->TCTRL & PIT_TCTRL_TEN);
}


//Update the reload value;
static void kinetis_PIT_set_count(struct kinetis_PIT_timer_interface *iface, float period_count) {
	if (period_count < iface->max_period) {
		iface->registers->CVAL = (uint32_t) (iface->period_to_tics * period_count);
	}
}

//Get the current value;
static float kinetis_PIT_get_count(struct kinetis_PIT_timer_interface *iface) {
	return (((float) iface->registers->CVAL) / iface->period_to_tics);
}


//Update the reload value;
static void kinetis_PIT_set_reload(struct kinetis_PIT_timer_interface *iface, float period_count) {
	if (period_count < iface->max_period) {
		iface->registers->LDVAL = (uint32_t) (iface->period_to_tics * period_count);
	}
}

//Get the reload value;
static float kinetis_PIT_get_reload(struct kinetis_PIT_timer_interface *iface) {
	return (((float) iface->registers->LDVAL) / iface->period_to_tics);
}


//Enable the interrupt;
static void kinetis_PIT_enable_int(struct kinetis_PIT_timer_interface *iface) {
	//Set bit 1 of TCTRL;
	iface->registers->TCTRL |= PIT_TCTRL_TIE;

}

//Disable the interrupt;
static void kinetis_PIT_disable_int(struct kinetis_PIT_timer_interface *iface) {
	//Clear bit 1 of TCTRL;
	iface->registers->TCTRL &= ~PIT_TCTRL_TIE;
}

//Is the interrupt enabled ?
static bool kinetis_PIT_int_enabled(struct kinetis_PIT_timer_interface *iface) {
	//Is bit 1 of TCTRL set ?
	return (bool) (iface->registers->TCTRL & PIT_TCTRL_TIE);
}


//Update the handler;
void kinetis_PIT_update_handler(struct kinetis_PIT_timer_interface *iface, void (*handler)()) {

	//Cache the interrupt channel;
	uint8_t interrupt_channel = iface->reload_int_channel;

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
static bool kinetis_PIT_flag_set(struct kinetis_PIT_timer_interface *iface) {
	//Is bit 0 of TFLG set ?
	return (bool) (iface->registers->TFLG);
}


//Clear the interrupt flag;
static void kinetis_PIT_clear_flag(struct kinetis_PIT_timer_interface *iface) {
	//Set bit 0 of TFLG;
	iface->registers->TFLG = 1;
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

	//Enable PIT clock gating;
	SIM_SCGC6 |= SIM_SCGC6_PIT;

	//Enable clocks for all PITs;
	PIT_MCR = 0x00;

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

	//Disable PIT clock gating;
	SIM_SCGC6 &= ~SIM_SCGC6_PIT;

	//Disable clocks for all PITs;
	PIT_MCR = 0x02;


	//Eventually stop the driver;
	kinetis_PIT_exit(driver);

	//Free the interrupt channels array;
	kernel_free((void *) driver->hw_specs.int_channels);

	//free the driver:
	kernel_free(driver);

}


//---------------------------------------------------- Start - Stop ----------------------------------------------------

/**
 * kinetis_PIT_init :
 * 	- initialises interfaces;
 * 	- initialises the hardware;
 * 	- registers interfaces;
 *
 * @param driver : the PIT driver;
 * @param unused : used for config file, not necessary here;
 */

void kinetis_PIT_init(struct kinetis_PIT_driver *driver, void *unused) {

	//If the driver is already started :
	if (driver->ifaces) {

		//Error, miss-use;
		kernel_error("kinetis_PIT.c : kinetis_PIT_start : attempted to start the driver twice");

	}

	//Create the timer interface initializer;
	struct timer_interface iface_init = {

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

		//Cache the interrupt channel;
		uint8_t int_channel = (specs->int_channels)[PIT_id];

		//Create the PIT timer interface initializer;
		struct kinetis_PIT_timer_interface PIT_init = {

			//Transmit function pointers;
			.iface = iface_init,

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

		//Initialise the timer to 1KHz;
		timer_init((struct timer_interface *) iface, 1000);


		//Set the interrupt priority;
		core_IC_set_priority(int_channel, DRIVER_STARUS_INTERRUPT_PRIORITY);

		//Set the default handler;
		core_IC_set_default_handler(int_channel);


		//Register the timer interface;
		driver->ifaces[PIT_id] = (struct timer_interface *) iface;

	}

}


/**
 * kinetis_PIT_exit :
 * 	- resets the hardware;
 * 	- deletes interfaces;
 * 	- unregisters interfaces;
 *
 * @param driver : the driver to de-init;
 */

void kinetis_PIT_exit(struct kinetis_PIT_driver *driver) {

	//For each interface :
	for (uint8_t PIT_id = 0; PIT_id--;) {

		//Cache the interface;
		struct timer_interface *iface =  (driver->ifaces)[PIT_id];

		//Reset the handler;
		timer_reset(iface);

		//Destruct and delete the timer interface;
		timer_delete(iface);

		//Reset the interface reference;
		(driver->ifaces)[PIT_id] = 0;

	}

}
