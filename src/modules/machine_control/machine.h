//
// Created by root on 7/3/18.
//

#ifndef TRACER_MACHINE_H
#define TRACER_MACHINE_H


#include "modules/machine_control/actuation.h"

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


/*
 * A machine contains, an actuation layer, a geometry, and a controller;
 */

typedef struct {

	//The machine's actuation layer;
	actuation_t *const actuation;

	//The machine's geometry;
	const geometry_t *const geometry;

	//The machine's controller;
	mcontroller_t *controller;

};

#endif //TRACER_MACHINE_H
