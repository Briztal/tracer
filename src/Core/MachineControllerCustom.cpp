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
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/



#include "MachineControllerSystem.h"
#include "../Interfaces/CommandInterface.h"
#include "../Actions/ContinuousActions.h"


#include "../config.h"


#include "../StepperControl/MotionScheduler.h"
#include "../StepperControl/LinearMotionN/LinearMotionN.h"
#include "../StepperControl/LinearMotionN/HomingMotion.h"
#include "EEPROMStorage.h"


void MachineController::action(char * dptr, uint8_t size) {
    CI::echo("400");
    float coords[NB_STEPPERS];
    coords[0] = 100;
    coords[1] = 160;
    coords[2] = 100;
    coords[3] = 40;
    //MotionScheduler::set_speed_for_group(0, 300);
    MotionScheduler::set_speed_for_group(0, 500);
    MotionScheduler::set_speed_group(0);

    //TODO TESTER AVEC LES Z

    LinearMotionN::prepare_motion(coords);

    CI::echo("EXIT");

}

void MachineController::home(char * dptr, uint8_t size) {

    HomingMotion::move();

}



