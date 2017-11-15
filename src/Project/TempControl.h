/*
  TempControl.h - Part of TRACER

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


#ifndef TRACER_CLOSEDLOOPS_H
#define TRACER_CLOSEDLOOPS_H


#include <Sensors/Thermistors/Thermistors.h>
#include <ControlLoops/PID.h>
#include <Actions/ContinuousActions.h>
#include "TempControl.h"
#include <interface.h>

class TempControl {


public:

    static uint32_t uiii;

    //---------------------------Interrupt regulation functions---------------------------

    //The function to initialise the temperature regulation routine
    static inline void regulation_init() {

        //Reset every PID.
        PID::reset_all();

    }


/*
 * temperature_regulation : this function is the interrupt routine, called to regulate temperatures in the machine.
 *
 *  It checks if each thermistor is enabled, and if it is, regulated its temperature.
 *
 */
    static inline void temperature_regulation() {



        //Thermistor 0 :
        if (temp_regulation_enabled[0]) {


            //Get the current temperature
            float temp = Thermistors::get_temperature_hotend_0();

            if (uiii) {
                CI::echo("temp : " + String(temp));
            }

            //Get the temperature error
            float error = temp_targets[0] - temp;


            //Get the power output
            float output = PID::compute_hotend_0(error);

            if (uiii--) {
                CI::echo("output : " + String(output));
                uiii = 100;
            }

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
            //TODO GRO BUG


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

    //The function to initialise the temperature regulation routine
    static inline void regulation_finalisation() {


        memset(temp_targets, 0, sizeof(float) * 5);

        PID::reset_all();

        ContinuousActions::set_power_0(0);
        ContinuousActions::set_power_1(0);
        ContinuousActions::set_power_2(0);
        ContinuousActions::set_power_3(0);
        ContinuousActions::set_power_4(0);

    }


    //---------------------------State selection functions---------------------------

    //Hotends
    static void enable_hotend_regulation(uint8_t hotend);

    static void set_hotend_target(uint8_t hotend, float target);

    static void disable_hotend_regulation(uint8_t hotend);


    //Hotbed

    static void enable_hotbed_regulation();

    static void set_hotbed_target(float target);

    static void disable_hotbed_regulation();


private:

    //The temperature targets
    static float *const temp_targets;

    //The regulation flags : i-th is set if the i-th thermistor's regulation is enabled.
    static bool *const temp_regulation_enabled;


};


#endif //TRACER_CLOSEDLOOPS_H
