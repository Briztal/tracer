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

#include <kernel/debug.h>


//------------------------------------- Base Timer -------------------------------------

/*
 * A basic timer contains one state register (enabled / disabled),
 * 	a value register, and a conversion factor;
 */

struct timer_interface {

	//Update the base frequency;
	void (*const set_base_frequency)(struct timer_interface_t *, uint32_t base_frequency;)

	//Start the timer;
	void (*const start)(struct timer_interface *);

	//Stop the timer;
	void (*const stop)(struct timer_interface *);

	//Is the timer started ?;
	bool (*const started)(struct timer_interface *);


	//Set the timer's count;
	void (*const set_count)(struct timer_interface *, float period);

	//Get the timer's count;
	float (*const get_count)(struct timer_interface *);


	//Set the reload interrupt value;
	void (*const set_ovf_value)(struct timer_interface *, float period);

	//Get the reload interrupt value;
	float (*const get_ovf_value)(struct timer_interface *);


	//Enable the reload interrupt;
	void (*const enable_int)(struct timer_interface *);

	//Enable the reload interrupt;
	void (*const disable_int)(struct timer_interface *);

	//Is the reload interrupt enabled ?
	bool (*const int_enabled)(struct timer_interface *);

	//Update the interrupt handler;
	void (*const update_handler)(struct timer_interface *, void (*handler)());


	//Is the interrupt flag enabled ?
	bool (*const int_flag_set)(struct timer_interface *);

	//Is the interrupt flag enabled ?
	void (*const clr_int_flag)(struct timer_interface *);


	//The function to destruct the timer instance;
	void (*deleter)(const struct timer_interface *);

};


//Start the timer;
static inline void timer_start(struct timer_interface *timer) { (*(timer->start))(timer); }

//Start the timer;
static inline void timer_stop(struct timer_interface *timer) { (*(timer->start))(timer); }

static inline bool timer_started(struct timer_interface *timer) { return (*(timer->started))(timer); }


//Set the timer count value;
static inline void timer_set_count(struct timer_interface *timer, float period) {

	(*(timer->set_count))(timer, period);

}

//Set the timer count value;
static inline float timer_get_count(struct timer_interface *timer) {

	return (*(timer->get_count))(timer);
}


//Set the timer reload value;
static inline void timer_set_ovf_value(struct timer_interface *timer, float period) {


	(*(timer->set_ovf_value))(timer, period);
}

//Set the timer count value;
static inline float timer_get_ovf_value(struct timer_interface *timer, float period) {

	return (*(timer->get_ovf_value))(timer);
}




//Enable the reload interrupt;
static inline void timer_enable_int(struct timer_interface *timer) {
	(*(timer->enable_int))(timer);
}

//Disable the reload interrupt;
static inline void timer_disable_int(struct timer_interface *timer) {
	(*(timer->disable_int))(timer);
}

//Disable the reload interrupt;
static inline bool timer_int_enabled(struct timer_interface *timer) {
	return (*(timer->int_enabled))(timer);
}

//Set the timer count value;
static inline void timer_update_handler(struct timer_interface *timer, void (*handler)()) {

	(*(timer->update_handler))(timer, handler);
}


//Enable the reload interrupt;
static inline bool timer_int_flag_set(struct timer_interface *timer) {
	return (*(timer->int_flag_set))(timer);
}

//Disable the reload interrupt;
static inline void timer_clr_int_flag(struct timer_interface *timer) {
	(*(timer->clr_int_flag))(timer);
}

//Delete the timer interface;
static inline void timer_destruct(struct timer_interface *timer) { (*(timer->deleter))(timer); }


#endif

