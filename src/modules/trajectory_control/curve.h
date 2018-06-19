//
// Created by root on 6/19/18.
//

#ifndef TRACER_CURVE_H
#define TRACER_CURVE_H


#include "stdint.h"

#include "geometry.h"

//The trajectory function type; Receives a movement data and an index and updates the positions array;
typedef void (*curve_function_t)(struct movement_t *movement, float index, float *positions);


/*
 * The parametric equation struct;
 */
typedef struct {

	//The trajectory function : it takes a real value (the index) and outputs a position;
	const curve_function_t function;

	//The initial value of the trajectory index;
	const float initial_index;

	//The final value of the trajectory index;
	const float final_index;

} curve_t;


//Get a particular point of the curve;
static void curve_evaluate(const curve_t *curve, const float index, float *const dest) {

	//Get the function, and call it with all args;
	(*(curve->function))(curve, index, dest);

}

//Evaluate the curve in one point and translate the result in control coordinates;
void curve_evaluate_convert(const curve_t *curve, uint8_t dimension, const geometry_t *geometry, float index,
							float *dest);



#endif //TRACER_CURVE_H
