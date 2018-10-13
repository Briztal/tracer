/*
  sysclock.h Part of TRACER

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

#ifndef TRACER_SYSTICK_H
#define TRACER_SYSTICK_H


#include <stdint.h>

#include <stdbool.h>


/*
 * The system clock is the reference clock for the kernel;
 *
 * The library receives ticks, supposedly every half millisecond, and updates its internal counter;
 *
 * Every time its receives a systick, it verifies if it must trigger the context switch, and does it if so;
 */

struct sys_timer {
	
	//Setup;
	void (*const int_set_frequency)(uint32_t);
	void (*const int_set_priority)(uint8_t);
	void (*const int_set_handler)(void (*)());
	
	//Start - stop
	void (*const start)();
	void (*const stop)();
	
	//Int mgt
	void (*const int_enable)();
	void (*const int_disable)();
	
};


/*
 * System clock timer initialisation;
 */

//Initialise the system timer;
void sysclock_init(struct sys_timer *const timer);

//Is the sysclock timer initialised ?
bool sysclock_initialised();


/*
 * System clock operation. Any call with system timer not initialised generates a kernel panic;
 */

//Initialise the system clock;
void sysclock_start();

//Receive a system systick;
void sysclock_tick();

//Update the current task's activity_time;
void sysclock_set_process_duration(uint16_t ms);

//Get the millisecond reference;
uint32_t sysclock_milliseconds();

//Wait till time has reached the given limit;
void systick_wait(uint16_t ms_delay);


#endif //TRACER_SYSTICK_H
