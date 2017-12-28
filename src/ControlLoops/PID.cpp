/*
  PID.cpp - Part of TRACER

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

#include <EEPROM/EEPROMTree.h>
#include <EEPROM/EEPROM.h>
#include "PID.h"

/*
 * Default constructor : initialises everything to zero;
 */

PID::PID() : last(0), sum(0), kp(0), ki(0), kd(0) {}


/*
 * Constructor : initialises last and sum;
 */

PID::PID(float kp, float ki, float kd) : last(0), sum(0), kp(kp), ki(ki), kd(kd) {}


/*
 * Destructor : no dynamic arguments to delete;
 */

PID::~PID() = default;


/*
 * reset : this function resets the GENERATE_PID;
 */

void PID::reset() {

    //Clear the last error;
    last = 0;

    //Clear the error sum;
    sum = 0;
}

/*
 * set_constants : this function updates the kp, ki and kd constants;
 */

void PID::set_constants(float kp, float ki, float kd) {

    //Update the kp;
    this->kp = kp;

    //Update the ki;
    this->ki = ki;

    //Update the kd;
    this->kd = kd;
}


/*
 * getConstants : this function
 */

PID::PID_constants_t PID::getConstants() {

    //Initialise a struct;
    PID::PID_constants_t constants =  PID::PID_constants_t();

    //Update kp, ki, and kd;
    constants.kp = &kp, constants.ki = &ki, constants.kd = &kd;

    //Return the created struct;
    return constants;

}


/*
 * EEPROMRegister : this function registers the PID in the EEPROM;
 */

void PID::EEPROMRegister(char *path, const char *name) {

    //Create the parent
    EEPROMTree *tree = new EEPROMTree(new String(name), nullptr);

    //Create sons;
    tree->addChild(new EEPROMTree(new String("kp"), &kp));
    tree->addChild(new EEPROMTree(new String("ki"), &ki));
    tree->addChild(new EEPROMTree(new String("kd"), &kd));

    //Register the tree in the EEPROM;
    EEPROM::add_branch(path, tree);

}


/*
 * compute : this function computes the output for the provided error;
 */

float PID::compute(float error) {

    //Update the sum, and cache the last error;
    float s = (sum+=error), l = last;

    //Determine the output value;
    float ret = kp * error + ki * s + kd * (error - l);

    //Update the last error;
    last = error;

    //Return the output value;
    return ret;

}
