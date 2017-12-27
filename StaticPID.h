/*
  PID.h - Part of TRACER

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

#ifndef TRACER_SPIDS_H
#define TRACER_SPIDS_H

#include <config.h>

#ifdef ENABLE_CONTROL_LOOPS

#include <EEPROM/_eeprom_storage_data.h>

class StaticPID {

    /*
     * PIDs compute a value with an input.
     *
     * This value is based on the input, the input's sum, and the input's variation.
     *  More litteraly, it verifies the following equation :
     *      output = kp * input + ki * sum + kd * variation
     *
     */


public:

    //Compute a particular StaticPID result for an error at a given time.
    static float compute(uint8_t pid, float error);

    //Reset a particular StaticPID.
    static void reset(uint8_t pid);

    //Reset every StaticPID.
    static void reset_all();



#define GENERATE_PID(i, name, kp, ki, kd)\
    /*compute function*/\
    static float compute_##name(float error);\
    /*reset function*/\
    static void reset_##name();

#include <Config/control_loops_config.h>

#undef GENERATE_PID

private:

    //Array for last measured errors.
    static float *const lasts;

    //Array for sums.
    static float *const sums;

    static pid_data_t *const pids_data;

};


#endif //TRACER_PIDS_H

#endif