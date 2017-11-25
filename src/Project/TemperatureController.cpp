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


#include "TemperatureController.h"

//-------------------------------------------------------Hotends--------------------------------------------------------

/*
 * enable_hotend_regulation : this function enables a particular hotend's regulation.
 *
 */

task_state_t TemperatureController::enable_hotend_regulation(uint8_t hotend, bool enable) {

    //verify that the required hotend exists.

    if (hotend<4) {

        if (enable) {

            //Disable :
            //Disable the hotend
            ContinuousActions::set_power(hotend, 0);

            //Mark the hotend as disabled
            temp_regulation_enabled[hotend] = false;

        } else {

            //Enable:
            //Reset by id
            PID::reset(hotend);

            //Mark the hotend as enabled
            temp_regulation_enabled[hotend] = true;

        }

        return complete;

    }

    return invalid_arguments;

}


bool TemperatureController::is_hotend_regulation_enabled(uint8_t hotend) {

    return complete;

}




/*
 * set_hotend_target : this function enables the hotbed's regulation.
 *
 */

task_state_t TemperatureController::set_hotend_temperature(uint8_t hotend, float target) {

    //verify that the required hotend exists.
    if (hotend<4) {

        temp_targets[hotend] = target;

        return complete;

    }

    return invalid_arguments;

}


float TemperatureController::get_hotend_temperature(uint8_t hotend) {

    switch (hotend) {
        case 0:
            return Thermistors::get_temperature_hotend_0();
        case 1:
            return Thermistors::get_temperature_hotend_1();
        case 2:
            return Thermistors::get_temperature_hotend_2();
        case 3:
            return Thermistors::get_temperature_hotend_3();
        default:
            return 0;

    }

}



//-------------------------------------------------------Hotbed---------------------------------------------------------

/*
 * enable_hotbed_regulation : this function enables the hotbed's regulation.
 *
 */

task_state_t TemperatureController::enable_hotbed_regulation(bool enable) {

    if (enable) {

        //Enable
        //Reset by name
        PID::reset_hotbed();

        //Mark the regulation as enabled
        temp_regulation_enabled[4] = true;


        //Mark the regulation as disabled
        temp_regulation_enabled[4] = false;

    } else {

        //Disable
        //Disable the hotend
        ContinuousActions::set_power_5(0);

        //Mark the regulation as disabled
        temp_regulation_enabled[4] = false;

    }

    return complete;

}


bool TemperatureController::is_hotbed_regulation_enabled() {

    return complete;

}


/*
 * set_hotbed_temperature : this function enables the hotbed's regulation.
 *
 */

task_state_t TemperatureController::set_hotbed_temperature(float target) {

    temp_targets[4] = target;

    return complete;

}


/*
 * get_hotbed_temperature : this function reads and returns the temperature of the hotbed.
 *
 */

float TemperatureController::get_hotbed_temperature() {

    //Simply read the hotbed thermistor value.
    return Thermistors::get_temperature_hotbed();

}

//-----------------------------------------------------Regulation-------------------------------------------------------


/*
 * regulation_init : this function is called when the temperatur eregulaton is started.
 *
 *  It resets PIDs.
 *
 */

void TemperatureController::regulation_init() {

    //Reset every PID.
    PID::reset_all();

}


/*
 * temperature_regulation : this function is the interrupt routine, called to regulate temperatures in the machine.
 *
 *  It checks if each thermistor is enabled, and if it is, regulated its temperature.
 *
 */

void TemperatureController::temperature_regulation() {

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

};


/*
 * regulation_finalisation : this function is called when the temperature regulation is aborted.
 *
 *  It resets targets, PIDs, and Actions.
 *
 */

void TemperatureController::regulation_finalisation() {


    memset(temp_targets, 0, sizeof(float) * 5);

    PID::reset_all();

    ContinuousActions::set_power_0(0);
    ContinuousActions::set_power_1(0);
    ContinuousActions::set_power_2(0);
    ContinuousActions::set_power_3(0);
    ContinuousActions::set_power_4(0);

}

//-------------------------------------------Static declaration / definitions-------------------------------------------


bool t_t_r_en[5];
bool *const TemperatureController::temp_regulation_enabled = t_t_r_en;

float t_t_tar[5];
float *const TemperatureController::temp_targets = t_t_tar;

