/*
  Machine.h - Part of TRACER

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

#include <EEPROM.h>
#include <EEPROM/EEPROMStorage.h>
#include "MachineController.h"
#include "TemperatureController.h"
#include <StepperControl/MachineInterface.h>
#include <StepperControl/StepperController.h>
#include <StepperControl/TrajectoryTracer.h>
#include <interface.h>
#include <TaskScheduler/TaskScheduler.h>
#include <Actions/ContinuousActions.h>


//-------------------------------------------------Movement-------------------------------------------------------------


/* MachineController Structure :
 *     _________________
 *    |   |      |      |
 *    |   |      |      |
 * y1 |---|------|------|
 *    |   |3    2|      |
 *    |   |      |      |
 *    |   |0    1|      |
 * y0 |---|------|------|
 *    |___|______|______|
 *       x0     x1      x
 *  z
 *
 *
 *  Coordinates structure : 0   1   2   3   4   5   6   7   8
*                           X0  X1  Y0  Y1  Z   E0  E1  E2  E3
 *
 *  1 endstop per X_ Y_ Z.
 *
 *  1 Hotend per working_extruder, 1 thermistor per working_extruder
 *
 *  1 Hotbed, with 1 thermistor
 *
 *  1 permanent ventilation per Hotend
 *
 *  1 power-adjustable print-ventilation per working_extruder.
 *
 */



//TODO DEBUG

/*
 * Carriage reset :
 *
 *  This function resets all axis to their zero coordinate, at the maximal speed that do not exceeds carriages
 *      regulation speeds. This speed will be one of current regulations speeds.
 *
 *  It determines the maximal speed with the following algorithm :
 *      - set the carriage 0 as the reference (the one whose speed will be its regulation speed).
 *      - for each carriage,
 *          if the speed of the carriage (determined according to the reference carriage) exceeds its regulation speed,
 *          then this carriage becomes the reference
 *
 */

task_state_t MachineController::carriages_reset() {

    //Initialise the reference carriage data
    carriage_data t = carriage_data();

    //Initialise the reference carriage.
    set_reference_carriage(0, position[0], position[2], &t);

    //Verify carriages 1, 2 and 3.
    check_carriage(1, position[1], position[2], &t);
    check_carriage(2, position[1], position[3], &t);
    check_carriage(3, position[0], position[3], &t);

    //Set the reference carriage, that will determine the movement speed.
    MachineInterface::set_speed_group(t.carriage_id);

    //No need to set the regulation speed, ad it is already set.

    //Zero all carriages
    position[0] = position[1] = position[2] = position[3] = 0;

    //Move
    return MachineInterface::linear_movement(position);

}


/*
 * set_reference_carriage : this function updates the carriage identifier data, with the provided indice,
 *      and determines the movement time corresponding to the provided movement data, with the following formula :
 *
 *          Movement_time = movement_distance / movement_speed = sqrt(movement_x **2 + movement_y**2) / movement_dist
 *
 */

void MachineController::set_reference_carriage(uint8_t id, float x, float y, carriage_data *data) {

    //Set the ID
    data->carriage_id = id;

    //Use the formula to determine the time
    data->time = sqrt_float(x * x + y * y) / MachineInterface::get_speed(id);

}


/*
 * check_carriage : this function verifies that the movement speed, with the reference carriage [data] does not exceeds
 *
 *  the carriage [id] regulation speeds. if so, it sets [id] as the reference carriage, using the formula in
 *      the function behind
 *
 */

void MachineController::check_carriage(uint8_t id, float x, float y, carriage_data *data) {

    //Determine the carriage [id] 's movement distance.
    float distance = sqrt_float(x * x + y * y);

    //Determine the speed corresponding to this movement, knowing the movement time
    float speed = distance / data->time;

    float reg_speed = MachineInterface::get_speed(id);

    //If the speed exceeds the carriage's regulation speed, then set [id] as the reference carriage.
    if (speed > reg_speed) {

        //Set the ID
        data->carriage_id = id;

        //Determine the time like in the function behind (we didn't call it, because we already have the distance.
        data->time = distance / reg_speed;

    }

}


