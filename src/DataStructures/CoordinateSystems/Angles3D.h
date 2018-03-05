//
// Created by root on 3/4/18.
//

#ifndef TRACER_ANGLES3D_H
#define TRACER_ANGLES3D_H


#include "Triplet.h"

class Angles3D : public Triplet {

public:

    //Basic constructor;
    Angles3D() : Triplet(){}

    //Value constructor;
    Angles3D(float v0, float v1, float v2) : Triplet(v0, v1, v2) {};

    //Copy constructor;
    Angles3D(const Angles3D &src) : Triplet(src) {};


public:

    //Modify an angle's value;
    void set(uint8_t index, float value) override;

private:

    //Set all angles in ]-pi; pi]
    void wrapAngles();

    //Set an angle in ]pi; pi]
    float wrapAngle(float angle);



};


#endif //TRACER_ANGLES3D_H
