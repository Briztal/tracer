/*
  timer.c Part of TRACER

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

#include "timer.h"

#include <util/string.h>

/**
 * timer_init :
 * 	- stops the timer;
 * 	- Disables the interrupt;
 * 	- Clears the interrupt flag;
 * 	- Reset the handler to 0;
 * 	- Set the base frequency;
 * 	- Set count to 0;
 * 	- Set reload to 1;
 *
 * @param timer : the timer to initialise;
 * @param base_frequency : the required base frequency for the timer;
 */

void timer_init(const struct timer_if *timer, uint32_t base_frequency, uint32_t ovf_period, void (*handler)()) {

	//Stop the timer;
	timer_stop(timer);

	//Disable the timer interrupt;
	timer_disable_int(timer);

	//Clear the interrupt flag;
	timer_flag_clr(timer);

	//Set the handler;
	timer_set_handler(timer, handler);

	//Set the base frequency;
	timer_set_base_frequency(timer, base_frequency);

	//Set the count and ovf to required values;
	timer_set_int_period(timer, ovf_period);

	//Enable the timer interrupt;
	timer_enable_int(timer);

	//Start the timer;
	timer_start(timer);


}


/**
 * timer_reset :
 * 	- stops the timer;
 * 	- disable the interrupt;
 * 	- clear the interrupt flag;
 * 	- reset the handler to 0;
 * 	- sets count to 0;
 * 	- sets reload to 1;
 *
 * @param timer
 */

void timer_reset(const struct timer_if *timer, uint32_t base_frequency) {

	//Stop the timer;
	timer_stop(timer);

	//Disable the timer interrupt;
	timer_disable_int(timer);

	//Clear the interrupt flag;
	timer_flag_clr(timer);

	//Reset the handler to 0;
	timer_set_handler(timer, 0);

	//Set the base frequency;
	timer_set_base_frequency(timer, base_frequency);

	//Set the ovf to 1 period unit; count will be updated at the same time;
	timer_set_int_period(timer, 1);

}

//Implement interface capabilities;
DEFINE_INTERFACE(timer_if);

//----------------------------------------------- Neutral timer interface ----------------------------------------------


//Dumb functions;
void n_sbf(uint32_t u){};
uint32_t n_g(){return 0;};
void n_st() {};
bool n_stt() {return false;};
void n_uh(void (*h)()) {};


/*
 * The neutral timer interface can be written safely on any timer interface. It will prevent access to previous
 * 	functions, without causing null pointer access. Functions have no side effects;
 */

const struct timer_if neutral_timer_if = {

	.set_base_frequency = n_sbf,
	.start = n_st,
	.stop = n_st,
	.started = n_stt,
	.set_count = n_sbf,
	.get_count = n_g,
	.set_int_period = n_sbf,
	.get_int_period = n_g,
	.enable_int = n_st,
	.disable_int = n_st,
	.int_enabled = n_stt,
	.flag_is_set = n_stt,
	.flag_clr = n_st,
	.set_handler = n_uh,

};