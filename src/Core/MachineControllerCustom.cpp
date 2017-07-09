/*
  MachineControllerCustom.h - Part of TRACER

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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/



#include "MachineControllerSystem.h"
#include "../Interfaces/CommandInterface.h"
#include "../Actions/ContinuousActions.h"


#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#include "../StepperControl/MotionScheduler.h"
#include "../StepperControl/LinearMotionN/LinearMotionN.h"
#include "../StepperControl/LinearMotionN/HomingMotion.h"

#endif

void MachineController::action(char * dptr, unsigned char size) {

    CI::echo("400");
    float coords[NB_STEPPERS];
    coords[0] = 160;
    coords[1] = 100;
    coords[2] = 10;
    coords[3] = 40;
    MotionScheduler::setSpeed(300);
    LinearMotionN::prepare_motion(coords);
    delay(1000);

    CI::echo("EXIT");

}

void MachineController::home(char * dptr, unsigned char size) {

    HomingMotion::move();

}



