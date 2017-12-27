/*
  OrientationController.cpp - Part of TRACER

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


#include <cmath>

#include "OrientationController.h"


/*
 * Constructor : initialises target position;
 */

OrientationController::OrientationController() : target_theta(0), target_phi(0), pid_r(new PID()), pid_t(new PID()) {}


/*
 * Destructor : delete the two PIDs;
 */

OrientationController::~OrientationController() {

    //Delete the r PID;
    delete pid_r;

    //Delete;
    delete pid_t;
}


/*
 * reset : resets all data at initialisation state;
 */

void OrientationController::reset() {

    //Reset targets;
    target_phi = target_theta = 0;

    //Reset semi_errors;
    computeSemiErrors();

}


/*
 * setTaget : updates the target position with the provided values;
 */

void OrientationController::setTaget(float target_theta, float target_phi) {

    //Update the theta coordinate;
    this->target_theta = target_theta;

    //Update the theta coordinate;
    this->target_phi = target_phi;

}


/*
 * getCorrectionVector : computes the correction vector corresponding to the current position;
 */

void OrientationController::getCorrectionVector(float current_theta, float current_phi, rotation_vector *vector) {

    //Get the semi error on the r axis;
    float error_r = semi_error_r - std::cos(target_theta) * target_phi;

    //Get the semi error on the t axis;
    float error_t = semi_error_t - std::sin(target_theta) * target_phi;

    //Feed the r StaticPID with the r error;
   vector->r = pid_r->compute(error_r);

    //Feed the t StaticPID with the t error;
    vector->t = pid_t->compute(error_t);

}


/*
 * computeSemiErrors : this function determines semi-error associates to the current target;
 */

void OrientationController::computeSemiErrors() {

    //Get the semi error on the r axis;
    semi_error_r = std::cos(target_theta) * target_phi;

    //Get the semi error on the t axis;
    semi_error_t = std::sin(target_theta) * target_phi;

}
