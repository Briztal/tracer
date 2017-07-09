/*
  StepperController.h - Part of TRACER

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

#ifndef CODE_STEPPERCONTROLLER_H
#define CODE_STEPPERCONTROLLER_H

#include <Arduino.h>
#include "../Core/EEPROMStorage.h"
#include "../config.h"

class StepperController {
public:


#define STEPPER(i, ...) \
    static bool isAtMax##i();\
    static bool isAtMin##i();\
    static void enable##i();\
    static void disable##i();\
    static void setDir##i (bool sens);\
    static void step##i();\

#include   "../config.h"
#undef STEPPER

#ifdef position_log
static void send_position();

#endif

    static void set_directions(unsigned char negative_signatures);

    static void set_dimensions();

    static void begin();

    static void enable(unsigned char signature);

    static void fastStep(unsigned char id);

    static unsigned int fastStepDelay(unsigned char id);

    static unsigned int *const delays;
private:

    static unsigned char *step_path;

#define STEPPER(i, ...) \
    static long lim##i;\
    static long max##i;\
    static bool dir##i;
#include  "../config.h"
#undef STEPPER

#ifdef position_log
#define STEPPER(i, ...) \
    static long incr##i;\
    public:static long pos##i;\

#include  "../config.h"
#undef STEPPER
#endif

    static void echo_positions();
};


#endif //CODE_MOTIONCONTROLLER_H