//TODO COMMENT
void MachineController::fill_coords(machine_coords_t *coords) {

    if (!coords->x_enabled) {
        coords->x = machine_coords[0];
    }
    if (!coords->y_enabled) {
        coords->y = machine_coords[1];
    }
    if (!coords->z_enabled) {
        coords->z = machine_coords[2];
    }
    if (!coords->e_enabled) {
        coords->e = machine_coords[3];
    }
}

void MachineController::persist_coords(machine_coords_t *coords) {

    machine_coords[0] = coords->x;
    machine_coords[1] = coords->y;
    machine_coords[2] = coords->z;
    machine_coords[3] = coords->e;

}


task_state_t MachineController::line_to(machine_coords_t coords) {

    //Initialise a state
    task_state_t state = complete;

    //Get the destination coordinates
    fill_coords(&coords);

    //Try to move to the specified position
    switch (extrusion_state.mode) {
        case 0:
            state = mode_0(&coords);
        default:
            break;
    }

    if (state == complete) {
        persist_coords(&coords);
    }

    return state;

}


task_state_t MachineController::line_of(machine_coords_t coords) {

    //Update the local coords
    coords.x += machine_coords[0];
    coords.y += machine_coords[1];
    coords.z += machine_coords[2];
    coords.e += machine_coords[3];

    //Initialise a state
    task_state_t state = complete;

    //Try to move to the specified position
    switch (extrusion_state.mode) {
        case 0:
            state = mode_0(&coords);
        default:
            break;
    }

    if (state == complete) {
        persist_coords(&coords);
    }

    return state;

}

//---------------------------------------------------Movement planners--------------------------------------------------

/*
 * mode_0 : single working_extruder mode    CI::echo("complete : "+String(state == complete));
.
 *
 *  In this mode, every working_extruder can be moved independently.
 *
 */

task_state_t MachineController::mode_0(machine_coords_t *coords) {

    MachineInterface::set_speed_group(extrusion_state.working_extruder);

    bool x0, y0;

    switch (extrusion_state.working_extruder) {
        case 0:
            x0 = y0 = true;
            break;
        case 1:
            x0 = false;
            y0 = true;
            break;
        case 2:
            x0 = y0 = false;
            break;
        case 3:
            x0 = true;
            y0 = false;
            break;
        default:
            return invalid_arguments;
    }


    if (x0) {
        position[0] = EEPROMStorage::coordinate_interface_data.x0_offset + coords->x;
    } else {
        position[1] = EEPROMStorage::coordinate_interface_data.x1_offset - coords->x;
    }

    if (y0) {
        position[2] = EEPROMStorage::coordinate_interface_data.y0_offset + coords->y;
    } else {
        position[3] = EEPROMStorage::coordinate_interface_data.y1_offset - coords->y;
    }

    position[4] = EEPROMStorage::coordinate_interface_data.z_offset - coords->z;

    //TODO SANITY CHECK
    //sanity_check(position);

    for (uint8_t i = 0; i < NB_AXIS; i++) {
        CI::echo(String(i) + " " + String(position[i]));
    }

    return MachineInterface::linear_movement(position);


}


/*
 * sanity_check : this function verifies that the required destination coordinates are valid.
 *
 *  It verifies that coordinates on each axis are not negative (beyond the endstop) and that the space between
 *      carriages allows them not to be in contact.
 *
 */

void MachineController::sanity_check(float *position) {

    //First, we must check that coordinates on moving axis are not negative :
    float *f;
    for (int axis = 0; axis < 5; axis++) {
        f = position + axis;
        if (*f < 0) *f = 0;
    }

    /* Now we must check that the distance between carriages are sufficient to avoid contact. As opposed axis
     *  move in opposite direction, we measure the sum of their coordinates.
     */

    //We first check X axis :

    //Cache for the difference between the sum and the max sum.
    float difference = EEPROMStorage::coordinate_interface_data.x_max_sum - (position[0] + position[1]);

    //If the sum is greater than the limit,
    if (difference) {

        //First, offset x1
        float tmp = position[1] -= difference;

        //If x1 becomes negative, zero x1 and offset x0 of the remaining distance
        if (tmp < 0) {
            position[0] += tmp;
            position[1] = 0;
        }
    }

    //Same thing for Y axis :

    //Cache for the difference between the sum and the max sum.
    difference = EEPROMStorage::coordinate_interface_data.y_max_sum - (position[2] + position[3]);

    //If the sum is greater than the limit,
    if (difference) {

        //First, offset x1
        float tmp = position[3] -= difference;

        //If x1 becomes negative, zero x1 and offset x0 of the remaining distance
        if (tmp < 0) {
            position[2] += tmp;
            position[3] = 0;
        }
    }

}


