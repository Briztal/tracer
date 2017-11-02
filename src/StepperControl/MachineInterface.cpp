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
#include "MachineInterface.h"
#include "EEPROM/EEPROMStorage.h"
#include "SubMovementManager.h"
#include <Actions/ContinuousActions.h>
#include <hardware_language_abstraction.h>

#if (KERNEL == 0)
#include <LinearMovement.h>
#elif (KERNEL == 1)
#include <StepperControl/KinematicsCore2/Movements/ComplexLinearMovement.h>
#include <Project/Config/geometry.h>
#include <TaskScheduler/task_state_t.h>

#elif (KERNEL == 2)
#include <StepperControl/KinematicsCore2/Movements/ComplexLinearMovement.h>
#endif

//------------------------------------------------------Movements-------------------------------------------------------

/*
 * linear_movement : this function prepares a linear movement from the current position to the provided destination,
 *      realised by the selected kernel.
 */

task_state_t MachineInterface::linear_movement(float *destination) {

#if (KERNEL == 0)
    return LinearMovement::plan_movement(destination);
#elif (KERNEL == 1)
    return ComplexLinearMovement::plan_movement(destination);
#elif (KERNEL == 2)
    return ComplexLinearMovement::plan_movement(destination);
#endif

}



//----------------------------------------------current_stepper_positions-----------------------------------------------

/*
 * get_current_position : this function provides the current position to any external process.
 *
 */

void MachineInterface::get_current_position(float *const position) {
    memcpy(position, current_position, sizeof(float) * NB_AXIS);
}


/*
 * update_position : this function updates the current position with the new position provided as argument.
 *
 */

void MachineInterface::update_position(const float *const new_position) {

    //Update the current position
    memcpy(current_position, new_position, sizeof(float) * NB_AXIS);

    //Update the low level's end point.
    SubMovementManager::update_end_position(new_position);

}


//--------------------------------------------Coordinate_Systems_Translation--------------------------------------------

/*
 * translate : this function translates a position expressed in the high level coordinate system into
 *      its image, in the stepper coordinate system.
 *
 *  The procedure depends on the machine it runs for, so it simply calls an inline function in Project/geometry.h.
 *
 */

void MachineInterface::translate(const float *const hl_coordinates, float *const steppers_coordinates) {

    geometry_translate(hl_coordinates, steppers_coordinates);

}


/*
 * get_stepper_positions_for : this function gets the stepper positions, for the provided point, according to
 *      the provided trajectory_function (get_position).
 *
 */

void MachineInterface::get_stepper_positions_for(void (*get_position)(float, float *), float point, float *positions) {

    //Initialisa the local high level position array;
    float hl_positions[NB_AXIS];

    //Get the initial high level position
    get_position(point, hl_positions);

    //Translate it to obtain the initial stepper position
    MachineInterface::translate(hl_positions, positions);
}


//---------------------------------------------------Speed_Management---------------------------------------------------

/*
 * get_speed : this function returns the regulation_speed on the current regulation_speed group.
 *
 */

float MachineInterface::get_speed() {
    return speeds[speed_group];
}


/*
 * get_speed_group : this function provides the regulation_speed group to any external process.
 */

uint8_t MachineInterface::get_speed_group() {
    return speed_group;
}


/*
 * set_speed_group : this function updates the current regulation_speed group with the value provided inargument.
 */

void MachineInterface::set_speed_group(uint8_t speed_group) {
    MachineInterface::speed_group = speed_group;
}


/*
 * set_speed_for_group : this function updates the regulation_speed for the group provided in argument with the regulation_speed
 *      provided in argument, or with the maximum regulation_speed for this group if the regulation_speed is greater than the limit.
 *
 */

task_state_t MachineInterface::set_speed_for_group(uint8_t speed_group, float new_speed) {

    //If the current speed if the new speed, nothing to change.
    if (speeds[speed_group] == new_speed) {
        return null_task;
    }

    //Get the maximum regulation_speed for this group
    float max_speed = max_speeds[speed_group];


    //update speeds with the minimum of regulation_speed and the maximum regulation_speed
    speeds[speed_group] = min(max_speed, new_speed);

    //Task completed
    return complete;

}


/*
 * get_movement_distance_for_group : this function computes the movement distance for the movement provided in argument,
 *      in the cartesian group provided in argument.
 *
 *  The movement is provided in the form of its step_distances.
 *
 *  The distance in the group is defined as the norm2 of the distance vector's projected
 *      in the concerned cartesian group
 *
 * 4 us
 *
 */

float MachineInterface::get_movement_distance_for_group(uint8_t speed_group, const float *const distances) {

    float square_dist_sum = 0;

    //Initialise the stepper index pointer
    const int8_t *indexes = (speed_groups_indices + 3 * speed_group);


    //Sum the square of all distance :
    for (uint8_t stepper = 0; stepper < 3; stepper++) {

        //Get the current axis value
        int8_t index = indexes[stepper];

        //if the cartesian group comprises less than 3 axis;
        if (index == -1) break;

        //get the distance
        float dist = distances[index];

        //update the square sum
        square_dist_sum += dist * dist;
    }

    //compute the square root and return it.
    float f = sqrt_float(square_dist_sum);

    return f;
}


//---------------------------------------------------Tools_Management---------------------------------------------------

/*
 * set_energy_density : sets the energy density for the action whose index is provided in argument
 */

void MachineInterface::set_energy_density(uint8_t tool_index, float power) {
    tools_energy_densities[tool_index] = power;
}


/*
 * get_tools_data : this function makes a copy of current energy densities, and returns the signatures of
 *      enabled tools.
 */

sig_t MachineInterface::get_tools_data(float *energy_densities) {

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

uint8_t MachineInterface::set_tools_updating_function(sig_t tools_signature, void (**updating_functions)(float)) {

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

void MachineInterface::stop_tools(sig_t stop_signature) {

#define CONTINUOUS(i, name, pin, max) \
    if (stop_signature & ((sig_t)((sig_t) 1 << i))) {\
        ContinuousActions::stop_##i();\
    }

#include <config.h>

#undef CONTINUOUS

}


//--------------------------------------------Static declaration - definition-------------------------------------------

#define m MachineInterface

//Positions
float t_hl_pos[NB_AXIS]{0};
float *m::current_position = t_hl_pos;

//Speeds
float t_speeds[NB_CARTESIAN_GROUPS+1] = {

#define CARTESIAN_GROUP(...) 100,

#include <config.h>

#undef CARTESIAN_GROUP
        //end the array
        0};
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



//Speed groups indices

//declare and fill the array
int8_t t_sg_indices[3 * NB_CARTESIAN_GROUPS + 1] = {

#define CARTESIAN_GROUP(i, a, b, c, s) a, b, c,

#include <config.h>

#undef CARTESIAN_GROUP
        //end the array
        0};

//Assign the array
const int8_t *const m::speed_groups_indices = t_sg_indices;


#undef m

#endif