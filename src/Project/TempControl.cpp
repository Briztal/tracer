/*
  TempControl.cpp - Part of TRACER

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


#include "TempControl.h"


//-------------------------------------------------------Hotends--------------------------------------------------------


/*
 * enable_hotend_regulation : this function enables a particular hotend's regulation.
 *
 */

void TempControl::enable_hotend_regulation(uint8_t hotend) {

    if (hotend<4) {

        //Reset by id
        PID::reset(hotend);

        //Mark the hotend as enabled
        temp_regulation_enabled[hotend] = true;

    }

}


/*
 * set_hotend_target : this function enables the hotbed's regulation.
 *
 */

void TempControl::set_hotend_target(uint8_t hotend, float target) {

    if (hotend<4) {

        temp_targets[hotend] = target;

    }


}


/*
 * disable_hotend_regulation : this function disables a particular hotend's regulation.
 *
 */

void TempControl::disable_hotend_regulation(uint8_t hotend) {

    if (hotend<4) {

        //Disable the hotend
        ContinuousActions::set_power(hotend, 0);

        //Mark the hotend as disabled
        temp_regulation_enabled[hotend] = false;

    }

}


//-------------------------------------------------------Hotbed---------------------------------------------------------

/*
 * enable_hotbed_regulation : this function enables the hotbed's regulation.
 *
 */

void TempControl::enable_hotbed_regulation() {

    //Reset by name
    PID::reset_hotbed();

    //Mark the regulation as enabled
    temp_regulation_enabled[4] = true;

}



/*
 * set_hotbed_target : this function enables the hotbed's regulation.
 *
 */

void TempControl::set_hotbed_target(float target) {

    temp_targets[4] = target;

}



/*
 * disable_hotbed_regulation : this function disables the hotbed's regulation.
 *
 */

void TempControl::disable_hotbed_regulation() {

    //Disable the hotend
    ContinuousActions::set_power_5(0);

    //Mark the regulation as disabled
    temp_regulation_enabled[4] = false;

}


//-------------------------------------------Static declaration / definitions-------------------------------------------


bool t_t_r_en[5];
bool *const TempControl::temp_regulation_enabled = t_t_r_en;

float t_t_tar[5];
float *const TempControl::temp_targets = t_t_tar;

uint32_t TempControl::uiii = 100;