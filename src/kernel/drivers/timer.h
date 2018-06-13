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

#include "stdbool.h"


//------------------------------------- State Register -------------------------------------

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
	volatile uint8_t *const state_register;

	//The mask to manipulate only relevant bits;
	const uint8_t bit_mask;

	//The value to write to enable the register;
	const uint8_t enabled_value;

	//The value to write to disable the register;
    const uint8_t disabled_value;

} state_register_t;

//Enable the register;
static inline void state_register_enable(const state_register_t *data);

//Disable the register;
static inline void state_register_disable(const state_register_t *data);

//Is the register enabled ?
static inline bool state_register_is_enabled(const state_register_t *data);


//------------------------------------- Base Timer -------------------------------------

/*
 * A basic timer contains one state register (enabled / disabled),
 * 	a value register, and a conversion factor; 
 */

typedef struct {

    //The timer data structure, required for de-init;
    void *data;
	
	//The register that contains/determines the timer state;
	state_register_t timer_state;

	//The interrupt period register;
	void *period_register;

	//The conversion factor, used to convert number of tics to a register value;
	float period_to_tics;
	
	//The function to update a count register;
	void (*count_register_update)(void *register_p, float new_register_value);

    //The function to de-init the timer;
    void (*timer_exit)(void*);

} timer_base_t;

//Start the timer;
static inline void timer_start_timer(const timer_base_t *timer_data);

//Start the timer;
static inline void timer_stop_timer(const timer_base_t *timer_data);

//Update the period register with the provided value;
static inline void timer_set_period(const timer_base_t *timer_data, float period);

//De initialise the timer and delete the structure;
static inline void timer_exit(timer_base_t *);


//------------------------------------- Timer interrupt -------------------------------------

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

	//A pointer to the IRQ function pointer for fast update;
	void (**hander_update)(void);

} timer_interrupt_data_t;


//Enable the timer interrupt;
static inline void timer_interrupt_enable(timer_interrupt_data_t *timer_data);

//Disable the timer interrupt;
static inline void timer_interrupt_disable(timer_interrupt_data_t *timer_data);


//Clear the timer interrupt flag;
static inline void timer_interrupt_flag_clear(timer_interrupt_data_t *timer_data);

//Is the the timer interrupt flag set;
static inline void timer_interrupt_flag(timer_interrupt_data_t *timer_data);


//Update the handler.
static inline void timer_interrupt_set_handler(timer_interrupt_data_t *timer_data, void (*IRQ_function)(void));


//------------------------------------- Timer complex structs -------------------------------------

/*
 * A struct to represent a timer with an overflow interrupt;
 */

typedef struct {

	//The timer base;
	timer_base_t timer;

	//The ovf interrupt;
	timer_interrupt_data_t ovf_interrupt;

} timer_ovf_int_t;

static inline void timer_ovf_int_exit(const timer_ovf_int_t *timer);

#include "timer.c"

#endif

