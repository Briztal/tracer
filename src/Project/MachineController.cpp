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
 *  1 Hotend per carriage_id, 1 thermistor per carriage_id
 *
 *  1 Hotbed, with 1 thermistor
 *
 *  1 permanent ventilation per Hotend
 *
 *  1 power-adjustable print-ventilation per carriage_id.
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
    switch (mode) {
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
    switch (mode) {
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
 * mode_0 : single carriage_id mode    CI::echo("complete : "+String(state == complete));
.
 *
 *  In this mode, every carriage_id can be moved independently.
 *
 */

task_state_t MachineController::mode_0(machine_coords_t *coords) {

    MachineInterface::set_speed_group(carriage_id);

    bool x0, y0;

    switch (carriage_id) {
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


//-----------------------------------------------------setup---------------------------------------------------

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


task_state_t MachineController::speed_set(uint8_t carriage_id, float speed) {

    //TODO VERIFY CARRIAGE AND SPEED BOUNDS

    //Set the speed
    return MachineInterface::set_speed_for_group(carriage_id, speed);

}


task_state_t MachineController::extruder_set(uint8_t carriage) {


    //Nothing to do if the carriage id the current one.
    if (carriage == carriage_id) {
        return null_task;
    }

    switch (mode) {
        case 0:
            //4 carriage mode
            if (carriage > 3) {
                return invalid_arguments;
            }
            break;
        case 1 :
            //2 carriages mode
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
            //Never happens
            return complete;
    }

    //If valid arguments :
    carriage_id = carriage;

    return complete;

}


task_state_t MachineController::extruder_speed_set(uint8_t carriage_id, float speed) {

    task_state_t state = extruder_set(carriage_id);

    if (state == complete) {
        state = speed_set(carriage_id, speed);
    }

    return state;

}



//-------------------------------Cooling-------------------------------

/*
 *
 * set_cooling_power : this function sets the power of the cooling.
 *
 * It does not turn it on though.
 *
 * For this, call enable_cooling
 *
 */

task_state_t MachineController::set_cooling_power(float power) {

    //Minor the power;
    if (power < 0)
        power = 0;

    //Major the power;
    if (power > 100)
        power = 100;

    //Save the power
    cooling_power = power;

    //Complete
    return complete;

}

/*
 * get_cooling_power : this function returns the power of the cooling
 *
 */

float MachineController::get_cooling_power() {

    CI::echo(cooling_power);

    return complete;
}

task_state_t MachineController::enable_cooling(bool enable) {

    if (enable)
        ContinuousActions::set_power_5(cooling_power);
    else
        ContinuousActions::stop_0();

    return complete;

}

bool MachineController::is_cooling_enabled() {

    if (ContinuousActions::get_state_5())
        CI::echo("1");
    else
        CI::echo("0");

    return complete;

}


uint8_t MachineController::mode = 0;

uint8_t MachineController::carriage_id = 0;

float MachineController::cooling_power = 100;

float tmpos[NB_AXIS];
float *const MachineController::position = tmpos;

float t_mch_crd[4]{0};
float *const MachineController::machine_coords = t_mch_crd;

