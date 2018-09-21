/*
  kx_pit_channel.c Part of TRACER

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

//------------------------------------------------ Module channel header -----------------------------------------------

#include <kernel/mod/module_channel.h>

//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * Makefile must provide, in addition to channel parameters :
 * 	- REFERENCE_SYMBOL : name of the channel;
 * 	- INT_CHANNEL : interrupt channel;
 * 	- REG : start of the channel's registers area;
 */

//If one of the macro was not provided :
#if !defined(CHANNEL_NAME) || !defined(INT_CHANNEL) || !defined(REG)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define CHANNEL_NAME "name"
#define INT_CHANNEL 0
#define REG 0

#endif


//------------------------------------------------ Internal parameters ------------------------------------------------

#define REGISTERS ((volatile uint32_t *)(REG))

#define LDVAL    ((volatile uint32_t *) (REGISTERS))
#define CVAL    ((const volatile uint32_t *) (REGISTERS + 1))
#define TCTRL    ((volatile uint32_t *) (REGISTERS + 2))
#define TFLG    ((volatile uint32_t *) (REGISTERS + 3))

//Enable the timer;
#define TCTRL_TEN    ((uint32_t)(1<<0))

//Enable the interrupt;
#define TCTRL_TIE    ((uint32_t)(1<<1))

//Timer interrupt flag. W1C
#define TFLG_TIF (1<<0)


//------------------------------------------------ headers ------------------------------------------------

#include <stdint.h>

#include <stdbool.h>
#include <kernel/ic.h>

#include "kx_pit_n.h"


//-------------------------------------------------- Timer Operations --------------------------------------------------


//TODO BUS CLOCK;

static uint32_t get_frequency(uint32_t frequency) {

	return 24000000;
	//TODO::::::: get the appropriate clock;

}


//Enable the timer;
static void start() {

	//Set TEN;
	(*TCTRL) |= TCTRL_TEN;
}


//Disable the timer;
static void stop() {

	//Clear TEN;
	*TCTRL &= ~TCTRL_TEN;
}


//Is the timer enabled ?
static bool started() {

	//Return the state of TCTRL_TEN;
	return (bool) (*TCTRL & TCTRL_TEN);

}


//Update the reload value;
static void set_count(uint32_t count) {


	//If the period count is invalid, use the maximal period;
	if (count >= (uint32_t) -1) {
		count = (uint32_t) -1;
	}

	//Cache the current reload value;;
	uint32_t reload = *LDVAL;


	//Disable the timer;
	stop();

	//Set the new reload value;
	*LDVAL = count;

	//Start, so that count starts at val;
	start();

	//Data memory barrier;
	__asm__ __volatile ("DMB");

	//Restore the previous value;
	*LDVAL = reload;

}

//Get the current value;
static uint32_t get_count() {
	return (((uint32_t) *CVAL));/// period_to_tics);
}


//Update the reload value;
static void set_ovf(uint32_t count) {

	//If the period count is invalid, use the maximal period;
	if (count >= (uint32_t) -1) {
		count = (uint32_t) -1;
	}

	/*
	 * Update LDVAL, so that interrupts happen at the required period, and update CVAL, so that the next
	 * interrupt hapens in @period_count period units;
	 */

	//Determine the value to store;
	uint32_t value = count;

	//Update reload value;
	*LDVAL = value;

	//Stop the timer;
	stop();

	//Memory barrier;
	__asm__ __volatile__ ("DMB");

	//Start the timer, to restart a cycle at value;
	start();


}

//Get the reload value;
static uint32_t get_ovf() {
	return *LDVAL;
}


//Enable the interrupt;
static void enable_int() {
	//Set bit 1 of TCTRL;
	*TCTRL |= TCTRL_TIE;

}

//Disable the interrupt;
static void disable_int() {

	//Clear bit 1 of TCTRL;
	*TCTRL &= ~TCTRL_TIE;
}

//Is the interrupt enabled ?
static bool int_enabled() {

	//Is bit 1 of TCTRL set ?
	return (bool) (*TCTRL & TCTRL_TIE);

}


//Is the flag set ?
static bool flag_is_set() {
	//Is bit 0 of TFLG set ?
	return (bool) (*TFLG);
}


//Clear the interrupt flag;
static void flag_clr() {

	ic_clear_interrupt_pending(INT_CHANNEL);

	//Set bit 0 of TFLG;
	*TFLG = TFLG_TIF;
}


//The client handler. Can be modified, and null;
static void (*client_handler)();

/**
 * The channel handler. Clears the interrupt flag, and calls the client handler if not null;
 */

static void handler() {

	//Clear the flag, to prevent more interrupt requests;
	flag_clr();

	//Cache the custom handler;
	void (*hdlr)() = client_handler;

	//If the custom handler is not null, execute it;
	if (hdlr) (*hdlr)();

}


//Update the handler;
static void set_handler(void (*handler)()) {

	//Update the client handler;
	client_handler = handler;

}


/*
 * The timer interface;
 */

//struct kx_pit_channel_data NM(kx_pit_channel) = {



static struct channel_specs channel = {

	//Initialise the interface;
	.iface = {

		.maximum = (uint32_t) -1,
		.frequency = 0,

		.get_frequency = &get_frequency,

		.start = &start,
		.stop = &stop,
		.started = &started,

		.set_count = &set_count,
		.get_count = &get_count,

		.set_int_period = &set_ovf,
		.get_int_period = &get_ovf,

		.enable_int = &enable_int,
		.disable_int = &disable_int,
		.int_enabled = &int_enabled,

		.flag_is_set = &flag_is_set,
		.flag_clr = &flag_clr,

		.set_handler = &set_handler,

	},

	//Save the string name;
	.name = STR(CHANNEL_NAME),

	//Save the interrupt channel;
	.interrupt_channel = INT_CHANNEL,

	//Provide a reference to the handler;
	.handler = &handler,

};


//Reference the channel;
MODULE_REFERENCE_CHANNEL(channel);

