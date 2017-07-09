/*
  Asserv.h - Part of TRACER

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
#ifndef TRACER_ASSERV_H
#define TRACER_ASSERV_H

/*
 * The Asserv class.
 *
 * It contains everything required to setup a control loop system.
 *
 * First, you can setup multiple PIDs, in the config file.
 *
 * These PIDs can be fed with any value you desire, but two categories are made for it in this class :
 *
 * - External parameters : parameters directly measured in your environment;
 *          (ex : pressure, temperature, vector sum of forces from an accelerometer)
 *
 * - Internal parameters : parameters computed from external parameters, but not directly measurable;
 *          (ex : orientation angles in a drone)
 *
 * These Internal Parameters must be updated from External ones, and so, you must manually define updating functions
 *
 */

class Asserv {

    //--------------------------------------------------------PID-------------------------------------------------------

private:
#define PID(i, name, kp, ki, kd)\
    static float last_##i;\
    static float sum_##i;\
    static float get_pid_##i(float error);

#include "../config.h"

#undef PID

    //------------------------------------------------External Parameters-----------------------------------------------

private :

#define EXTERNAL_PARAMETER(i, name, min, max, unit)\
    static float ref_##name;\
    static float name;\
    static void get_##name();

#include "../config.h"

#undef EXTERNAL_PARAMETER

    //------------------------------------------------Internal Parameters-----------------------------------------------

    /*
     * TODO : Declare here all your parameters, and the processing functions related to them like below :
     *
     * static float your_parameter;
     * static float another_parameter;
     *
     * static void update_function();
     *
     * //Here, update_function updates both two parameters.
     *
     */

    //---------------------------------------------------Loop Functions-------------------------------------------------


#define LOOP_FUNCTION(indice, name, period_ms) \
public:\
    static void enable_##indice();\
    static void disable_##indice();\
    static bool is_active_##indice();\
private:\
    static void trigger_##indice();\
    static bool active_##indice;

#include "../config.h"

#undef LOOP_FUNCTION;

};


#endif //TRACER_ASSERV_H
