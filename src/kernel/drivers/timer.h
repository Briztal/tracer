/*
  timer.h Part of TRACER

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

#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"

/*
 * This file defines the timer interaction structures;
 *
 *	A timer is composed of a counting unit, that can be set up.
 *  Although, it defines the abstraction between high level periods (ticking)
 *	and timer count registers values;
 *
 *	To this basic structure can be added several interrupts (overflow,
 *	output compare, etc...);
 */


/*
 * A struct to contain a pointer to a register, and its bits to clear / set;
 */

typedef struct {
		
	//The register that contains/determines the controlled state;
	volatile uint8_t *state_register;

	//The mask to manipulate exclusively timer state bits;
	const uint8_t state_bits;

} state_register_t;


/*
 * state_register_set: sets state bits in the timer state register
 */

static inline void state_register_set(state_register_t *data) {

	//Set timer state bits in the state register;
	data->state_register |=	data->timer_state_bits;

}


/*
 * state_register_clear: clears state bits in the timer state register
 */

static inline void state_register_clear(state_register_t *data) {

	//Clear timer state bits in the state register;
	data->state_register &=	~data->timer_state_bits;

}


/*
 * state_register_get : get the state. returns 0 for cleared and other for set;
 */

static inline bool state_register_get(state_register_t *data) {

	//Get timer state bits in the state register;
	return (bool) data->state_register & data->timer_state_bits;

}


/*
 * A basic  timer contains one state register (enabled / disabled), 
 * 	a value register, and a conversion factor; 
 */

typedef struct {
	
	//The register that contains/determines the timer state;
	state_register_t timer_state;

	//The conversion factor, used to convert number of tics to a register value;
	float conversion_factor;
	
	//The function to update a count register;
	void (*count_register_update)(void *register_p, float *new_register_value);

} timer_data_t;


/*
 * A timer interrupt is represented by : 
 *  - the interrupt state register;
 *  - the interrupt flag register;
 *  - the interrupt period register;
 *  - the interrupt IRQ function; 
 */

typedef struct {
	
	//The register that contains/determines the interrupt state;
	state_register_t interrupt_state;
	
	//The register that contains/clears the interrupt flag
	state_register_t interrupt_flag;
	
	//The interrupt period register;
	void *period_register;
	
	//A pointer to the IRQ function pointer for fast update;
	void (**IRQ_update)(void);	

} timer_interrupt_data_t;


/*
 * timer_update_period_register : as manipulating the period required a 
 * 	conversion and the call of a specialised function, 
 * 	this inline function simplifies the process; 
 */

static inline void timer_update_period_register(const timer_data_t *const timer_data,
												const timer_interrupt_data_t *const interrupt_data, float period) {
	
	//Call the timer's specialised function
	//converting period to register value;
	(*(timer_data->count_register_update))(interrupt_data->period_register, 
		register_p, period * timer_data->conversion_factor);

}


/*
 * timer_set_IRQ : updates the timer's IRQ function;
 */

static inline void timer_set_IRQ(timer_data_t *timer_data, 
	void (*IRQ_function)(void)) {

	//Update the IRQ function;
	*timer_data->IRQ_update = IRQ_function; 
	
}


#endif
