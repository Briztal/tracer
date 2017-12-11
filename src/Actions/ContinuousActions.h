/*
  ContinuousActions.h - Part of TRACER

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

/*
 * TODO DOC : difference btwn an action and a PWM : Power abstraction layer (simple linear correspondance)
 */


#ifndef CODE_CONTINUOUSACTIONS_H
#define CODE_CONTINUOUSACTIONS_H

#include <stdint.h>
#include <config.h>

class ContinuousActions {

public:

    //initialise_data all PWMs
    static void init();

    //Set an action's power
    static void set_power(uint8_t action, float power);

    //get an action's state
    static bool get_state(uint8_t action);

    //stop an acion.
    static void stop(uint8_t action);

#define CONTINUOUS(i, name, pin, max) \
    static void set_power_##i(float f);\
    static bool get_state_##i();\
    static void stop_##i();

#include <config.h>

#undef CONTINUOUS

private:

    //PWMs states
    static bool *const states;
};


#endif //CODE_CONTINUOUSACTIONS_H
