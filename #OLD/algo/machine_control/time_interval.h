/*
  time_interval.h Part of TRACER

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

#ifndef TRACER_TIME_INTERVAL_H
#define TRACER_TIME_INTERVAL_H

#include <stdbool.h>

#include <stdint.h>


/*
 * To describe durations, we here define time interval type;
 */

struct time_interval {

	//The minimum. Exists because the interval is closed; The minimum always exists, as duration is positive;
	float min;

	//The maximum. Exists because the interval is closed;
	float max;

	//The maximum doesn't always exist. This flag is set when it actually does;
	bool max_exists;

	//A useful flag that contains the value of "min > max" if related, or simply tells that the interval is empty;
	bool empty;

	//A useful flag, to mention that the interval is invalid in the considered circumstances;
	bool valid;

};

//The largest interval; Can be used to reset a time interval;
const struct time_interval time_interval_largest;

//Modify the dst interval, to be the intersection of itself and the provided bound.
// If intersection does not exist, dst is reduced to its appropriate bound;
bool time_interval_intersect(struct time_interval *dst, struct time_interval bound);

/*
 * timer_interval_merge : this function returns an interval that :
 * 	- has its max as the minimum of all intervals maximum bounds;
 * 	- has its min as the maximum of all intervals minimum bounds;
 * 	- is empty if all movements are empty;
 * 	- is valid if all movements are valid, and min <= max;
 */

struct time_interval timer_interval_merge(uint8_t dimension, const struct time_interval *intervals);


#endif //TRACER_TIME_INTERVAL_H
