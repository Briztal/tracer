//
// Created by root on 6/25/18.
//

#include "kinematics.h"
#include "mbuilder.h"
#include "modules/machine_control/movement.h"


inline void compute_acceleration_bounds(kinematics_t *kinematics, kinematics_acceleration_data_t *data) {

	//Call the appropriate function passing provided parameters;
	(*(kinematics->compute_acceleration_bounds))(kinematics, data);

}

inline void compute_stop_distance(kinematics_t *kinematics, kinematics_stop_distance_data_t *data) {

	//Call the appropriate function passing provided parameters;
	(*(kinematics->compute_stop_distance))(kinematics, data);

}

inline void update_integrated_stop_distance(kinematics_t *kinematics, kinematics_stop_distance_data_t *data) {

	//Call the appropriate function passing provided parameters;
	(*(kinematics->update_integrated_stop_distance))(kinematics, data);

}

inline float get_max_jerk_speed(kinematics_t *kinematics, uint8_t axis, float equation_coefficient) {

	//Call the appropriate function passing provided parameters;
	return (*(kinematics->compute_max_jerk_speed))(axis, equation_coefficient);

}

/*
 * get_sub_movement_time : this function determines the correct time, for the provided movement;
 *
 *  The movement is passed in he form of :
 *      - movement distance : the high level distance
 *      - stepper_distances : the effective machine_control step_distances of the current movement;
 *
 *  It starts to determine the largest time window, at the current speeds. This window is the intersection of
 *      time windows for all machine_control.
 *
 *  Finally, the regulation time is determined, according to the following rule :
 *      - if the machine must decelerate_of, the chosen time is the upper bound of the determined window.
 *      - if not, if the regulation time is outside of the regulation window, the new time is the corresponding bound;
 *      - if not, the current time is the regulation time.
 *
 */

void kinematics_compute_movement_data(kinematics_t *const kinematics, const mbuilder_t *const mbuilder,
									  movement_t *const movement) {

	//Create a pointer for the actuation array;
	const float *actuation_distances = mbuilder->actuation_distances;

	//TODO SPEED GROUPS;
	//Cache the movement distance;
	const float movement_distance = sub_movement_data->movement_distance;

	//Cache the target speed;
	const float target_speed = sub_movement_data->regulation_speed;

	//The regulation time, corresponding to the regulation regulation_speed;
	float target_time = movement_distance / target_speed;


	//Cache the actuator speeds pointer;
	float *const actuators_speeds = kinematics->actuators_speeds;

	//Cache duration of the previous movement;
	float previous_duration = kinematics->previous_movement_duration;

	//To simplify the reading, ours time bounds vars will be in structs;
	typedef struct {

		//The movement time;
		float time;

		//The movement minimal time;
		float min_time;

		//The movement maximal time;
		float max_time;

		//Have the struct been initialised ?
		bool max_time_initialised;

	} time_bounds_t;


	//Create our time bounds struct;
	time_bounds_t time_bounds = {
		.time = 0,
		.min_time = 0,
		.max_time = 0,
		.max_time_initialised = false,
	};


	//Create the acceleration bounds structure; Only initialise the actuator identifier;
	kinematics_acceleration_data_t acc_data = {
		.actuator = mbuilder->dimension,
	};

	//For each actuator :
	for (; acc_data.actuator--;) {

		//Cache the actuator'minimal_speed current target_speed;
		float axis_speed = actuators_speeds[acc_data.actuator];

		//Cache the actuation distance;
		float actuation_distance = actuation_distances[acc_data.actuator];

		//Acceleration and deceleration computation;
		compute_acceleration_bounds(kinematics, &acc_data);


		//--------------------------------- Min time bound update ---------------------------------

		//Compute the maximal acceleration at that target_speed;
		//The maximal target_speed increase is the product of the acceleration and the previous_duration of the previous movement;
		//Add it to the current target_speed to get the actuator's maximal acceptable target_speed;
		float maximal_speed = axis_speed + previous_duration * acc_data.max_acceleration;

		//The minimal time bound always exists, as there always is a maximal target_speed;

		//update the minimum time :
		//Formula : time = actuator_distance / target_speed
		float minimal_time = actuation_distance / maximal_speed;

		//update minimum time, as the maximum of the new time and the current beginning time :
		time_bounds.min_time = (minimal_time < time_bounds.min_time) ? time_bounds.min_time : minimal_time;


		//--------------------------------- Max time bound update ---------------------------------

		//Compute the maximal deceleration at that target_speed;
		//The maximal target_speed increase is the product of the acceleration and the previous_duration of the previous movement;
		//Subtract it from the current target_speed to get the actuator's minimal acceptable target_speed;
		float minimal_speed = axis_speed - previous_duration * acc_data.max_deceleration;

		//If this actuator cannot stop, (min target_speed positive), the maximal time will exist;
		if (minimal_speed > 0) {

			//Determine the maximal time on the actuator;
			//Formula : time = actuation_distance / actuator_speed;
			float max_time = actuation_distance / minimal_speed;

			//If the maximal time is already initialised, a comparison is required;
			if (time_bounds.max_time_initialised) {

				//Update the maximum time, to be the minimum of the new time and the current ending time :
				if (time_bounds.max_time > max_time) time_bounds.max_time = max_time;

			} else {

				//If not, simply initialise the maximal time and mark it initialised;
				time_bounds.max_time = max_time;
				time_bounds.max_time_initialised = true;

			}

		}

	}

	//If acceleration bounds were couldn't be respected, bounds didn't intersect. Correct it :
	if ((time_bounds.max_time_initialised) && (time_bounds.max_time < time_bounds.min_time)) {

		//Always decelerate if bounds were not respected <- save the maximal time;
		time_bounds.min_time = time_bounds.max_time;
	}

	//Deceleration :
	//If a deceleration is required we will effectively decelerate if a stop is not possible (max time initialised);
	//If the maximal time is initialised, we must check that the target time is not greater;
	if ((time_bounds.max_time_initialised) &&
		(kinematics->deceleration_required || (target_time > time_bounds.max_time))) {

		//Select the time corresponding to the maximum deceleration.
		time_bounds.time = time_bounds.max_time;

	} else if (target_time < time_bounds.min_time) {
		//If the regulation time is lower than the minimal time (can't accelerate to target speed in one movement):

		//Select the time corresponding to the maximal acceleration.
		time_bounds.time = time_bounds.min_time;

	} else {
		//If the regulation time is in the time window :

		//Select the target time;
		time_bounds.time = target_time;
	}

	//Clear the deceleration flag;
	kinematics->deceleration_required = false;

	//Update the movement time;
	movement->time_to_dest = kinematics->previous_movement_duration = time_bounds.time;

}


