/*
  timer.c Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

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

#ifndef TRACER_TIMER_C
#define TRACER_TIMER_C


//------------------------------------- State Register functions -------------------------------------

/*
 * state_register_enable: sets state bits in the timer state register
 */

static inline void state_register_enable(const state_register_t *data) {

	//Clear relevant bits, write the enabled value and store the result;;
	*(data->state_register) = (*(data->state_register) & data->bit_mask) | data->enabled_value;

}


/*
 * state_register_disable: clears state bits in the timer state register
 */

static inline void state_register_disable(const state_register_t *data) {

	//Clear relevant bits, write the enabled value and store the result;;
	*(data->state_register) = (*(data->state_register) & data->bit_mask) | data->disabled_value;

}


/*
 * state_register_is_disabled : get the state. returns 0 for cleared and other for set;
 */

static inline bool state_register_is_enabled(const state_register_t *data) {

	//Get timer state bits in the state register;
	return ((*data->state_register & data->bit_mask) == data->enabled_value);

}


//------------------------------------- Timer base functions -------------------------------------

/*
 * timer_set_period : as manipulating the period required a
 * 	conversion and the call of a specialised function, 
 * 	this inline function simplifies the process; 
 */

static inline void timer_set_period(const timer_base_t *const timer_data, float period) {
	
	//Call the timer's specialised function
	//converting period to register value;
	(*(timer_data->count_register_update))(timer_data->period_register, period * timer_data->period_to_tics);

}


//------------------------------------- Timer interrupts functions -------------------------------------

/*
 * timer_interrupt_set_handler : updates the timer's handler;
 */

static inline void timer_interrupt_set_handler(timer_interrupt_data_t *timer_data, void (*handler)(void))  {

	//Update the IRQ function;
	*timer_data->hander_update = handler;

}


#endif
