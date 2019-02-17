//
// Created by root on 10/1/18.
//

#ifndef TRACER_GEOMETRY_H
#define TRACER_GEOMETRY_H


#include "stdbool.h"

#include "stdint.h"

/*
 * The geometric_model is the relation between control coordinates and actuation coordinates.
 *
 *	It achieves conversion in both directions;
 *
 * Each geometric_model implementation will be composed of the basic geometric_model struct, that only comprises
 * 	a pointer to the conversion function;
 */

struct geometric_model {
	
	//The pointer to the function used to convert control position to actuation position;
	void (*const control_to_actuation)(
		const float *const control,
		int32_t *const actuation);
	
	//The pointer to the function used to convert actuation position to control position;
	void (*const actuation_to_control)(
		const int32_t *const actuation,
		float *const control);
	
};


#endif //TRACER_GEOMETRY_H
