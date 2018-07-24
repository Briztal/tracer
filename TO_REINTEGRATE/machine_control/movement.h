/*
  movement.h Part of TRACER

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


#endif TRACER_MOVEMENT_H