//
// Created by root on 6/15/18.
//


#include "kinetis_PIT.h"

#include <kernel/kernel.h>

//----------------------------------------- PITs private functions declarations ----------------------------------------

//Update the timer's period;
void kinetis_PIT_update_period(volatile void *LDVAL, float period);

//Reset a timer to its default configuration;
void kinetis_PIT_reset(const kinetis_PIT_data_t *);

//De-initialise a timer;
void kinetis_PIT_exit(const timer_base_t *timer_data);


//--------------------------------------------- PITs functions definitions ---------------------------------------------

/*
 * kinetis_PIT_init_as_timer : initialises the PIT as an interrupt timer;
 */

timer_base_t *kinetis_PIT_init_as_timer(kinetis_PIT_data_t *PIT_data, uint32_t base_frequency) {

	//Reset the timer;
	kinetis_PIT_reset(PIT_data);

	//Cache the memory zone pointer;
	kinetis_PIT_memory_t *memory = PIT_data->memory;

	//Determine the tics/period ratio;
	float period_to_tics = ((float) PIT_data->clock_frequency) / ((float) base_frequency);

	//Determine the maximal period;
	float max_period = ((float) (uint32_t) -1) / period_to_tics;

	//Determine the handler address;
	void (*handler_updater)(void) = core_IC_get_handler_address(PIT_data->reload_int_channel);

	//Initializer for the the timer data;
	timer_base_t init = {

		//Store the PIT data for de-initialisation;
		.data = PIT_data,

		//The timer state register;
		.timer_state = {

			//The first byte of the TCTRL register. Little endian, same address;
			.state_register =  (volatile uint8_t *const) &memory->TCTRL,

			//Bit 0 of TCTRL's first byte;
			.bit_mask = 1,

			//Timer enabled <- 2^0;
			.enabled_value = 1,

			//Timer disabled <- 0;
			.disabled_value = 0,

		},

		//Save the determined period factor;
		.period_to_tics = period_to_tics,

		//Save the maximal period;
		.maximal_period = max_period,

		//Update the period updater;
		.period_register_update = &kinetis_PIT_update_period,

		//The reload interrupt;
		.reload_interrupt = {

			//The Reload Value will be our period register;
			.period_register = (volatile void *const) &memory->LDVAL,

			//The interrupt state resides in bit 1 of TCTRL;
			.interrupt_state = {

				//The first byte of the TCTRL register. Little endian, same address;
				.state_register =  (volatile uint8_t *const) &memory->TCTRL,

				//Bit 1 of TCTRL's first byte;
				.bit_mask = 2,

				//Interrupt enabled <- 2^1;
				.enabled_value = 2,

				//Interrupt disabled <- 0;
				.disabled_value = 0,

			},

			//The interrupt flag resides in bit 2 of TCTRL;
			.interrupt_flag = {

				//The first byte of the TFLG register. Little endian, same address;
				.state_register =  (volatile uint8_t *const) &memory->TFLG,

				//Bit 0 of TCTRL's first byte;
				.bit_mask = 1,

				//Cannot be set;
				.write_enable_value = 0,

				//Write 1 to clear;
				.write_disable_value = 1,

				//Set when 1 is read;
				.read_enabled_value = 1,

			},

			//The interrupt handler pointer to update the reload handler;
			.hander_update = &handler_updater,

		},

		//Update the timer exit functions
		.timer_exit = &kinetis_PIT_exit,

	};

	//Allocate some heap memory for the timer data, initialise it and return its pointer;
	return kernel_malloc_copy(sizeof(timer_base_t), &init);

}


/*
 * kinetis_PIT_update_period : writes the given value to the given register;
 */

void kinetis_PIT_update_period(volatile void *LDVAL, float period) {

	//Cast the load pointer to a uint32_t *, disable optimisations, cast the period and write it;
	*(volatile uint32_t *) LDVAL = (uint32_t) period;

}


/*
 * kinetis_PIT_reset : resets the timer to the default config :
 * 	- timer and int off;
 * 	- max period;
 * 	- default interrupt handler;
 */

void kinetis_PIT_reset(const kinetis_PIT_data_t *const data) {

	//Reset the handler;
	core_IC_set_default_handler(data->reload_int_channel);

	//Cache the memory pointer;
	kinetis_PIT_memory_t *memory = data->memory;

	//Disable timer and interrupt;
	memory->TCTRL = 0;

	//Clear the flag;
	memory->TFLG = 1;

	//Reset the current and load value;
	memory->CVAL = memory->LDVAL = (uint32_t) -1;

}


/*
 * kinetis_PIT_exit : de-initialises a timer;
 */

void kinetis_PIT_exit(const timer_base_t *const timer_data) {

	//Reset the timer;
	kinetis_PIT_reset((kinetis_PIT_data_t *)timer_data->data);

	//Delete the timer struct;
	kernel_free((void *) timer_data);

};

