/*
PID.h - Part of TRACER

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


#ifndef TRACER_PID_H
#define TRACER_PID_H

#include "stdint.h"

class PID {

    //--------------------------------------- Initialisation - Destruction ---------------------------------------

public:

    //Default constructor;
    PID();

    //Constructor;
    PID(float kp, float ki, float kd);

    //Destructor;
    ~PID();

    //Reset the GENERATE_PID values;
    void reset();

    //Update the regulation constants;
    void set_constants(float kp, float ki, float kd);


    //-------------------------------------------------- Computation --------------------------------------------------

public:

    //Compute the output value for the given error;
    float compute(float error);


    //-------------------------------------------------- Constants --------------------------------------------------

public:

    //A struct to contain constants pointers;
    struct PID_constants_t {
        float *kp, *ki, *kd;
    };

    //Get the constant pointers;
    PID_constants_t getConstants();

    //Register the PID in the EEPROM;
    void EEPROMRegister(char *path, const char *name);


    //-------------------------------------------------- Fields --------------------------------------------------

private:

    //Array for last measured errors.
    float last;

    //Array for sums.
    float sum;

    //Regulation constants;
    float kp, ki, kd;

};


#endif
