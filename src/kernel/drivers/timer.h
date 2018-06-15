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


//------------------------------------- State Register -------------------------------------

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
inline void state_register_enable(const state_register_t *data);

//Disable the register;
inline void state_register_disable(const state_register_t *data);

//Is the register enabled ?
inline bool state_register_is_enabled(const state_register_t *data);


//------------------------------------- Special Register -------------------------------------

/*
 * A struct to contain a pointer to a register where the value set is not correlated with the value that will be read.
 * 	Timer flags are special registers;
 */

typedef struct {

	//The register that contains/determines the controlled state;
	volatile uint8_t *const state_register;

	//The mask to manipulate only relevant bits;
	const uint8_t bit_mask;

	//The value to write to enable the register;
	const uint8_t write_enable_value;

	//The value to write to disable the register;
	const uint8_t write_disable_value;

	//The value read when the register is enabled;
	const uint8_t read_enabled_value;

} special_register_t;


//Enable the register;
inline void special_register_enable(const special_register_t *data);

//Disable the register;
inline void special_register_disable(const special_register_t *data);

//Is the register enabled ?
inline bool special_register_is_enabled(const special_register_t *data);


//------------------------------------- Timer interrupt -------------------------------------

/*
 * A timer interrupt is represented by : 
 *  - the interrupt state register;
 *  - the interrupt flag register;
 *  - the interrupt period register;
 *  - the interrupt IRQ function; 
 */

typedef struct {

	//The interrupt period register;
	volatile void *const period_register;

	//The register that contains/determines the interrupt state;
	state_register_t interrupt_state;

	//The register that contains/clears the interrupt flag
	special_register_t interrupt_flag;

	//A pointer to the IRQ function pointer for fast update;
	void (**hander_update)(void);

} timer_interrupt_data_t;


//Enable the timer interrupt;
inline void timer_interrupt_enable(const timer_interrupt_data_t *timer_interrupt);

//Disable the timer interrupt;
inline void timer_interrupt_disable(const timer_interrupt_data_t *timer_interrupt);


//Clear the timer interrupt flag;
inline void timer_interrupt_flag_clear(const timer_interrupt_data_t *timer_interrupt);

//Is the the timer interrupt flag set;
inline bool timer_interrupt_flag(const timer_interrupt_data_t *timer_interrupt);

//Update the handler.
inline void timer_interrupt_set_handler(const timer_interrupt_data_t *timer_interrupt, void (*handler)(void));


//------------------------------------- Base Timer -------------------------------------

/*
 * A basic timer contains one state register (enabled / disabled),
 * 	a value register, and a conversion factor;
 */

typedef struct timer_base_t {

	//The timer data structure, required for de-init;
	void *data;

	//The register that contains/determines the timer state;
	state_register_t timer_state;

	//The conversion factor, used to convert number of tics to a register value;
	const float period_to_tics;

	//The maximal period that can be set for this timer configuration;
	const float maximal_period;

	//The function to update a count register;
	void (*period_register_update)(volatile void *register_p, float new_register_value);

	//The overflow interrupt
	timer_interrupt_data_t reload_interrupt;

	//The function to de-init the timer;
	void (*timer_exit)(const struct timer_base_t *);

} timer_base_t;


//Start the timer;
inline void timer_start_timer(const timer_base_t *timer_data);

//Stop the timer;
inline void timer_stop_timer(const timer_base_t *timer_data);

//Update the period register with the provided value;
inline void timer_set_period(const timer_base_t *timer_data, float period);

//De initialise the timer and delete the structure;
inline void timer_exit(const timer_base_t *timer_struct);


#endif

#include "timer.c"
