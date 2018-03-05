//
// Created by root on 3/3/18.
//

#include "Vector3DToAngles.h"

#include "math.h"

void Vector3DToAngles::convert(Vector3D &vector, Angles3D &angles) {

    const float *const v = vector.get_data();

    angles.set(0, atan2f(v[2], v[1]));
    angles.set(1, atan2f(v[0], v[2]));
    angles.set(2, atan2f(v[1], v[0]));

}

