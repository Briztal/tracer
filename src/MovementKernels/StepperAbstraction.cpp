/*
  StepperAbstraction.cpp - Part of TRACER

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

#include <stdint.h>
#include "StepperAbstraction.h"
#include "../Core/EEPROMStorage.h"


void StepperAbstraction::translate(const float *const hl_coordinates, long *const steppers_coordinates) {

    for (uint8_t axis = 0; axis<NB_STEPPERS; axis++) {
        steppers_coordinates[axis] = (long) (EEPROMStorage::steps[axis] * hl_coordinates[axis]);
    }

}

void StepperAbstraction::invert(const long *const steppers_coordinates, float *const hl_coordinates) {

    for (uint8_t axis = 0; axis<NB_STEPPERS; axis++) {
        hl_coordinates[axis] = (float) (steppers_coordinates[axis]) / EEPROMStorage::steps[axis];
    }

}

