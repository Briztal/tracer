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
 *  1 Hotend per working_carriage, 1 thermistor per working_carriage
 *
 *  1 Hotbed, with 1 thermistor
 *
 *  1 permanent ventilation per Hotend
 *
 *  1 power-adjustable print-ventilation per working_carriage.
 *
 */



//TODO DEBUG

/*
 * home :
 *
 *  This function resets all axis to their zero coordinate, at the maximal speed that do not exceeds carriages
 *      regulation speeds. This speed will be one of current regulations speeds.
 *
 *  It determines the maximal speed with the following algorithm :
 *      - set the carriage 0 as the reference (the one whose speed will be its regulation speed).
 *      - for each carriage,
 *          if the speed of the carriage (determined according to the reference carriage) exceeds its regulation speed,
 *          then this carriage becomes the reference
 */

task_state_t MachineController::home() {

    //Initialise the reference carriage data
    carriage_data t = carriage_data();

    //Initialise the reference carriage.
    set_reference_carriage(0, axis_positions[0], axis_positions[2], &t);

    //Verify carriages 1, 2 and 3.
    check_carriage(1, axis_positions[1], axis_positions[2], &t);
    check_carriage(2, axis_positions[1], axis_positions[3], &t);
    check_carriage(3, axis_positions[0], axis_positions[3], &t);

    //Set the reference carriage, that will determine the movement speed.
    MachineInterface::set_speed_group(t.carriage_id);

    //No need to set the regulation speed, ad it is already set.

    //Zero all carriages
    axis_positions[0] = axis_positions[1] = axis_positions[2] = axis_positions[3] = 0;

    //Move
    return MachineInterface::linear_movement(axis_positions);

}


/*
 * set_reference_carriage : this function updates the carriage identifier data, with the provided indice,
 *      and determines the movement time corresponding to the provided movement data, with the following formula :
 *
 *          Movement_time = movement_distance / movement_speed = sqrt(movement_x **2 + movement_y**2) / movement_dist
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


/*
 * replace_coords : this function replaces coordinates with flag reset by current coordinates.
 */

void MachineController::replace_coords(movement_state_t *coords) {

    if (!coords->x_flag) {
        coords->x = current_position.x;
    }
    if (!coords->y_flag) {
        coords->y = current_position.y;
    }
    if (!coords->z_flag) {
        coords->z = current_position.z;
    }
    if (!coords->e_flag) {
        coords->e = current_position.e;
    }
}


/*
 * line : this function schedules a linear move to the given coordinates.
 *
 *  The destination position can be given in two ways :
 *      - absolute, real coordinates are given;
 *      - relative, algebraic distances from the current position are given;
 */

task_state_t MachineController::line(movement_state_t movement_state) {

    if (!TrajectoryTracer::enqueue_authorised()) {
        return reprogram;
    };

    //Initialise a state
    task_state_t state = complete;

    //The movement_state_t inherits the carriage_state_t, that allows us to modify the extrusion state before moving.
    task_state_t return_state = set_carriages_state((carriages_state_t) movement_state);

    //If the modification failed, fail.
    if (return_state != complete) {
        return return_state;
    }

    /* As we must transmit the new destination coordinates, we will set all coordinates in movement_state, and pass it
     *  to movement functions.
     */

    if (movement_state.relative_flag) {
        //Relative movement

        //Sum the given values to the current coordinates to get destination coordinates
        movement_state.x += current_position.x;
        movement_state.y += current_position.y;
        movement_state.z += current_position.z;
        movement_state.e += current_position.e;

    } else {
        //Absolute movement

        //Get the destination coordinates
        replace_coords(&movement_state);

    }

    //Try to move to the specified position
    switch (mode) {
        case 0:
            state = move_mode_0(&movement_state);
        default:
            break;
    }

    if (state == complete) {
        persist_coords(&movement_state);
    }

    return state;

}


/*
 * persist_coords : current coordinates are updated, to new one, given by coords
 */

void MachineController::persist_coords(movement_state_t *coords) {

    //Save new coordinates.
    current_position.x = coords->x;
    current_position.y = coords->y;
    current_position.z = coords->z;
    current_position.z = coords->e;

}



//---------------------------------------------------Movement planners--------------------------------------------------

/*
 * move_mode_0 : single working_carriage mode
.
 *
 *  In this mode, every working_carriage can be moved independently.
 */

