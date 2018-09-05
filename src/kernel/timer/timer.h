/*
  timer.h Part of TRACER

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


#ifndef TIMER_H
#define TIMER_H

#include "stdint.h"

#include "stdbool.h"

#include <core/debug.h>


struct timer_interface {

	//Update the base frequency;
	void (*const set_base_frequency)(uint32_t base_frequency);


	//Start the timer;
	void (*const start)();

	//Stop the timer;
	void (*const stop)();

	//Is the timer started ?;
	bool (*const started)();


	//Set the timer's count;
	void (*const set_count)(float period);

	//Get the timer's count;
	float (*const get_count)();


	//Set the reload interrupt value;
	void (*const set_ovf_value)(float period);

	//Get the reload interrupt value;
	float (*const get_ovf_value)();


	//Enable the reload interrupt;
	void (*const enable_int)();

	//Enable the reload interrupt;
	void (*const disable_int)();

	//Is the reload interrupt enabled ?
	bool (*const int_enabled)();

	//Update the interrupt handler;
	void (*const update_handler)(void (*handler)());


	//Is the interrupt flag enabled ?
	bool (*const int_flag_set)();

	//Is the interrupt flag enabled ?
	void (*const int_flag_clr)();


	//The function to destruct the timer instance;
	void (*deleter)();


};

//Initialise the timer. Disable timer, interrupt and flag, set the base frequency, and reset count and reload;
void timer_init(struct timer_interface *, uint32_t base_frequency);

//Stop the timer, stop the interrupt, reset counts, reset the handler to 0;
void timer_reset((struct timer_interface *);


//Set the base frequency of the timer;
static inline void timer_set_base_frequency(timer, uint32_t base_frequency) {
	(*(timer->set_base_frequency))(timer, base_frequency);
}

//Start the timer;
static inline void timer_start(timer) { (*(timer->start))(timer); }

//Start the timer;
static inline void timer_stop(timer) { (*(timer->stop))(timer); }

//Is the timer started;
static inline bool timer_started(timer) { return (*(timer->started))(timer); }


//Set the timer count value;
static inline void timer_set_count(timer, float period) {
	(*(timer->set_count))(timer, period);

}

//Set the timer count value;
static inline float timer_get_count(timer) {

	return (*(timer->get_count))(timer);
}


//Set the timer reload value;
static inline void timer_set_ovf_value(timer, float period) {

	(*(timer->set_ovf_value))(timer, period);
}

//Set the timer count value;
static inline float timer_get_ovf_value(timer) {

	return (*(timer->get_ovf_value))(timer);
}




//Enable the reload interrupt;
static inline void timer_enable_int(timer) {
	(*(timer->enable_int))(timer);
}

//Disable the reload interrupt;
static inline void timer_disable_int(timer) {
	(*(timer->disable_int))(timer);
}

//Disable the reload interrupt;
static inline bool timer_int_enabled(timer) {
	return (*(timer->int_enabled))(timer);
}

//Set the timer count value;
static inline void timer_update_handler(timer, void (*handler)()) {

	(*(timer->update_handler))(timer, handler);
}


//Enable the reload interrupt;
static inline bool timer_int_flag_set(timer) {
	return (*(timer->int_flag_set))(timer);
}

//Disable the reload interrupt;
static inline void timer_clr_int_flag(timer) {
	(*(timer->int_flag_clr))(timer);
}

//Delete the timer interface;
static inline void timer_delete(timer) { (*(timer->deleter))(timer); }


#endif

