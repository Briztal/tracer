/*
  cmd.h Part of TRACER

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

#ifndef TRACER_CMD_H
#define TRACER_CMD_H

#include "iface.h"

/*The command if references a single value that can be adjusted;*/
struct cmd_if {

	/*
	 * Constants;
	 */

	/*The minimal value the command can take;*/
	const uint32_t min;

	/*The maximal value the command can take;*/
	const uint32_t max;

	/*The minimal value expressed in another dimension, related to the command; Used for conversion;*/
	const uint32_t hl_min;

	/*The maximal value expressed in another dimension, related to the command; Used for conversion;*/
	const uint32_t hl_max;

	
	/*
	 * Functions;
	 */

	/*Set to a certain value;*/
	void (*const set)(uint32_t);

	/*Disable;*/
	void (*const disable)();

};


/*The neutral command if. Does nothing;*/
extern struct cmd_if neutral_cmd_if;


static inline void cmd_set(const struct cmd_if *iface, uint32_t value) {
	(*(iface->set))(value);
}


static inline void cmd_disable(const struct cmd_if *iface) {
	(*(iface->disable))();
}

#endif /*TRACER_CMD_H*/
