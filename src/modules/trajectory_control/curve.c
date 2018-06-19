//
// Created by root on 6/19/18.
//

#ifndef TRACER_EQUATION_H
#define TRACER_EQUATION_H


#include "curve.h"

//Evaluate the curve in one point and translate the result in control coordinates;
void curve_evaluate_convert(const curve_t *curve, uint8_t dimension, const geometry_t *geometry, float index,
							float *dest) {

	//The temporary control position;
	float temp[dimension];

	//Evaluate the curve and store the result in the temporary array;
	curve_evaluate(curve, index, temp);

	//Translate the geometry;
	geometry_convert(geometry, temp, dest);

}


#endif //TRACER_EQUATION_H
