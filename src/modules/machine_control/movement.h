//
// Created by root on 6/22/18.
//

#ifndef TRACER_MOVEMENT_H
#define TRACER_MOVEMENT_H

#include <stdint.h>


//------------------------------------------------------ Signature -----------------------------------------------------

//To describe boolean qualifications over an array of elements, we define the signature boolean type;
typedef uint32_t sig_t;


//------------------------------------------------------ Movement ------------------------------------------------------

/*
 * A movement is represented by a set of coordinates and a time to complete;
 */

typedef struct {
	//The distances coordinates array;
	int16_t *actuation_distances;

	//The number of microseconds till position must be reached;
	float time_to_dest;

} movement_t;

