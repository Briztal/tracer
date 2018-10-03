//
// Created by root on 10/1/18.
//

#ifndef TRACER_CARTESIAN_H
#define TRACER_CARTESIAN_H

#include <algo/machine_control/geometry.h>


/*
 * A cartesian geometry has all its axis orthonormal;
 *
 * The conversion between control and actuation coordinate is achieved with a multiplication on each axis;
 */

struct cartesian_geometry {
	
	//A geometric model base, for pointer compatibility;
	struct geometric_model geometry;
	
	//Conversion factors can be modified. They can't be set to 0;
	void (*const update_conversion_factor)(uint8_t index, float conversion_factor);
	
};

#endif //TRACER_CARTESIAN_H
