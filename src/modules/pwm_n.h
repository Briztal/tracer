/*
  pwm_n.h -  Part of TRACER

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

#ifndef TRACER_PWM_X_H
#define TRACER_PWM_X_H

#include <stdint.h>
#include <kernel/interface/command.h>


struct channel_specs {

	//The command interface,
	const struct command_if iface;

	//The isr handler;
	void (*const isr_handler)(void);

	//The name of the channel;
	const char *channel_name;

	//The name of the timer file;
	const char *timer_name;

	//The name of the gpio file;
	const char *gpio_name;

	//The period of the pwm;
	const uint32_t period;

	//The tolerance of the timer;
	const uint32_t tolerance;

};


#endif //TRACER_PWM_X_H
