//
// Created by root on 7/4/18.
//

#ifndef TRACER_MACHINE_H
#define TRACER_MACHINE_H

#include <stdbool.h>

#include <stdint.h>

#include "geometry.h"


//------------------------------------------- Machine constants -------------------------------------------

/*
 * The machine constants structure. Contains constant machine parameters;
 */

typedef struct {

	//The machine's dimension;
	const uint8_t dimension;

	//The machine's geometry;
	const geometry_t *const geometry;

} machine_constants_t;


//------------------------------------------- Machine state -------------------------------------------

/*
 * A machine state contains all data that describe the real time state of the machine;
 *
 * 	It comprises a mandatory part, that presents the actuation layer's current state, and a custom part,
 * 	that is left to the controller's implementation;
 */

typedef struct {

	//Actuation positions. Actuators commanded on an integer base;
	int32_t *actuation_positions;

	//Actuation speeds. Time is expressed in float, so are speeds;
	float *actuation_speeds;

	//The controller's state; Can contain whatever the controller decides;
	void *controller_state;

} machine_state_t;


//------------------------------------------- Time interval -------------------------------------------

/*
 * To describe durations, we here define time interval type;
 */

typedef struct {

	//The minimum. Exists because the interval is closed; The minimum always exists, as duration is positive;
	float min;

	//The maximum. Exists because the interval is closed;
	float max;

	//The maximum doesn't always exist. This flag is set when it actually does;
	bool max_exists;

	//A useful flag that contains the value of "min == max";
	bool singleton;

	//A useful flag that contains the value of "min > max";
	bool empty;

} time_interval_t;



#endif //TRACER_MACHINE_H
