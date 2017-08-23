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



#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

#include "StepperController.h"
#include "../interface.h"
#include "../Core/EEPROMStorage.h"


void StepperController::enable(sig_t signature) {
#define STEPPER(i, sig, rel, dp, ps, pd, pinPower, ve, pmi, vi, pma, va) \
    if (signature&sig) {\
        digital_write(pinPower, LOW);\
    } else {\
        digital_write(pinPower, HIGH);\
    }

#include <config.h>

#undef STEPPER

}

//TODO PASSER EN SIGNATURES PLUTOT QU'EN ATTRIBUTS
void StepperController::set_directions(sig_t negative_signatures) {

    long d = micros();

    bool sig_dir;

#ifdef position_log
#define STEPPER(i, sig, rel, ps, pinDir, dirp, pp, ve, pmi, vi, pma, va) \
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

#define STEPPER(i, sig, rel, ps, pinDir, dirp, pp, ve, pmi, vi, pma, va) \
    sig_dir = negative_signatures&sig;\
    if ((!sig_dir) && !dir##i) {\
        dir##i = true;\
        digital_write(pinDir, dirp);\
    } else if (sig_dir && (dir##i)) {\
        dir##i = false;\
        digital_write(pinDir, !dirp);\
    }\

#endif

#include <config.h>

#undef STEPPER

    d = micros()-d;
    CI::echo("TIME : "+String(d));
}

/*
#ifdef position_log
#define STEPPER(i, sig, rel, ps, pinDir, dirp, pp, ve, pmi, vi, pma, va) \
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

#include <config.h>

#undef STEPPER
#else
#define STEPPER(k1_position_indice, sig, rel, ps, pinDir, dirp,  pp, ve, pmi, vi, pma, va) \
void StepperController::setDir##k1_position_indice(bool sens) {\
    if (!(sens ^ dir##k1_position_indice )) return;\
        dir##k1_position_indice = sens;\
        if (sens) {\
            digital_write(pinDir, dirp);\
            lim##k1_position_indice = max##k1_position_indice ;\
        } else {\
            digital_write(pinDir, !dirp);\
            lim##k1_position_indice = 0;\
    }\
}

#include <config.h>

#undef STEPPER;
#endif
 */

void StepperController::begin() {

#define STEPPER(i, sig, rel, pinStep, pinDir, dp, pinPower, ve, pinEndMin, vi, pinEndMax, va)\
    pinMode(pinPower, OUTPUT);pinMode(pinDir, OUTPUT);pinMode(pinStep, OUTPUT);\

#include <config.h>

#undef STEPPER

    enable(0);

}


void StepperController::fastStep(sig_t id) {

#ifdef position_log

#define STEPPER(i, sig, rel, pinStep, pd, dp, pp, ve, pmi, vi, pma, va)\
        if (id&sig) {\
            pos##i += incr##i;\
            digital_write(pinStep, HIGH);\
            digital_write(pinStep, LOW);\
        }

#include <config.h>

#undef STEPPER

#else

#define STEPPER(k1_position_indice, sig, rel, pinStep, pd, dp,  pp, ve, pmi, vi, pma, va)\
        if (id&sig) {\
            digital_write(pinStep, HIGH);\
            digital_write(pinStep, LOW);\
        }

#include <config.h>

#undef STEPPER

#endif

}


#ifdef position_log

void StepperController::send_position() {
    float t[NB_STEPPERS];
#define STEPPER(i, ...) \
    t[i] = ((float)pos##i/(float)EEPROMStorage::steps[i]);

#include <config.h>

#undef STEPPER

    CI::send_position(t);
}

#endif

#define m StepperController

#define STEPPER(i, ...) \
    bool m::dir##i = false;

#include <config.h>

#undef STEPPER

#ifdef position_log
#define STEPPER(i, ...) \
    int32_t m::incr##i = 1;\
    int32_t m::pos##i = 0;

#include <config.h>

#undef STEPPER

#undef m

#endif

#endif