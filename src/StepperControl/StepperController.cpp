/*
  StepperController.cpp - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/



#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL


#include "StepperController.h"
#include "../Interfaces/CommandInterface.h"
#include "../Core/EEPROMStorage.h"

#define PINS_REQUIRED
#define PARAMS_REQUIRED

#define STEPPER(i, sig, rel, pinStep, pinDir, dp,  pinPower, ve, pinEndMin, vi, pinEndMax, va)\
bool StepperController::isAtMax##i ()  {return (digital_read(pinEndMax )==HIGH);}\
bool StepperController::isAtMin##i() { return (digital_read(pinEndMin)==HIGH);}\


#include "../config.h"

#undef STEPPER


void StepperController::enable(uint16_t signature) {
#define STEPPER(i, sig, rel, dp, ps, pd, pinPower, ve, pmi, vi, pma, va) \
    if (signature&sig) {\
        digital_write(pinPower, LOW);\
    } else {\
        digital_write(pinPower, HIGH);\
    }

#include "../config.h"

#undef STEPPER

}

//TODO PASSER EN SIGNATURES PLUTOT QU'EN ATTRIBUTS
void StepperController::set_directions(uint16_t negative_signatures) {
    bool sig_dir;
#ifdef position_log
#define STEPPER(i, sig, rel, ps, pinDir, dirp,  pp, ve, pmi, vi, pma, va) \
    sig_dir = negative_signatures&sig;\
    if ((!sig_dir) && !dir##i) {\
        dir##i = true;\
        digital_write(pinDir, dirp);\
        incr##i=1;\
    } else if (sig_dir && (dir##i)) {\
        dir##i = false;\
        digital_write(pinDir, !dirp);\
        incr##i=-1;\
    }\


#else

#define STEPPER(i, sig, rel, ps, pinDir, dirp,  pp, ve, pmi, vi, pma, va) \
    sig_dir = negative_signatures&sig;\
    if ((!sig_dir) && !dir##i) {\
        dir##i = true;\
        digital_write(pinDir, dirp);\
    } else if (sig_dir && (dir##i)) {\
        dir##i = false;\
        digital_write(pinDir, !dirp);\
    }\

#endif

#include "../config.h"

#undef STEPPER

}

#ifdef position_log
#define STEPPER(i, sig, rel, ps, pinDir, dirp,  pp, ve, pmi, vi, pma, va) \
void StepperController::setDir##i(bool sens) {\
    CI::echo(String(i)+" "+String(sens));\
    if (!(sens ^ dir##i )) return;\
        dir##i = sens;\
        if (sens) {\
            digital_write(pinDir, dirp);\
            incr##i=1;\
        } else {\
            digital_write(pinDir, !dirp);\
            incr##i=-1;\
    }\
}

#include "../config.h"

#undef STEPPER
#else
#define STEPPER(i, sig, rel, ps, pinDir, dirp,  pp, ve, pmi, vi, pma, va) \
void StepperController::setDir##i(bool sens) {\
    if (!(sens ^ dir##i )) return;\
        dir##i = sens;\
        if (sens) {\
            digital_write(pinDir, dirp);\
            lim##i = max##i ;\
        } else {\
            digital_write(pinDir, !dirp);\
            lim##i = 0;\
    }\
}

#include "../config.h"

#undef STEPPER;
#endif

void StepperController::begin() {

    set_dimensions();

#define STEPPER(i, sig, rel, pinStep, pinDir, dp,  pinPower, ve, pinEndMin, vi, pinEndMax, va)\
    pinMode(pinPower, OUTPUT);pinMode(pinDir, OUTPUT);pinMode(pinStep, OUTPUT);\
    digital_write(pinPower, HIGH);setDir##i(true);

#include "../config.h"

#undef STEPPER

}

void StepperController::fastStep(uint16_t id) {
//CI::echo("S");
#ifdef position_log

#define STEPPER(i, sig, rel, pinStep, pd, dp,  pp, ve, pmi, vi, pma, va)\
        /*if (id&(uint16_t)1) {*/\
        if (id&sig) {\
            pos##i += incr##i;\
            digital_write(pinStep, HIGH);\
            digital_write(pinStep, LOW);\
        }\
        //if (!(id>>=1)) return;

#include "../config.h"

#undef STEPPER

#else
#define STEPPER(i, sig, rel, pinStep, pd, dp,  pp, ve, pmi, vi, pma, va)\
        /*if (id&(uint16_t)1) {*/\
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
uint16_t StepperController::fastStepDelay(uint16_t id) {
    uint16_t delay = 0;
#ifdef position_log

    #define STEPPER(i, sig, rel, si, st, sp, a, d, pinStep, pd, pp, pmi, vi, pma, va) \
        if (id&(uint16_t)1) {\
            pos##i += incr##i;\
            delay+=delays[i];\
            BIT_SET(*__digitalPinToPortReg(pinStep), __digitalPinToBit(pinStep));\
            BIT_CLEAR(*__digitalPinToPortReg(pinStep), __digitalPinToBit(pinStep));\
        }\
        if (!(id>>=1)) return delay;

#include "../config.h"
#undef STEPPER

#else
#define STEPPER(i, sig, rel, si, st, sp, a, d, pinStep, pd, pp, pmi, vi, pma, va) \
        if (id&(uint16_t)1) {\
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

#include "../config.h"

#undef STEPPER
#endif

}

void StepperController::set_dimensions() {
    float *sizes = EEPROMStorage::sizes;
    float *steps = EEPROMStorage::steps;

#include "../config.h"

#undef STEPPER

}


#ifdef position_log

void StepperController::send_position() {
    float t[NB_STEPPERS];
#define STEPPER(i, ...) \
    t[i] = ((float)pos##i/(float)EEPROMStorage::steps[i]);

#include "../config.h"

#undef STEPPER

    CI::send_position(t);
}

#endif

#define m StepperController

#define STEPPER(i, ...) \
    bool m::dir##i = false;

#include "../config.h"

#undef STEPPER

#ifdef position_log
#define STEPPER(i, ...) \
    int32_t m::incr##i = 1;\
    int32_t m::pos##i = 0;

#include "../config.h"

#undef STEPPER
#endif

uint16_t td[NB_STEPPERS];
uint16_t *const m::delays = td;


#endif