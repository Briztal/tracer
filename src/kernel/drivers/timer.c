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


#ifndef TRACER_TIMER_C
#define TRACER_TIMER_C

#include "timer.h"


//------------------------------------- State Register functions -------------------------------------

/*
 * state_register_enable: sets state bits in the timer state register
 */

inline void state_register_enable(const state_register_t *const data) {

	//Clear relevant bits, write the enabled value and store the result;;
	*(data->state_register) = (*(data->state_register) & data->bit_mask) | data->enabled_value;

}


/*
 * state_register_disable: clears state bits in the timer state register
 */

inline void state_register_disable(const state_register_t *const data) {

	//Clear relevant bits, write the enabled value and store the result;;
	*(data->state_register) = (*(data->state_register) & data->bit_mask) | data->disabled_value;

}


/*
 * state_register_is_disabled : get the state. returns 0 for cleared and other for set;
 */

inline bool state_register_is_enabled(const state_register_t *const data) {

	//Get timer state bits in the state register;
	return ((*data->state_register & data->bit_mask) == data->enabled_value);

}


//------------------------------------- Special Register functions -------------------------------------

/*
 * state_register_enable: sets state bits in the timer state register
 */

inline void special_register_enable(const special_register_t *const data) {

	//Clear relevant bits, write the enabled value and store the result;;
	*(data->state_register) = (*(data->state_register) & data->bit_mask) | data->write_enable_value;

}


/*
 * state_register_disable: clears state bits in the timer state register
 */

inline void special_register_disable(const special_register_t *const data) {

	//Clear relevant bits, write the enabled value and store the result;;
	*(data->state_register) = (*(data->state_register) & data->bit_mask) | data->write_disable_value;

}


/*
 * state_register_is_disabled : get the state. returns 0 for cleared and other for set;
 */

inline bool special_register_is_enabled(const special_register_t *const data) {

	//Get timer state bits in the state register;
	return ((*data->state_register & data->bit_mask) == data->read_enabled_value);

}


//------------------------------------- Timer interrupts functions -------------------------------------


/*
 * timer_interrupt_enable : enables the timer interrupt;
 */

inline void timer_interrupt_enable(const timer_interrupt_data_t *const timer_interrupt) {

	//Enable the state register;
	state_register_enable(&timer_interrupt->interrupt_state);

}


/*
 * timer_interrupt_disable : disables the timer interrupt;
 */

inline void timer_interrupt_disable(const timer_interrupt_data_t *const timer_interrupt) {

	//Enable the state register;
	state_register_disable(&timer_interrupt->interrupt_state);

}


/*
 * Clear the timer interrupt flag;
 */

inline void timer_interrupt_flag_clear(const timer_interrupt_data_t *const timer_interrupt) {

	//Enable the status register;
	special_register_disable(&timer_interrupt->interrupt_flag);

}


/*
 * Is the the timer interrupt flag set;
 */

inline bool timer_interrupt_flag(const timer_interrupt_data_t *const timer_interrupt) {

	//Return the flag state;
	return special_register_is_enabled(&timer_interrupt->interrupt_flag);

}


/*
 * timer_interrupt_set_handler : updates the timer's handler;
 */

inline void timer_interrupt_set_handler(const timer_interrupt_data_t *const timer_interrupt, void (*handler)(void)) {

	//Update the IRQ function;
	*(timer_interrupt->hander_update) = handler;

}

//------------------------------------- Timer base functions -------------------------------------


/*
 * timer_start_timer : starts the timer;
 */

inline void timer_start_timer(const timer_base_t *const timer_data) {

	//Enable the timer state register;
	state_register_enable(&timer_data->timer_state);

}


/*
 * timer_stop_timer : stops the timer;
 */

inline void timer_stop_timer(const timer_base_t *const timer_data) {

	//Enable the timer state register;
	state_register_disable(&timer_data->timer_state);

}


/*
 * timer_set_period : as manipulating the period required a
 * 	conversion and the call of a specialised function,
 * 	this inline function simplifies the process;
 */

inline void timer_set_period(const timer_base_t *const timer_data, float period) {

	//Call the timer's specialised function
	//converting period to register value;
	(*(timer_data->period_register_update))(timer_data->reload_interrupt.period_register,
											period * timer_data->period_to_tics);

}


/*
 * De initialise the timer and delete the structure;
 */

inline void timer_exit(const timer_base_t *const timer_struct) {

	//Call the exit function passing the struct;
	timer_struct->timer_exit(timer_struct);

	//TODO PASS ONLY DATA ?//TODO PASS ONLY DATA ?
	//TODO PASS ONLY DATA ?//TODO PASS ONLY DATA ?
	//TODO PASS ONLY DATA ?//TODO PASS ONLY DATA ?
	//TODO PASS ONLY DATA ?//TODO PASS ONLY DATA ?
	//TODO PASS ONLY DATA ?//TODO PASS ONLY DATA ?
	//TODO PASS ONLY DATA ?//TODO PASS ONLY DATA ?
	//TODO PASS ONLY DATA ?//TODO PASS ONLY DATA ?
	//TODO PASS ONLY DATA ?//TODO PASS ONLY DATA ?
	//TODO PASS ONLY DATA ?//TODO PASS ONLY DATA ?
	//TODO PASS ONLY DATA ?//TODO PASS ONLY DATA ?

}


#endif
