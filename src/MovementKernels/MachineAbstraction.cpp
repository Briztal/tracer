/*
  StepperAbstraction.cpp - Part of TRACER

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

#ifdef ENABLE_STEPPER_CONTROL

#include <stdint.h>
#include <Arduino.h>
#include "MachineAbstraction.h"
#include "../Core/EEPROMStorage.h"
#include <MovementKernels/Kernel2/RealTimeProcessor.h>
#include <Actions/ContinuousActions.h>


/*
 * translate : this function translates a position expressed in the high level coordinate system into
 *      its image, in the stepper coordinate system.
 *
 */

void MachineAbstraction::translate(const float *const hl_coordinates, float *const steppers_coordinates) {

    //0.8 us
    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        steppers_coordinates[axis] = (EEPROMStorage::steps[axis] * hl_coordinates[axis]);

    }
}


/*
 * invert : this function translates a position expressed in the stepper coordinate system into
 *      its image, in the high level coordinate system.
 *
 */

void MachineAbstraction::invert(const float *const steppers_coordinates, float *const hl_coordinates) {

    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        hl_coordinates[axis] = (steppers_coordinates[axis]) / EEPROMStorage::steps[axis];
    }
}


/*
 * get_current_position : this function provides the current position to any external process.
 *
 */

void MachineAbstraction::get_current_position(float *const position) {
    memcpy(position, current_position, sizeof(float) * NB_AXIS);
}


/*
 * update_position : this function updates the current position with the new position provided as argument.
 *
 */

void MachineAbstraction::update_position(const float *const new_position) {

    //Update the current position
    memcpy(current_position, new_position, sizeof(float) * NB_AXIS);

    //Update the low level's end point.
    RealTimeProcessor::update_end_position(new_position);

}


/*
 * get_speed : this function returns the speed on the current speed group.
 *
 */

float MachineAbstraction::get_speed() {
    return speeds[speed_group];
}


/*
 * get_speed_group : this function provides the speed group to any external process.
 */

uint8_t MachineAbstraction::get_speed_group() {
    return speed_group;
}


/*
 * set_speed_group : this function updates the current speed group with the value provided inargument.
 */

void MachineAbstraction::set_speed_group(uint8_t speed_group) {
    MachineAbstraction::speed_group = speed_group;
}


/*
 * set_speed_for_group : this function updates the speed for the group provided in argument with the speed
 *      provided in argument, or with the maximum speed for this group if the speed is greater than the limit.
 *
 */

void MachineAbstraction::set_speed_for_group(uint8_t speed_group, float new_speed) {

    //Get the maximum speed for this group
    float max_speed = max_speeds[speed_group];

    //update speeds with the minimum of speed and the maximum speed
    speeds[speed_group] = min(max_speed, new_speed);

}

//---------------------------------------------------TOOLS_MANAGEMENT---------------------------------------------------


/*
 * set_energy_density : sets the energy density for the action whose index is provided in argument
 */

void MachineAbstraction::set_energy_density(uint8_t tool_index, float power) {
    tools_energy_densities[tool_index] = power;
}


/*
 * get_tools_data : this function makes a copy of current energy densities, and returns the signatures of
 *      enabled tools.
 */

sig_t MachineAbstraction::get_tools_data(float *energy_densities) {

    uint8_t id = 0;
    sig_t sig = 0;
    float density;

#define CONTINUOUS(i, name, pin, max) \
    if ((density = tools_energy_densities[i])) {\
        energy_densities[id++] = density;\
        sig |= ((sig_t)1<<i);\
    }

#include <config.h>

#undef CONTINUOUS

    return sig;

}


/*
 * set_tools_updating_function : this function, given a signature, (i_th bit = true -> action i enabled),
 *      updates the function array given in argument, with the power settng function
 */

uint8_t MachineAbstraction::set_tools_updating_function(sig_t tools_signature, void (**updating_functions)(float)) {

    uint8_t id = 0;

#define CONTINUOUS(i, name, pin, max) \
    if (tools_signature&((sig_t)1<<i)) {\
        updating_functions[id++] = ContinuousActions::set_power##i;\
    }

#include <config.h>

#undef CONTINUOUS

    return id;

}


/*
 * stop_tools : stop tools referred by the provided signature.
 */

void MachineAbstraction::stop_tools(sig_t stop_signature) {

#define CONTINUOUS(i, name, pin, max) \
    if (stop_signature & ((sig_t)((sig_t) 1 << i))) {\
        ContinuousActions::stop_##i();\
    }

#include <config.h>

#undef CONTINUOUS

}


//--------------------------------------------Static declaration - definition-------------------------------------------

#define m MachineAbstraction

//Positions
float t_hl_pos[NB_AXIS]{0};
float *m::current_position = t_hl_pos;

//Speeds
float t_speeds[NB_CARTESIAN_GROUPS];
float *const m::speeds = t_speeds;

uint8_t m::speed_group;
//Maximum speeds

//declare and fill the array
static const float t_mx_speeds[NB_CARTESIAN_GROUPS + 1] = {

#define CARTESIAN_GROUP(i, a, b, c, s) s,

#include <config.h>

#undef CARTESIAN_GROUP
        //end the array
        0};
//Assign the array

const float *const m::max_speeds = t_mx_speeds;

//Tools
float t_tl_lp[NB_CONTINUOUS];
float *m::tools_energy_densities = t_tl_lp;


#undef m

#endif