//
// Created by root on 3/4/18.
//

#include <math.h>

#include "Angles3D.h"


void Angles3D::set(uint8_t index, float value) {

    //Call the parent function;
    Triplet::set(index, value);

    //Wrap angles;
    wrapAngles();

}


/*
 * wrapAngles : this function will wrap all angles in ]-pi; pi]
 */

void Angles3D::wrapAngles() {

    //Wrap all angles;
    data[0] = wrapAngle(data[0]);
    data[1] = wrapAngle(data[1]);
    data[2] = wrapAngle(data[2]);

}


/*
 * wrapAngles : this function will wrap the angle in ]-pi; pi]
 */

float Angles3D::wrapAngle(float angle) {

    //While the angle is below -pi, add 2pi;
    while(angle <= -M_PI)
        angle += (float) M_2_PI;

    //While the angle is beyond pi, subtract 2pi;
    while(angle > M_PI)
        angle -= (float) M_2_PI;

    //Return the new angle;
    return angle;

}
