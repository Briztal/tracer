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
#include "EEPROMStorage.h"
#include "../StepperControl/MotionScheduler.h"
#include "../StepperControl/LinearMotionN/LinearMotionN.h"
#include "../parameters_indices.h"
#include "../StepperControl/LinearMotionN/HomingMotion.h"

volatile bool b;
int bite;
void f() {
    if (!bite--) {
        b = false;
        disable_stepper_interrupt;
    }
}

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


void MachineController::setLinearPower(float linearPower) {
    MachineController::linearPower = linearPower;
}

void MachineController::setSpeed(float speed) {//TODO CLEAN
    if (speed < EEPROMStorage::maximum_speed) {
        MachineController::speed = speed;
        MotionScheduler::setSpeed(speed);
    } else {
        //TODO ALERT CANAL
        //CI::send_speed_exceeding(0);
    }
}

//----------------------------------------------DRAWING METHODS---------------------------------------------------------

//PRIMARIES

void MachineController::homeMachine() {
    //homingMotion.move();
}

void MachineController::absoluteMove() {
    //motionN.move();
}


void MachineController::ellipticMove() {
    /*if (motionE.prepare_motion()) {
        motionN.move();
        motionE.move();
    }*/
}

//SECONDARIES

void MachineController::relativeMove(float x, float y, float z) {
    //TODO REFAIRE absoluteMove(tab[0]/EEPROMStorage::stepsX+x, tab[1]/EEPROMStorage::stepsY+y, tab[2]/EEPROMStorage::stepsZ+z);

}

void MachineController::drawPolygon() {
    //TODO REFAIRE absoluteMove(curve_t[i][0], curve_t[i][1], curve_t[i][2]);
}

////////////////////////////////////////////////NOT SAFE YET////////////////////////////////////////////////////////////

void MachineController::bezierMove() {
    /*motionB.prepareMotion();
    motionN.move();
    motionB.move();*/

}

////////////////////////////////////////////////////PUNCTUAL_ACTIONS////////////////////////////////////////////////////

void MachineController::adjustLinearPower() {
    //ContinuousActions::set_power_for_speed_0(parameters[Vid]);
    CI::echo("modes :" + String(ContinuousActions::linear_modes_enabled()));
    CI::echo("linear powerAdjusted");
}

void MachineController::adjustSpeedPower() {
    //ContinuousActions::set_power_for_speed_0(parameters[Vid]);
    CI::echo(("modes :" + String(ContinuousActions::linear_modes_enabled())));
    CI::echo("Power by speed adjusted");
}


////////////////////////////////////////////////STATIC FIELDS DECLARATION///////////////////////////////////////////////
#define m MachineController

//POWER AND SPEED
float m::linearPower = 0.1; //Watt per Millimeter per second (W/(mm/s))
float m::power; //power ratio;
float m::speed = 100; //mm per second


/*
//MOTIONS
HomingMotion m::homingMotion = HomingMotion();
LinearMotionN m::motionN = LinearMotionN();
MotionBezier m::motionB = MotionBezier();
MotionEllipses m::motionE = MotionEllipses();

 */
float *m::parameters;
bool *m::v_parameters;



