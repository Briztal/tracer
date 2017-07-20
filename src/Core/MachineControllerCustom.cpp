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


#include "../StepperControl/MotionScheduler.h"
#include "../StepperControl/LinearMotionN/LinearMotionN.h"
#include "../StepperControl/LinearMotionN/HomingMotion.h"


void MachineController::action(char * dptr, unsigned char size) {


    /*
     SIM_SCGC6 |= SIM_SCGC6_PIT;
    __asm__ volatile("nop"); // solves timing problem on Teensy 3.5
    PIT_MCR = 1;

    CI::echo(String(TICS_PER_MS));
    CI::echo(String(US_TIMER_MAX_PERIOD));
    unsigned long period_us = 30000;
    CI::echo(String((period_us > US_TIMER_MAX_PERIOD) ?\
        US_TIMER_MAX_PERIOD :  (TICS_PER_MS) * period_us - 1));

    unsigned long l = (unsigned long)PIT_LDVAL0;

    CI::echo(String(l));

    while(true) {
        digitalWrite(13, !digitalRead(13));
        delay(1000);
    }

     */
    //set_stepper_int_period(30000);
    //CI::echo(String(PIT_LDVAL0));
    /*CI::echo("SUUS");

    float f = 250;

    CI::echo("float : "+String(f));

    char t[4];

    char * pc = (char*) &f;

    for (int i = 0; i<4; i++) {
        CI::echo("char "+String(i)+" "+String(pc[i]));
        t[i] = pc[i];
    }

    float *pf = (float*) pc;

    CI::echo("org "+String(*pf));

    CI::echo("new "+String(*(float*)t));


    */

    CI::echo("400");
    float coords[NB_STEPPERS];
    coords[0] = 160;
    coords[1] = 100;
    coords[2] = 0;
    coords[3] = 0;//40;
    //MotionScheduler::set_speed_for_group(0, 300);
    MotionScheduler::set_speed_for_group(1, 3);
    MotionScheduler::set_speed_group(1);

    //TODO TESTER AVEC LES Z

    LinearMotionN::prepare_motion(coords);

    CI::echo("EXIT");

}

void MachineController::home(char * dptr, unsigned char size) {

    HomingMotion::move();

}



