//
// Created by root on 3/1/18.
//

#ifndef TRACER_ROTATION_DATA_H
#define TRACER_ROTATION_DATA_H


#include "DataStructures/CoordinateSystems/Vector3D.h"

/*
 * This structure contains all data that characterise a rotation in space :
 *
 *  - the rotation vector, a 3D normalised vector that directs the rotation axis;
 *  - the rotation angle, directed positively with the rotation vector;
 */

typedef struct {

    //The rotation angle;
    float rotation_angle = 0;

    //The rotation vector;
    Vector3D rotation_vector = Vector3D(0,0,0);

} rotation_data_t;

#endif //TRACER_ROTATION_DATA_H
