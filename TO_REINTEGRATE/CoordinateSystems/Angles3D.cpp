//
// Created by root on 3/4/18.
//


#include "Angles3D.h"

//#include <Math/math_constants.h>


void Angles3D::set(uint8_t index, float value) {

    //Call the parent function;
    Triplet::set(index, value);

    //Wrap angles;
    wrapAngles_PI();

}


/*
 * wrapAngles_PI : this function will wrap all angles in ]-pi; pi]
 */

void Angles3D::wrapAngles_PI() {

    //Wrap all angles;
    data[0] = wrapAngle_PI(data[0]);
    data[1] = wrapAngle_PI(data[1]);
    data[2] = wrapAngle_PI(data[2]);

}


/*
 * wrapAngles_PI : this function will wrap the angle in ]-pi; pi]
 */

float Angles3D::wrapAngle_PI(float angle) {

    /*
    //While the angle is below -pi, add 2pi;
    while(angle <= -(float)M_PI)
        angle += (float) M_2PI;

    //While the angle is beyond pi, subtract 2pi;
    while(angle > (float)M_PI)
        angle -= (float) M_2PI;

    //Return the new angle;
    return angle;
     */

}
