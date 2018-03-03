//
// Created by root on 3/3/18.
//

#include "Vector3DToAngles.h"

#include "math.h"

void Vector3DToAngles::convert(Vector3D *vector, float *ax, float *ay, float *az) {

    *ax = atan2f(vector->z, vector->y);
    *ay = atan2f(vector->x, vector->z);
    *az = atan2f(vector->y, vector->x);

}

