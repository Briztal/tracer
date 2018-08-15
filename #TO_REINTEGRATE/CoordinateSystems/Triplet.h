//
// Created by root on 3/2/18.
//

#ifndef TRACER_TRIPLET_H
#define TRACER_TRIPLET_H

#include "stdint.h"


class Triplet {
    
    //--------------------------- Initialisation ---------------------------

public:

    //Basic constructor;
    Triplet();

    //Value constructor;
    Triplet(float v0, float v1, float v2);

    //Copy constructor;
    Triplet(const Triplet &src);

    virtual //Default destructor;
    ~Triplet() = default;

    //Clone the given vector;
    void clone(const Triplet &src);

    virtual //Reset the triplet;
    void reset();


    //---------------- Data operations ----------------

public:

    //Modify a coordinate's value;
    virtual void set(uint8_t index, float value);

    //Get a coordinate's value;
    float get(uint8_t index) const;

    //Get the array;
    virtual const float *const get_data() const;


    //--------------------------- Data ---------------------------

protected:
    
    //The triplet of angle;
    float data[3]{0};

    //--------------------------- toString ---------------------------

public:

    //tstring toString();

};


#endif //TRACER_TRIPLET_H