task_state_t MachineController::move_mode_0(movement_state_t *coords) {

    MachineInterface::set_speed_group(extrusion_state.working_carriage);

    bool x0, y0;

    switch (extrusion_state.working_carriage) {
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

    //Get the real position, taking offsets into account.
    float x = coords->x + offsets.x;
    float y = coords->y + offsets.y;
    float z = coords->z + offsets.z;
    float e = coords->e + offsets.e;

    //Set the x position
    if (x0) {
        axis_positions[0] = EEPROMStorage::coordinate_interface_data.x0_offset + x;
    } else {
        axis_positions[1] = EEPROMStorage::coordinate_interface_data.x1_offset - x;
    }

    //Set the y position
    if (y0) {
        axis_positions[2] = EEPROMStorage::coordinate_interface_data.y0_offset + y;
    } else {
        axis_positions[3] = EEPROMStorage::coordinate_interface_data.y1_offset - y;
    }

    //Set the z position
    axis_positions[4] = EEPROMStorage::coordinate_interface_data.z_offset - z;

    //Set the extruder position
    axis_positions[5 + extrusion_state.working_carriage] = e;

    //TODO SANITY CHECK
    //sanity_check(position);

    //Plan movement.
    task_state_t state = MachineInterface::linear_movement(axis_positions);

    return state;
}




/*
 * sanity_check : this function verifies that the required destination coordinates are valid.
 *
 *  It verifies that coordinates on each axis are not negative (beyond the endstop) and that the space between
 *      carriages allows them not to be in contact.
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


//-----------------------------------------------------Offsets---------------------------------------------------

task_state_t MachineController::set_current_position(offsets_state_t new_position) {

    /* As we must check all 4 offsets, the exact same way, a macro will be used.
     * This macro check the flag for the provided axis, and if set, updates the current offset with the given value.
     */
#define CHECK_AXIS(axis)\
    if (new_position.axis##_flag) {\
        offsets.axis = current_position.axis - new_position.axis;\
    }

    //Eventually modify the offset for all 4 axis (x, y, z, e)
    CHECK_AXIS(x);
    CHECK_AXIS(y);
    CHECK_AXIS(z);
    CHECK_AXIS(e);

#undef CHECK_AXIS;

    return complete;
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

/*
 * set_carriages_state : this function can modify all parameters related to extrusion :
 *
 *  - the working extruder;
 *  - the working extruder's speed;
 *  - (nominative) extruders'speeds.
 *
 *  Data will be modified only if flags related to it are set.
 */

task_state_t MachineController::set_carriages_state(carriages_state_t new_state) {

    //Initialise a state, that will be updated at each modification.
    task_state_t state = complete;

    //if the working extruder must be changed :
    if (new_state.working_carriage_flag) {

        //Change the working extruder
        state = set_working_extruder(new_state.working_carriage);

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
    SPEED_MODIFICATION(extrusion_state.working_carriage, new_state.working_carriage_speed_flag,
                       new_state.working_carriage_speed)

    //Eventually modify the speed for every carriage
    SPEED_MODIFICATION(new_state.nominative_carriage, new_state.nominative_speed_mod_flag, new_state.nominative_speed)

    //For safety and good practices, undef the macro.
#undef SPEED_MODIFICATION

}

/*
 * set_working_extruder : sets the working extruder.
 *
 *  As the process requires to check the mode and the extruder id, this separate function exists.
 */

task_state_t MachineController::set_working_extruder(uint8_t carriage) {

    //Nothing to do if the carriage id the current one.
    if (carriage == extrusion_state.working_carriage) {
        return complete;
    }

    switch (mode) {
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
    extrusion_state.working_carriage = carriage;

    //Succeed.
    return complete;

}


/*
 * get_extrusion_state : this function returns the current state of the cooling.
 *
 */

const MachineController::carriages_state_t MachineController::get_extrusion_state() {

    return extrusion_state;

}

//-------------------------------Cooling-------------------------------

/*
 * set_cooling_state : this function can modify all parameters related to cooling, namely :
 *  - cooling power;
 *  - cooling enable state (enabled, disabled).
 *
 *  Parameters will be modified only if their flags are set.
 */

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

    return complete;

}


/*
 * get_cooling_state : this function returns the current state of the cooling.
 */

const MachineController::cooling_state_t MachineController::get_cooling_state() {

    return cooling_state;

}

//-------------------------------Static declarations - definitions-------------------------------


MachineController::cooling_state_t MachineController::cooling_state = MachineController::cooling_state_t();

MachineController::carriages_state_t MachineController::extrusion_state = MachineController::carriages_state_t();

MachineController::hl_coord_t MachineController::current_position = MachineController::hl_coord_t();

MachineController::hl_coord_t MachineController::offsets = MachineController::hl_coord_t();

float tmpos[NB_AXIS];
float *const MachineController::axis_positions = tmpos;


uint8_t MachineController::mode = 0;