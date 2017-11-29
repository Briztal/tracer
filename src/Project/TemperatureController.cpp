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

task_state_t TemperatureController::set_hotends_state(hotends_state_t new_state) {

    //As we must control flags for each hotend, a repetitive process, we will use a macro.

#define HOTEND_ENABLE(i) \
    if (new_state.enabled_##i##_flag) {\
        \
        /*if the hotend must be enabled*/\
        if (new_state.enabled_##i) {\
            /*Reset by id*/\
            PID::reset(i);\
            \
            /*Mark the hotend as enabled*/\
             hotends_state.enabled_##i = true;\
            \
        } else {\
           /*Disable the hotend*/\
            ContinuousActions::set_power(i, 0);\
            \
            /*Mark the hotend as disabled*/\
            hotends_state.enabled_##i = false;\
            \
        }\
    }\

#define HOTEND_POWER(i)\
    /*if the hotend power must be changed*/\
    if (new_state.temp_##i##_flag) {\
        /*Update the temperature*/\
        hotends_state.temp_##i = new_state.temp_##i;\
    }\

#define HOTEND_SET(i) HOTEND_ENABLE(i) HOTEND_POWER(i);

HOTEND_SET(0);
HOTEND_SET(1);
HOTEND_SET(2);
HOTEND_SET(3);


#undef HOTEND_ENABLE


}

TemperatureController::hotends_state_t TemperatureController::get_hotends_state() {

    return hotends_state;

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

task_state_t TemperatureController::set_hotbed_state(hotbed_state_t new_state) {

    if (new_state.enabled_flag) {

        //If the bed must be enabled
        if (new_state.enabled) {

            //Reset by name
            PID::reset_hotbed();

            //Mark the regulation as enabled
            hotbed_state.enabled = true;

        } else {
            //If the bed must be disabled

            //Disable the hotend
            ContinuousActions::set_power_5(0);

            //Mark the regulation as disabled
            hotbed_state.enabled = false;

        }

    }

    //If the temperature must be changed
    if (new_state.temp_flag) {

        //Set the new temperature
        hotbed_state.temperature = new_state.temperature;

    }

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


#define TEMP_REGULATION(i, enabled, target, name) \
    \
    /*If the regulation is enabled (flag set)*/\
    if ((enabled)) {\
        \
        /*Get the current temperature*/\
        float temp = Thermistors::get_temperature_##name();\
        \
        /*Get the temperature error*/\
        float error = (target) - temp;\
        \
        /*Get the power output*/\
        float output = PID::compute_##name(error);\
        \
        /*Set the power output.*/\
        ContinuousActions::set_power_##i(output);\
        \
    }

    TEMP_REGULATION(0, hotends_state.enabled_0, hotends_state.temp_0, hotend_0);

    TEMP_REGULATION(1, hotends_state.enabled_1, hotends_state.temp_1, hotend_1);

    TEMP_REGULATION(2, hotends_state.enabled_2, hotends_state.temp_2, hotend_2);

    TEMP_REGULATION(3, hotends_state.enabled_3, hotends_state.temp_3, hotend_3);

    TEMP_REGULATION(4, hotbed_state.enabled, hotbed_state.temperature, hotbed);

};


/*
 * regulation_finalisation : this function is called when the temperature regulation is aborted.
 *
 *  It resets targets, PIDs, and Actions.
 *
 */

void TemperatureController::regulation_finalisation() {

    //Set hotends and hotbed target to zero.
    hotends_state.temp_0 = hotends_state.temp_1 = hotends_state.temp_1 = hotends_state.temp_1 = 0;
    hotbed_state.temperature = 0;

    //Reset all PIDs.
    PID::reset_all();

    //Set all powers to zero
    ContinuousActions::set_power_0(0);
    ContinuousActions::set_power_1(0);
    ContinuousActions::set_power_2(0);
    ContinuousActions::set_power_3(0);
    ContinuousActions::set_power_4(0);

}

//-------------------------------------------Static declaration / definitions-------------------------------------------

//Declare and define the hotends state (flags and vars for activities and temperature targets).
TemperatureController::hotends_state_t TemperatureController::hotends_state = TemperatureController::hotends_state_t();

//Declare and define the hotbed state (flags and vars for activity and temperature target).
TemperatureController::hotbed_state_t TemperatureController::hotbed_state = TemperatureController::hotbed_state_t();

