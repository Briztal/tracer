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


#include "../interface.h"

#ifdef ENABLE_GCODE_INTERFACE

void MachineController::g0() {
    CI::echo("SUUSg0");
}

void MachineController::g1() {
    CI::echo("SUUSg1");
}
void MachineController::g10() {
    CI::echo("SUUSg10");
}
void MachineController::g5d0() {
    CI::echo("SUUSg5d0");
}
void MachineController::m1() {
    CI::echo("SUUSm1");
}
void MachineController::m2() {
    CI::echo("SUUSm2");
}
void MachineController::m225() {
    CI::echo("SUUSm225");
}



#endif

#ifdef ENABLE_COMMAND_INTERFACE


#include "../config.h"


#include "../Actions/ContinuousActions.h"
#include "../StepperControl/MovementScheduler.h"
#include "../StepperControl/LinearMovement/LinearMovement.h"
#include "../StepperControl/LinearMovement/HomingMovement.h"
#include "../StepperControl/SpeedManager.h"
#include "../StepperControl/MovementExecuter.h"

void MachineController::action(char * dptr, uint8_t size) {

    CI::echo("400");
    SpeedManager::print_speed_distance();
    float coords[NB_STEPPERS]{0};
    coords[0] = 100;
    coords[1] = 160;
    coords[2] = 100;
    /*coords[3] = 40;
    coords[4] = 50;
    coords[5] = 138;
    coords[6] = 82;
    coords[7] = 43;
    coords[8] = 120;
    coords[9] = 12;
    coords[10] = 152;
    coords[11] = 90;
    coords[12] = 14;
    coords[13] = 142;
    coords[14] = 160;
    coords[15] = 35;
    coords[16] = 100;*/
    ContinuousActions::setLinearPower0(1);
    MovementScheduler::set_speed_for_group(0, 500);
    MovementScheduler::set_speed_group(0);

    //TODO TESTER AVEC LES Z

    LinearMovement::prepare_motion(coords);
    /*

    delay(20);

    coords[0] = 40;
    coords[1] = 110;
    coords[2] = 40;
    coords[3] = 110;
    coords[4] = 40;
    coords[5] = 110;
    coords[6] = 40;
    coords[7] = 110;
    coords[8] = 40;
    coords[9] = 110;
    coords[10] = 40;
    coords[11] = 110;
    coords[12] = 40;
    coords[13] = 110;
    coords[14] = 40;
    coords[15] = 110;
    coords[16] = 40;
    LinearMovement::prepare_motion(coords);

    delay(20);

    MovementExecuter::enqueue_homing_movement();

    delay(20);

    coords[0] = 75;
    coords[1] = 75;
    coords[2] = 75;
    coords[3] = 75;
    coords[4] = 75;
    coords[5] = 75;
    coords[6] = 75;
    coords[7] = 75;
    coords[8] = 75;
    coords[9] = 75;
    coords[10] = 75;
    coords[11] = 75;
    coords[12] = 75;
    coords[13] = 75;
    coords[14] = 75;
    coords[15] = 75;
    coords[16] = 75;
    LinearMovement::prepare_motion(coords);
     */

    CI::echo("EXIT");

}

void MachineController::home(char * dptr, uint8_t size) {

    HomingMovement::move();

}

#endif



