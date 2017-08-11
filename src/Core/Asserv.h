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
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

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
 * These Internal Parameters must be updated from External ones, and so, you must manually define updating functions
 *
 */

class Asserv {

    //------------------------------------------------External Parameters-----------------------------------------------

    /*
     * External parameters are represented by two values :
     *  - their setpoint, the value they are meant to reach;
     *  - their feedback, the value they currently have;
     *
     * As a consequence, two methods are defined :
     *  - set_[name]_setpoint : a public method, automatically implemented, callable from outside the class,
     *      that updates the setpoint to the desired value;
     *  - get_[name] : a private method, that you must implement for each parameter you define.
     *      It is in charge of collecting data required to determine the current feedback (maybe interrogate a sensor)
     *      and to actually determine and update the feedback.
     */

#define EXTERNAL_PARAMETER(i, name, min, max, unit)\
    public:\
    static void set_##name##_setpoint();\
    private:\
    static float setpoint_##name;\
    static float feedback_##name;\
    static void get_##name();

#include "../config.h"

#undef EXTERNAL_PARAMETER

    //--------------------------------------------------------PID-------------------------------------------------------

    /*
     * PIDs compute a value with an input.
     *
     * This value is based on the input, the input's sum, and the input's variation.
     *  More litteraly, it verifies the following equation :
     *      output = kp * input + ki * sum + kd * variation
     *
     */

private:

#define PID(i, name, kp, ki, kd)\
    static float last_##i;\
    static float sum_##i;\
    static float get_pid_##i(float error);\
    static float reset_pid_##i();

#include "../config.h"

#undef PID

    //---------------------------------------------------Loop Functions-------------------------------------------------

    /*
     * Loop functions are routines that are called regularly with interrupts.
     *
     * They are defined by the function they call and the period related.
     *
     */

#define LOOP_FUNCTION(indice, name, period_ms) \
public:\
    static void enable_##indice();\
    static void disable_##indice();\
    static bool is_active_##indice();\
private:\
    static void init_loop_##indice();\
    static void trigger_##indice();\
    static bool active_##indice;

#include "../config.h"

#undef LOOP_FUNCTION;

};


#endif //TRACER_ASSERV_H
