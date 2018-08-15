/*
  geometry.h Part of TRACER

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

#ifndef TRACER_GEOMETRY_H
#define TRACER_GEOMETRY_H


/*
 * The geometry is the relation between control coordinates and actuation coordinates.
 *
 *	It achieved conversion in both dimensions;
 */

/*
 * In order to convert control position into actuation position following arguments are required :
 */

typedef struct {

	//The geometry instance, const;
	const void *const instance;

	//The control coordinates array, constant;
	const float *const control_positions;

	//The actuation coordinates array, mutable;
	int32_t *const actuation_positions;

} control_to_actuation_args;


/*
 * In order to convert actuation position into control position following arguments are required :
 */

typedef struct {

	//The geometry instance, const;
	const void *const instance;

	//The actuation coordinates array, mutable;
	const int32_t *actuation_positions;

	//The control coordinates array, mutable;
	float *const control_positions;

} actuation_to_control_args;


/*
 * Each geometry implementation will be composed of the basic geometry struct, that only comprises
 * 	a pointer to the conversion function;
 */

typedef struct geometry_t{

	//The geometry data instance;
	void *instance;

	//A flag, determining if the geometry is regular, ie it transforms lines into lines;
	//If the geometry is not regular, extra computation will be required for distances determination;
	bool regular;

	//The pointer to the function used to convert control position to actuation position;
	void (*const control_to_actuation)(control_to_actuation_args *args);

	//The pointer to the function used to convert actuation position to control position;
	void (*const actuation_to_control)(control_to_actuation_args *args);

} geometry_t;


#endif //TRACER_GEOMETRY_H
