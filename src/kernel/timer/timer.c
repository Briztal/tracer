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

void timer_init(struct timer_interface *timer, uint32_t base_frequency) {

	//Stop the timer;
	timer_stop(timer);

	//Disable the timer interrupt;
	timer_disable_int(timer);

	//Clear the interrupt flag;
	timer_clr_int_flag(timer);

	//Reset the handler to 0;
	timer_update_handler(timer, 0);

	//Set the base frequency;
	timer_set_base_frequency(timer, base_frequency);

	//Set the count to 0 period units;
	timer_set_count(timer, 0);

	//Set the ovf to 1 period unit;
	timer_set_ovf_value(timer, 1);

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

void timer_reset(struct timer_interface *timer) {

	//Stop the timer;
	timer_stop(timer);

	//Disable the timer interrupt;
	timer_disable_int(timer);

	//Clear the interrupt flag;
	timer_clr_int_flag(timer);

	//Reset the handler to 0;
	timer_update_handler(timer, 0);

	//Set the count to 0 period units;
	timer_set_count(timer, 0);

	//Set the ovf to 1 period unit;
	timer_set_ovf_value(timer, 1);

}
