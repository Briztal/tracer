
/*
  StaticPID.cpp - Part of TRACER

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

#include <config.h>

#ifdef ENABLE_CONTROL_LOOPS

#include "StaticPID.h"

#include <EEPROM/EEPROMStorage.h>


/*
 * compute : this function computes a particular StaticPID output for a given error.
 *
 */

float StaticPID::compute(uint8_t pid, float error) {

    //Verify the the pid exists.
    if (pid >= NB_PIDS)
        return 0;

    //get-update the sum and get the last.
    float s = (sums[pid] += error), l = lasts[pid];

    //Get the pid data related to the pid.
    pid_data_t *data = EEPROMStorage::pids_data + pid;

    //Compute the return value.
    float ret = data->kp * error + data->ki * s + data->kd * (error - l);

    //Update the last
    lasts[pid] = error;

    //Return the return value.
    return ret;

}


/*
 * reset : this function resets a particular StaticPID.
 *
 */

void StaticPID::reset(uint8_t pid) {

    //Verify the the pid exists.
    if (pid >= NB_PIDS)
        return;

    //Zero the sum and the last.
    sums[pid] = 0;
    lasts[pid] = 0;

}


/*
 * reset_all : this function resets every StaticPID.
 */
void StaticPID::reset_all() {

    //Zero the sums array and the lasts array.
    memset(sums, 0, sizeof(float) * NB_PIDS);
    memset(lasts, 0, sizeof(float) * NB_PIDS);

}


/*
 * Name computing function :
 *
 *  These are function doing the same job that the couple of function upper, but are defined for each StaticPID,
 *      and are called using the StaticPID's name.
 *
 */

#include "StaticPID.h"

#define GENERATE_PID(i, name, ...)\
/*compute_ function : computes the output for a particular error*/\
float StaticPID::compute_##name(float error) {\
    float s = (sums[i]+=error), l = lasts[i];\
    pid_data_t *data = EEPROMStorage::pids_data + i;\
    float ret = data->kp * error + data->ki * s + data->kd * (error - l);\
    lasts[i] = error;\
    return ret;\
}\
/*reset function : resets the sum and last to zero*/\
void StaticPID::reset_##name() {\
    sums[i] = 0;\
    lasts[i] = 0;\
}

#include <Config/control_loops_config.h>
#include <Config/actions_config.h>

#undef GENERATE_PID

//Sums
float t_pid_sums[NB_PIDS];
float *const StaticPID::sums = t_pid_sums;

//Lasts
float t_pid_lasts[NB_PIDS];
float *const StaticPID::lasts = t_pid_lasts;

//PIDs
pid_data_t tcnt[NB_PIDS];
pid_data_t *const StaticPID::pids_data = tcnt;

#endif