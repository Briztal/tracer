//
// Created by root on 6/15/18.
//


#include "kinetis_PIT.h"

#include <kernel/kernel.h>

#include <kernel/debug.h>


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


//The timer interface doesn't store any dynamic data. Nothing to delete;
static void kinetis_PIT_timer_interface_destructor(struct kinetis_PIT_timer_interface *data) {}


//------------------------------------------- PITs timer interface functions -------------------------------------------


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





//--------------------------------------------- Driver Creation - Deletion ---------------------------------------------

//Create a kinetis PIT driver from all hardware peripherals;
struct kinetis_PIT_driver *kinetis_PIT_create(void *const registers, const uint8_t int_channel,
											  const uint32_t clock_frequency) {



	//Initialise the hardware specs structure with provided parameters;
	struct kinetis_PIT_hw hw_init = {
		.registers = registers,
		.reload_int_channel = int_channel,
		.clock_frequency = clock_frequency,
	};

	//Allocate and initialise the hw specs struct;
	struct kinetis_PIT_hw *hw_specs = kernel_malloc_copy(sizeof(struct kinetis_PIT_hw), &hw_init);

	//Create the driver initializer;
	struct kinetis_PIT_driver driver_init = {

		//Transmit the ownership of the specs struct;
		.hw_specs = hw_specs,

		//The interface will be initialised at startup;
		.iface = 0,

	};

	//Allocate, initialise and return the driver;
	return kernel_malloc_copy(sizeof(struct kinetis_PIT_driver), &driver_init);

}

//Delete a kinetis PIT driver;
void kinetis_PIT_delete(struct kinetis_PIT_driver *driver) {

	//Eventually stop the driver;
	kinetis_PIT_stop(driver);

	//Free the hardware specs struct;
	kernel_free(driver->hw_specs);

	//free the driver:
	kernel_free(driver);

}


//---------------------------------------------------- Start - Stop ----------------------------------------------------

//Start the driver, providing the configuration;
void kinetis_PIT_start(struct kinetis_PIT_driver *driver, struct kinetis_PIT_config *config) {

	//If the driver is already started :
	if (driver->iface) {

		//Error, miss-use;
		kernel_error("kinetis_PIT.c : kinetis_PIT_start : attempted to start the dirver twice");

	}

	//Cache the reload int channel;
	uint8_t channel = driver->hw_specs->reload_int_channel;

	//Determine the tics/period ratio;
	float period_to_tics = ((float) driver->hw_specs->clock_frequency) / ((float) config->base_frequency);

	//Determine the maximal period;
	float max_period = ((float) (uint32_t) -1) / period_to_tics;


	//Create the PIT timer interface initializer;
	struct kinetis_PIT_timer_interface iface_init = {

		.iface = {

			.maximal_period = max_period,

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

			.destructor = (void (*)(const struct timer_interface *)) kinetis_PIT_timer_interface_destructor,

		},

		//Transmit the reference of the registers zone;
		.registers = driver->hw_specs->registers,

		//Save the period to tics conversion factor;
		.period_to_tics = period_to_tics,

		//Copy the reload interrupt chanel;
		.reload_int_channel = channel,

	};


	//Allocate, initialise and save the timer interface;
	driver->iface = kernel_malloc_copy(sizeof(struct kinetis_PIT_timer_interface), &iface_init);


	/*
	 * Initialise the PIT;
	 */

	//The period will be of one unit;
	float period = 1;


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

}


//Stop the driver. Timer interface will be deleted;
void kinetis_PIT_stop(struct kinetis_PIT_driver *driver) {

	//Cache the interface;
	struct kinetis_PIT_timer_interface *iface = (struct kinetis_PIT_timer_interface *) driver->iface;

	if (driver->iface) {

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
		timer_destruct(driver->iface);
		kernel_free(driver->iface);

		//Deinit the driver;
		driver->iface = 0;

	}

}