/*
 * kinematics_update_jerk_distances : Updates distances to jerk point after a movement's computation;
 */

void kinematics_update_jerk_distances(kinematics_t *kinematics, uint8_t dimension, const movement_t *movement) {

	//Cache the pointer to movement distances;
	uint16_t *movement_distances = movement->actuation_distances;

	//Cache the pointer to jerk distances;
	uint32_t *jerk_distances = kinematics->jerk_distances;

	//For each actuator :
	while (dimension--) {

		//Cache and convert the distance for this actuator;
		uint32_t movement_distance = *(movement_distances++);

		//Cache the jerk distance for this actuator;
		uint32_t jerk_distance = *jerk_distances;

		//Update the jerk distance. Underflow is checked;
		*(jerk_distances++) = (movement_distance > jerk_distance) ? 0 : jerk_distance - movement_distance;

	}

}


/*
 * update_speeds : this function determines each actuator's speed after the last computed movement;
 *
 * 	It also determines deceleration distances for new speeds, and require a deceleration if it is too high;
 */

void kinematics_update_actuators_speeds(kinematics_t *const kinematics, const mbuilder_t *const builder) {

	//Cache var for machine_control distances;
	const float *const actuation_distances = builder->actuation_distances;

	//Only the inverse of the duration is used, computed now for optimisation purposes.
	float inv_duration = (float) 1 / kinematics->previous_movement_duration;

	//Cache the speeds pointer;
	float *const actuators_speeds = kinematics->actuators_speeds;

	//Cache pointer to jerk distances;
	uint32_t *jerk_distances = kinematics->jerk_distances;

	//Cache pointer to jerk offsets;
	uint32_t *jerk_offsets = kinematics->jerk_offsets;

	//Cache the integration signature;
	sig_t integration_method_signature = kinematics->stop_distance_integration_enabled;

	//Declare and initialise the stop distance data struct;
	kinematics_stop_distance_data_t data = {
		.actuator = builder->dimension,
	};

	//Cache the array of integrated stop distances.
	float *stop_distances = kinematics->stop_distances;

	//For each actuator :
	for (; data.actuator--;) {

		//Cache the previous regulation speed;
		data.previous_actuator_speed = actuators_speeds[data.actuator];

		//Compute the regulation_speed for the actuator
		//Formula : v = d / t = d * (1 / t).
		data.actuator_speed = actuators_speeds[data.actuator] = actuation_distances[data.actuator] * inv_duration;

		//Save the stop distance pointer;
		data.stop_distance = stop_distances + data.actuator;

		//Determine if we must use the intregration method or not;
		bool use_integration_method = (bool) ((uint8_t) integration_method_signature & (uint8_t) 1);

		//Update the integration signature to match the next actuator;
		integration_method_signature >>= 1;

		//If the integration method must be used for this actuator :
		if (use_integration_method) {

			//Update the integrated stop distance with the new speed change;
			update_integrated_stop_distance(kinematics, &data);

		} else {
			//If the direct calculation method must be used for this actuator :

			//As this method is a direct computation (and is always used for this axis), no computation is required;
			if (kinematics->deceleration_required) {

				//Mark the axis inactivity;
				stop_distances[data.actuator] = -1;

				//No computation is required;
				continue;

			}

			//Compute the deceleration integrated_stop_distance on the actuator
			compute_stop_distance(kinematics, &data);
		}


		//Cache the distance to the jerk point;
		uint32_t jerk_distance = jerk_distances[data.actuator];

		//If we are in the deceleration radius :
		if ((uint32_t) (*data.stop_distance) > jerk_distance + jerk_offsets[data.actuator]) {

			//Require a deceleration during the next movement;
			kinematics->deceleration_required = true;

		}

	}
}
