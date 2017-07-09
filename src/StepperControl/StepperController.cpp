/*
  StepperController.cpp - Part of TRACER

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

#include <Arduino.h>
#include "StepperController.h"

#include "../External/digitalWriteFast.h"
#include "../Interfaces/CommandInterface.h"

#define PINS_REQUIRED
#define PARAMS_REQUIRED

#define STEPPER(i, sig, pinStep, pinDir, dp,  pinPower, ve, pinEndMin, vi, pinEndMax, va)\
unsigned int pinPort##i = *__digitalPinToPortReg(pinStep);\
unsigned int pinBit##i = __digitalPinToBit(pinStep);\
bool StepperController::isAtMax##i ()  {return (digitalReadFast(pinEndMax )==HIGH);}\
bool StepperController::isAtMin##i() { return (digitalReadFast(pinEndMin)==HIGH);}\


#include "../config.h"

#undef STEPPER;


void StepperController::enable(unsigned char signature) {
#define STEPPER(i, sig, dp, ps, pd, pinPower, ve, pmi, vi, pma, va) \
    if (signature&sig) {\
        digitalWriteFast(pinPower, LOW);\
    } else {\
        digitalWriteFast(pinPower, HIGH);\
    }

#include "../config.h"

#undef STEPPER;

}

//TODO PASSER EN SIGNATURES PLUTOT QU'EN ATTRIBUTS
void StepperController::set_directions(unsigned char negative_signatures) {
    bool sig_dir;
#ifdef position_log
#define STEPPER(i, sig, ps, pinDir, dirp,  pp, ve, pmi, vi, pma, va) \
    sig_dir = negative_signatures&sig;\
    if ((!sig_dir) && !dir##i) {\
        dir##i = true;\
        digitalWriteFast(pinDir, dirp);\
        incr##i=1;\
    } else if (sig_dir && (dir##i)) {\
        dir##i = false;\
        digitalWriteFast(pinDir, !dirp);\
        incr##i=-1;\
    }\


#else

#define STEPPER(i, sig, ps, pinDir, dirp,  pp, ve, pmi, vi, pma, va) \
    sig_dir = negative_signatures&sig;\
    if ((!sig_dir) && !dir##i) {\
        dir##i = true;\
        digitalWriteFast(pinDir, dirp);\
    } else if (sig_dir && (dir##i)) {\
        dir##i = false;\
        digitalWriteFast(pinDir, !dirp);\
    }\

#endif

#include "../config.h"

#undef STEPPER

}

#ifdef position_log
#define STEPPER(i, sig, ps, pinDir, dirp,  pp, ve, pmi, vi, pma, va) \
void StepperController::setDir##i(bool sens) {\
    CI::echo(String(i)+" "+String(sens));\
    if (!(sens ^ dir##i )) return;\
        dir##i = sens;\
        if (sens) {\
            digitalWriteFast(pinDir, dirp);\
            lim##i = max##i ;\
            incr##i=1;\
        } else {\
            digitalWriteFast(pinDir, !dirp);\
            lim##i = 0;\
            incr##i=-1;\
    }\
}

#include "../config.h"

#undef STEPPER;
#else
#define STEPPER(i, sig, ps, pinDir, dirp,  pp, ve, pmi, vi, pma, va) \
void StepperController::setDir##i(bool sens) {\
    if (!(sens ^ dir##i )) return;\
        dir##i = sens;\
        if (sens) {\
            digitalWriteFast(pinDir, dirp);\
            lim##i = max##i ;\
        } else {\
            digitalWriteFast(pinDir, !dirp);\
            lim##i = 0;\
    }\
}

#include "../config.h"

#undef STEPPER;
#endif

void StepperController::begin() {

    set_dimensions();

#define STEPPER(i, sig, pinStep, pinDir, dp,  pinPower, ve, pinEndMin, vi, pinEndMax, va)\
    pinMode(pinPower, OUTPUT);pinMode(pinDir, OUTPUT);pinMode(pinStep, OUTPUT);\
    digitalWriteFast(pinPower, HIGH);setDir##i(true);

#include "../config.h"

#undef STEPPER

}

void StepperController::fastStep(unsigned char id) {
//CI::echo("S");
#ifdef position_log

#define STEPPER(i, sig, pinStep, pd, dp,  pp, ve, pmi, vi, pma, va)\
        /*if (id&(unsigned char)1) {*/\
        if (id&sig) {\
            pos##i += incr##i;\
            BIT_SET(*__digitalPinToPortReg(pinStep), __digitalPinToBit(pinStep));\
            BIT_CLEAR(*__digitalPinToPortReg(pinStep), __digitalPinToBit(pinStep));\
        }\
        //if (!(id>>=1)) return;

