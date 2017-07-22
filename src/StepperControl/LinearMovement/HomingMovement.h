/*
  HomingMotion.h - Part of TRACER

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

#include "../../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_HOMINGMOTION_H
#define TRACER_HOMINGMOTION_H


#include <stdbool.h>
#include "../MovementScheduler.h"

class HomingMovement {
public:

    static void move();

    static sig_t readEndStops();

private:

    static uint32_t getMaxDelay(sig_t signature, uint32_t *delays);
};

#endif //TRACER_HOMINGMOTION_H

#endif