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
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdint.h>
#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef CODE_STEPPERCONTROLLER_H
#define CODE_STEPPERCONTROLLER_H

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

    static void set_directions(sig_t negative_signatures);

    static void set_dimensions();

    static void begin();

    static void enable(sig_t signature);

    static void fastStep(sig_t id);

    static uint16_t fastStepDelay(sig_t id);

    static uint16_t *const delays;
private:

    static uint8_t *step_path;

#define STEPPER(i, ...) \
    static bool dir##i;
#include  "../config.h"
#undef STEPPER

#ifdef position_log
#define STEPPER(i, ...) \
    static int32_t incr##i;\
    public:static int32_t pos##i;\

#include  "../config.h"
#undef STEPPER
#endif

    static void echo_positions();
};

#endif //CODE_MOTIONCONTROLLER_H

#endif