#include "../config.h"

#undef STEPPER

#else
#define STEPPER(i, sig, pinStep, pd, dp,  pp, ve, pmi, vi, pma, va)\
        /*if (id&(unsigned char)1) {*/\
        if (id&sig) {\
            BIT_SET(*__digitalPinToPortReg(pinStep), __digitalPinToBit(pinStep));\
            BIT_CLEAR(*__digitalPinToPortReg(pinStep), __digitalPinToBit(pinStep));\
        }\
        //if (!(id>>=1)) return;

#include "../config.h"

#undef STEPPER

#endif

}

/*
unsigned int StepperController::fastStepDelay(unsigned char id) {
    unsigned int delay = 0;
#ifdef position_log

    #define STEPPER(i, sig, si, st, sp, a, d, pinStep, pd, pp, pmi, vi, pma, va) \
        if (id&(unsigned char)1) {\
            pos##i += incr##i;\
            delay+=delays[i];\
            BIT_SET(*__digitalPinToPortReg(pinStep), __digitalPinToBit(pinStep));\
            BIT_CLEAR(*__digitalPinToPortReg(pinStep), __digitalPinToBit(pinStep));\
        }\
        if (!(id>>=1)) return delay;

#include "../config.h"
#undef STEPPER

#else
#define STEPPER(i, sig, si, st, sp, a, d, pinStep, pd, pp, pmi, vi, pma, va) \
        if (id&(unsigned char)1) {\
            delay+=delays[i];\
            BIT_SET(*__digitalPinToPortReg(pinStep), __digitalPinToBit(pinStep));\
            BIT_CLEAR(*__digitalPinToPortReg(pinStep), __digitalPinToBit(pinStep));\
        }\
        if (!(id>>=1)) return delay;

#include "../config.h"

#undef STEPPER

#endif

    return delay;

}

 */



void StepperController::echo_positions() {
#ifdef position_log
    CI::echo("ENDPOS : ");
#define STEPPER(i, ...)\
    CI::echo(String(StepperController::pos##i));

#include "../config.h";

#undef STEPPER
#endif

}

void StepperController::set_dimensions() {
    float *sizes = EEPROMStorage::sizes;
    float *steps = EEPROMStorage::steps;

#define STEPPER(i, ...)\
     max##i = (*(sizes + i))* (*(steps + i));

#include "../config.h";

#undef STEPPER
}


#ifdef position_log

void StepperController::send_position() {
    float t[NB_STEPPERS];
#define STEPPER(i, ...) \
    t[i] = ((float)pos##i/(float)EEPROMStorage::steps[i]);

#include "../config.h";

#undef STEPPER

    CI::send_position(t);
}

#endif

#define m StepperController

#define STEPPER(i, ...) \
    long m::lim##i;\
    long m::max##i;\
    bool m::dir##i = false;

#include "../config.h"

#undef STEPPER

#ifdef position_log
#define STEPPER(i, ...) \
    long m::incr##i = 1;\
    long m::pos##i = 0;

#include "../config.h"

#undef STEPPER
#endif

unsigned int td[NB_STEPPERS];
unsigned int *const m::delays = td;