//-----------------------------------------------------Steppers---------------------------------------------------

task_state_t MachineController::enable_steppers(bool enable) {

    if (enable) {

        //Enable all steppers
        StepperController::enable();

    } else {

        //Disable all steppers
        StepperController::disable();

    }

    //Complete
    return complete;
}

//-----------------------------------------------------Extrusion---------------------------------------------------

task_state_t MachineController::set_extrusion_state(extrusion_state_t new_state) {

    //Initialise a state, that will be updated at each modification.
    task_state_t state = complete;

    //if the working extruder must be changed :
    if (new_state.working_extruder_flag) {

        //Change the working extruder
        state = set_working_extruder(new_state.working_extruder);

        //Fail if the function failed.
        if (state != complete) return state;


    }

    //As next blocs only focus on each carriage's speed, always the same way, we will use a macro.

#define SPEED_MODIFICATION(speed_group, flag, new_speed) \
    /*if the working extruder must be changed : */\
    if (flag) {\
        \
        /*Change the speed for i'th carriage*/\
        MachineInterface::set_speed_for_group(speed_group, new_speed);\
        \
        /*Fail if the function failed.*/\
        if (state!=complete) return state;\
        \
    }

    //Eventually modify the speed for the current carriage
    SPEED_MODIFICATION(extrusion_state.working_extruder, new_state.current_speed_flag, new_state.current_speed)

    //Eventually modify the speed for every carriage
    SPEED_MODIFICATION(0, new_state.speed_0_flag, new_state.speed_0)
    SPEED_MODIFICATION(1, new_state.speed_1_flag, new_state.speed_1)
    SPEED_MODIFICATION(2, new_state.speed_2_flag, new_state.speed_2)
    SPEED_MODIFICATION(3, new_state.speed_3_flag, new_state.speed_3)

    //For safety and good practices, undef the macro.
#undef SPEED_MODIFICATION

}


task_state_t MachineController::set_working_extruder(uint8_t carriage) {

    //Nothing to do if the carriage id the current one.
    if (carriage == extrusion_state.working_extruder) {
        return null_task;
    }

    switch (extrusion_state.mode) {
        case 0:
            //4 carriage mode.
            if (carriage > 3) {
                return invalid_arguments;
            }
            break;
        case 1 :
            //2 carriages mode.
            if (carriage > 1) {
                return invalid_arguments;
            }
            break;
        case 2:
            //1 carriages mode : always zero.
            if (carriage) {
                return invalid_arguments;
            }
            break;
        default:
            //Never happens.
            return complete;
    }

    //If valid arguments :
    extrusion_state.working_extruder = carriage;

    //Succeed.
    return complete;

}


/*
 * get_extrusion_state : this function returns the current state of the cooling.
 *
 */

const MachineController::extrusion_state_t MachineController::get_extrusion_state() {

    return extrusion_state;

}

//-------------------------------Cooling-------------------------------


task_state_t MachineController::set_cooling_state(cooling_state_t new_state) {

    //If the enable state must be changed
    if (new_state.enabled_flag) {

        if (new_state.enabled) {

            //If the cooling must be enabled, enable it.
            ContinuousActions::set_power_5(cooling_state.power);

        } else {

            //If the cooling must be stopped, stop it.
            ContinuousActions::stop_0();

        }
    }

    //If the power must be changed
    if (new_state.power_flag) {

        //Cache for the power.
        float power = new_state.power;

        //Minor the power;
        if (power < 0)
            power = 0;

        //Major the power;
        if (power > 100)
            power = 100;

        //Save the power
        cooling_state.power = power;

    }

}

/*
 * get_cooling_state : this function returns the current state of the cooling.
 *
 */

const MachineController::cooling_state_t MachineController::get_cooling_state() {

    return cooling_state;

}

MachineController::cooling_state_t MachineController::cooling_state = MachineController::cooling_state_t();

MachineController::extrusion_state_t MachineController::extrusion_state = MachineController::extrusion_state_t();

float tmpos[NB_AXIS];
float *const MachineController::position = tmpos;

float t_mch_crd[4]{0};
float *const MachineController::machine_coords = t_mch_crd;

