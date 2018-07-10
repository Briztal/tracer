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
 * 	The geometry is the relation between control coordinates and actuation coordinates.
 *
 * 	Movement and speed regulation are based on the high level coordinate system;
 *
 * 	Speed limitation, speed planning or jerk limitation are based on low level coordinates;
 */

/*
 * Each geometry implementation will be composed of the basic geometry struct, that only comprises
 * 	a pointer to the conversion function;
 */
typedef struct geometry_t{

	//A flag, determining if the geometry is regular, ie it transforms lines into lines;
	//If the geometry is not regular, extra computation will be required for distances determination;
	bool regular;

	//The pointer to the function used to convert control coordinates to actuation coordinates;
	void (*conversion)(const struct geometry_t *geometry, const float *control_coords, float *actuation_coords);

	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;
	//TODO NOP ! MAKE AN INTERFACE LINKABLE TO PROCESS QUEUE;

} geometry_t;



//To ease the redaction, a static function to directly execute the translation function;
inline void geometry_convert(const geometry_t *const geometry, const float *const control_coords, float *actuation_coords) {

	//Call the geometry conversion providing geometry data and the couple of coordinate sets:
	(*(geometry->conversion))(geometry, control_coords, actuation_coords);

}

#endif //TRACER_GEOMETRY_H
