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

#include "time_interval.h"

#include <kernel/debug/log.h>

const struct time_interval time_interval_largest = {
	.min = 0,
	.max = 0,
	.max_exists = false,
	.valid = true,
	.empty = false,
};


//Reset a time interval to the largest one;
void time_interval_reset(struct time_interval *dst) {
	
	*dst = time_interval_largest;
	
}


/*
 * This function will correct the final duration window, including information from the provided optional interval;
 *
 * 	The corrected final interval will be a portion of the provided one; If both interval do not intersect,
 * 	the appropriate bound is chosen, as the unique value of the final interval, that becomes a singleton;
 */

bool time_interval_intersect(struct time_interval *const dst, const struct time_interval bound) {
	
	//If the constraint interval is invalid :
	if (!bound.valid) {
		
		//Error, the constraint must provide a valid interval;
		kernel_log_("machine_controller.c : mcontroller_correct_duration_window : the constraint "
						"interval is invalid;");
		
	}
	
	//If the provided interval is empty :
	if (bound.empty) {
		
		//Nothing to do; As this function is executed, the final interval is not a singleton;
		return false;
		
	}
	
	//Cache all required data. Min var is mutable, as is susceptible to be changed in the first step;
	float min = dst->min;
	const bool max_exists = dst->max_exists;
	const float max = dst->max;
	
	//We can correct the interval's minimum bound;
	
	//If the minimal bound must be corrected :
	if (min < bound.min) {
		
		//If the final interval's maximum bound exists, and is lower than the optional interval's minimum :
		if (max_exists && (max <= bound.min)) {
			
			//The final interval becomes a singleton containing max;
			dst->min = max;
			return true;
			
		}
		
		//If not, just correct the minimal bound and its cached version;
		dst->min = min = bound.min;
		
	}
	
	
	//If the optional maximum exists, we can correct the final interval's max bound:
	if (bound.max_exists) {
		
		//If it is smaller that the minimum :
		if (bound.max <= min) {
			
			//The final interval becomes a singleton containing min;
			dst->max_exists = true;
			dst->max = min;
			return true;
			
		}
		
		//If the final interval's max doesn't exist, or if it is greater that the optional interval's one :
		if ((!max_exists) || (bound.max < max)) {
			
			//Update the maximal bound, its flag;
			dst->max_exists = true;
			dst->max = bound.max;
			
		}
		
	}
	
	
	//The singleton cases have been evaluated. The final interval is not a singleton;
	return false;
	
}


/*
 * timer_interval_merge : this function returns an interval that :
 * 	- has its max as the minimum of all intervals maximum bounds;
 * 	- has its min as the maximum of all intervals minimum bounds;
 * 	- is empty if all movements are empty;
 * 	- is valid if all movements are valid, and min <= max;
 */

struct time_interval timer_interval_merge(const uint8_t dimension, const struct time_interval *const intervals) {
	
	//Declare the final interval; will be saved at the end;
	struct time_interval final_interval = {
		
		//Largest width;
		.max_exists = false,
		.max = 0,
		.min = 0,
		
		//Initialised empty, will be cleared as soon as a non empty interval is found;
		.empty = true,
		
		//Initialised valid, will be cleared if a non valid interval is found;
		.valid = true,
		
	};
	
	//We will determine the minimum of all max bounds, and the maximum of all min bounds;
	
	//For each interval except the first (done one line earlier):
	for (uint8_t actuator_index = 0; actuator_index < dimension; actuator_index++) {
		
		//Cache the current interval pointer;
		const struct time_interval *const interval = intervals + actuator_index;
		
		//Propagate the invalidity;
		final_interval.valid = final_interval.valid && interval->valid;
		
		//If the interval is empty, it is not taken into account in the bounds comput;
		
		//If the actuator's duration interval is not empty :
		if (!interval->empty) {
			
			//Mark the final interval not empty;
			final_interval.empty = false;
			
			//The minimal time always exists;
			float min = interval->min;
			
			//If the interval's is superior to the computed minimum :
			if (min > final_interval.min) {
				
				//Update the minimum bound;
				final_interval.min = min;
				
			}
			
			//If the interval's maximal time exists :
			if (interval->max_exists) {
				
				//Cache it;
				float max = interval->max;
				
				//If the final interval's max bound exists :
				if (final_interval.max_exists) {
					
					//If it is inferior to the computed maximum :
					if (max < final_interval.max) {
						
						//Update the minimum bound;
						final_interval.max = max;
						
					}
					
				} else {
					//If the final interval's max doesn't exist :
					
					//Update it to the interval's one, and mark the max bound existing;
					final_interval.max_exists = true;
					final_interval.max = max;
					
				}
				
			}
			
		}
		
	}
	
	//Update the validity flag to consider also bounds crossing;
	final_interval.valid |= (final_interval.min > final_interval.max);
	
	//Return the computed interval;
	return final_interval;
	
}
