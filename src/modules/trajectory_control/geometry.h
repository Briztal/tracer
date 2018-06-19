//
// Created by root on 6/19/18.
//

#ifndef TRACER_GEOMETRY_H
#define TRACER_GEOMETRY_H

/*
 * This library defines the geometry type;
 *
 * 	The geometry is the relation between control coordinates and actuation coordinates.
 *
 * 	Movement and speed regulation are based on the high level coordinate system;
 *
 * 	Speed limitation, speed planning or jerk limitation are based on low level coordinates;
 */

/*
 * The geometry data : each geometry implementation will be composed of the basic geometry struct, that only comprises
 * 	a pointer to the conversion function;
 */
typedef struct geometry_t{

	//The pointer to the function used to convert control coordinates to actuation coordinates;
	void (*conversion)(const struct geometry_t *geometry, const float *control_coords, float *actuation_coords);

} geometry_t;


//To ease the redaction, a static function to directly execute the translation function;
inline void geometry_convert(const geometry_t *const geometry, const float *const control_coords, float *actuation_coords) {

	//Call the geometry conversion providing geometry data and the couple of coordinate sets:
	(*(geometry->conversion))(geometry, control_coords, actuation_coords);

}

#endif //TRACER_GEOMETRY_H
