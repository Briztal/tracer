/*
  HomingMotion.cpp - Part of TRACER

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

#include "../../../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#include "../../../config.h"
#include "../../../Core/EEPROMStorage.h"
#include "HomingMovement.h"
#include "../../StepperController.h"
#include "../SpeedPlanner.h"
#include "../MovementExecuter.h"

#define step 10

void HomingMovement::move() {

    //Enable all steppers
    StepperController::enable(255);

    //First step is to generate the signature for all axis;
    sig_t signature = 0;

    //TODO DIRECTIONS : 0 quand C'est en increment... Ca va pas !!

#define STEPPER(i, sig, rel, ...) \
        if (!rel) {\
            signature|=sig;\
            StepperController::setDir##i(true);\
            StepperController::setDir##i(false);\
        }

#include "../../../config.h"

#undef STEPPER


    uint32_t delay = 100;//TODO AJUSTER LE DELAY EN FONCTION DE LA VITESSE DES AXES RESTANTS
    uint32_t timeline = micros();

    uint32_t delays[NB_STEPPERS];
    float speed;
    uint32_t d;
    for (int axis = 0; axis < NB_STEPPERS; axis++) {
        speed = min(EEPROMStorage::maximum_speeds[axis], EEPROMStorage::accelerations[axis] * 0.05);
        d = (uint32_t) (1000000 / (speed * EEPROMStorage::steps[axis]));
        delays[axis] = delay;
        delay = max(delay, d);
    }

    while (signature) {
        for (int s = 0; s < step - 1; s++) {
            StepperController::fastStep(signature);
            timeline += delay;
            while (micros() < timeline) {}
        }

        StepperController::fastStep(signature);

        signature = readEndStops();
        delay = getMaxDelay(signature, delays);
        timeline += delay;
        while (micros() < timeline) {}
    }

    for (int axis = 0; axis < NB_STEPPERS; axis++) {
        SpeedPlanner::positions[axis] = 0;
    }

    SpeedPlanner::send_position();

}

uint32_t HomingMovement::getMaxDelay(sig_t signature, uint32_t *delays) {
    uint32_t delay = 0;
    int axis = 0;
    for (; axis < NB_STEPPERS; axis++) {
        if (signature & (uint32_t) 1) {
            delay = delays[axis];
            signature >>= 1;
            axis++;
            break;
        }
        signature >>= 1;
    }
    for (; axis < NB_STEPPERS; axis++) {
        if (signature & (uint32_t) 1) {
            delay = max(delay, delays[axis]);
        }
        signature >>= 1;
    }
    return delay;
}

sig_t HomingMovement::readEndStops() {
    sig_t signature = 0;
    sig_t bit = 1;

#define STEPPER(i, sig, rel, ps, pd, dp, pp, ve, pinEndMin, minValue, pma, va)\
    if (!rel) {\
        if ((bool)digital_read(pinEndMin) == minValue) {\
            signature|=sig;\
        }\
    }

#include "../../../config.h"

#undef STEPPER

    return signature;

}


#endif