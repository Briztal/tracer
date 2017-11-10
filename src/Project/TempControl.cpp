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

#include <Sensors/Thermistors/Thermistors.h>
#include <ControlLoops/PID.h>
#include <Actions/ContinuousActions.h>
#include "TempControl.h"



//-------------------------------------------------Regulation functions-------------------------------------------------


void TempControl::regulation_init() {

    //Reset every PID.
    PID::reset_all();

}

/*
 * temperature_regulation : this function is the interrupt routine, called to regulate temperatures in the machine.
 *
 *  It checks if each thermistor is enabled, and if it is, regulated its temperature.
 *
 */

void TempControl::temperature_regulation() {


    //Thermistor 0 :
    if (temp_regulation_enabled[0]) {

        //Get the current temperature
        float temp = Thermistors::get_temperature_hotend_0();

        //Get the temperature error
        float error = temp_targets[0] - temp;

        //Get the power output
        float output = PID::compute_hotend_0(error);

        //Set the power output.
        ContinuousActions::set_power_0(output);

    }




    //Thermistor 0 :
    if (temp_regulation_enabled[1]) {

        //Get the current temperature
        float temp = Thermistors::get_temperature_hotend_1();

        //Get the temperature error
        float error = temp_targets[1] - temp;

        //Get the power output
        float output = PID::compute_hotend_1(error);

        //Set the power output.
        ContinuousActions::set_power_1(output);

    }


    //Thermistor 2 :
    if (temp_regulation_enabled[2]) {

        //Get the current temperature
        float temp = Thermistors::get_temperature_hotend_2();

        //Get the temperature error
        float error = temp_targets[2] - temp;

        //Get the power output
        float output = PID::compute_hotend_2(error);

        //Set the power output.
        ContinuousActions::set_power_2(output);

    }

    //Thermistor 3 :
    if (temp_regulation_enabled[3]) {

        //Get the current temperature
        float temp = Thermistors::get_temperature_hotend_3();

        //Get the temperature error
        float error = temp_targets[3] - temp;

        //Get the power output
        float output = PID::compute_hotend_3(error);

        //Set the power output.
        ContinuousActions::set_power_3(output);

    }


    //hotbed :
    if (temp_regulation_enabled[4]) {

        //Get the current temperature
        float temp = Thermistors::get_temperature_hotbed();

        //Get the temperature error
        float error = temp_targets[4] - temp;

        //Get the power output
        float output = PID::compute_hotbed(error);

        //Set the power output.
        ContinuousActions::set_power_4(output);

    }

}


//----------------------------------------------Regulation state setters -----------------------------------------------


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
