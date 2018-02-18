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
#include <EEPROM/EEPROMTree.h>
#include <EEPROM/EEPROMMap.h>

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

    float error_r, error_t;


    //If both target and current phi.s are in zone 1 :
    if (target_in_zone_1 && (current_phi > (float) M_PI_2)) {

        //The corrected angle;
        float corrected_phi = ((float) M_PI - target_phi);

        //Get the semi error on the r axis;
        error_r = semi_error_r_1 - std::cos(target_theta) * corrected_phi;

        //Get the semi error on the t axis;
        error_t = semi_error_t_1 - std::sin(target_theta) * corrected_phi;

    } else {
        //If almost one phi is in zone 0 :

        //Get the semi error on the r axis;
        error_r = semi_error_r_0 - std::cos(target_theta) * target_phi;

        //Get the semi error on the t axis;
        error_t = semi_error_t_0 - std::sin(target_theta) * target_phi;

    }

    //Feed the r PID with the r error;
    vector->r = pid_r->compute(error_r);

    //Feed the t PID with the t error;
    vector->t = pid_t->compute(error_t);

}


/*
 * computeSemiErrors : this function determines semi-error associates to the current target;
 */

void OrientationController::computeSemiErrors() {

    //Update the zone :
    target_in_zone_1 = (target_phi > (float) M_PI_2);

    //Semi errors for case 0 :

    //Get the semi error on the r axis;
    semi_error_r_0 = std::cos(target_theta) * target_phi;

    //Get the semi error on the t axis;
    semi_error_t_0 = std::sin(target_theta) * target_phi;


    //Semi errors for case 1, only if phi is in zone 1
    if (target_in_zone_1) {

        //The corrected angle;
        float corrected_phi = ((float) M_PI - target_phi);

        //Get the semi error on the r axis;
        semi_error_r_1 = std::cos(target_theta) * corrected_phi;

        //Get the semi error on the t axis;
        semi_error_t_1 = std::sin(target_theta) * corrected_phi;

    }

}


/*
 * EEPROMRegister : this function registers the two PIDs in the EEPROMMap;
 */

void OrientationController::EEPROMRegister(char *path, const char *name) {

    /*

    //Create the parent
    EEPROMTree *tree = new EEPROMTree(new String(name), nullptr);

    //Create sons;
    tree->addChild(new EEPROMTree(new String("kp"), &kp));
    tree->addChild(new EEPROMTree(new String("ki"), &ki));
    tree->addChild(new EEPROMTree(new String("kd"), &kd));

    //Register the tree in the EEPROMMap;
    EEPROMMap::add_branch(path, tree);

     */

}
