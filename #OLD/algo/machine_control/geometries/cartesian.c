//
// Created by root on 10/1/18.
//


//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * cortex_m4f.mk must provide :
 * 	- NB_AXIS : the number of axis in the geometry
 * 	- NB_AXIS : the geometry struct's required name;
 */

//If one of the macro was not provided :

#if !defined(NB_AXIS) || !defined(NAME)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define NB_AXIS 5

#define NAME core_instance

#endif


//--------------------------------------------------- Includes --------------------------------------------------

#include <util/macro/incr_call.h>
#include <stdio.h>
#include "cartesian.h"


//--------------------------------------------------- Headers --------------------------------------------------

//Conversion factor update;
static void update_conversion_factor(uint8_t index, float control_to_actuation);

//Coordinate translation;
static void control_to_actuation(const float *control, int32_t *actuation);
static void actuation_to_control(const int32_t *actuation, float *control);



//--------------------------------------------------- Data --------------------------------------------------

#define INIT_1(x) 1,


//All conversion factors will be initialised to 1;
static float c_a_factors[NB_AXIS] = {INCR_CALL(NB_AXIS, INIT_1)};
static float a_c_factors[NB_AXIS] = {INCR_CALL(NB_AXIS, INIT_1)};


//Define and initialise the geometry structure;
struct cartesian_geometry NAME = {
	
	//Initialise the geometry base;
	.geometry = {
		
		//Transmit functions;
		.actuation_to_control = &actuation_to_control,
		.control_to_actuation = &control_to_actuation,
		
	},
	
	//Transmit the setup function;
	.update_conversion_factor = update_conversion_factor,

};


//--------------------------------------------------- Functions --------------------------------------------------


FILE
/**
 * cartesian_geometry_update_conversion_factor : checks that the index is valid, resets the conversion factor to 1
 * 	if it is null, and updates the conversion factor in the array;
 *
 * @param geometry : the cartesian geometry whose conversion factor must be modified;
 * @param index : the index of the conversion factor to modify;
 * @param control_to_actuation : the conversion factor from control coordinate to actuation coordinate;
 */

static void update_conversion_factor(uint8_t index, float control_to_actuation) {
	
	//If the index is invalid :
	if (index >= NB_AXIS) {
		
		//Do nothing;
		return;
		
	}
	
	//Declare the inverse of the provided factor
	float actuation_to_control;
	
	//If the conversion factor is null :
	if (!control_to_actuation) {
		
		//Reset the conversion factor to 1;
		control_to_actuation = actuation_to_control = 1;
		
	} else {
		
		//If the factor is not null, take its inverse;
		actuation_to_control = 1 / control_to_actuation;
		
	}
	
	//Update conversion factors;
	c_a_factors[index] = control_to_actuation;
	a_c_factors[index] = actuation_to_control;
	
	
}


/**
 * control_to_actuation : translates control coordinates to actuation coordinates;
 *
 * @param control : control coordinates;
 * @param actuation : actuation coordinates;
 */

static void control_to_actuation(const float *control, int32_t *actuation) {
	
	//TODO VECTOR MULTIPLICATION;
	
	//Cache the conversion factor array;
	const float *factors = c_a_factors;
	
	//For each axis :
	for (uint8_t i = 0; i < NB_AXIS; i++) {
		
		//Multiply the conversion factor and the control coordinate, convert to uint32_t and store;
		*(actuation++) = (int32_t) (*(factors) * *(control++));
		
	}
	
}


/**
 * actuation_to_control : translates actuation coordinates to control coordinates;
 *
 * @param actuation : actuation coordinates;
 * @param control : control coordinates;
 */

static void actuation_to_control(const int32_t *actuation, float *control) {
	
	//TODO VECTOR DIVISION;
	
	//Cache the conversion factor array;
	const float *factors = a_c_factors;
	
	//For each axis :
	for (uint8_t i = 0; i < NB_AXIS; i++) {
		
		//Multiply the conversion factor and the control coordinate, convert to uint32_t and store;
		*(control++) = (*(factors) * (float) *(actuation++));
		
	}
	
}



