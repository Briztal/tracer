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


task_state_t MachineController::carriages_reset() {

    position[0] = position[1] = position[2] = position[3] = 0;

    //TODO DETERMINE THE CARRIAGE THAT MOVES THE MOST AND CHOOSE THE SPEED GROUP ACCORDINGLY

    //MachineInterface::set_speed_group(0);

    //MachineInterface::set_speed_for_group(0, speed);

    return MachineInterface::linear_movement(position);

}


task_state_t MachineController::line_to(float x, float y, float z) {

    //Initialise a state
    task_state_t state = complete;

    //Try to move to the specified position
    switch (mode) {
        case 0:
            state = mode_0(x, y, z);
        default:
            break;
    }

    if (state == complete) {

        //Update the local coords
        machine_coords[0] = x;
        machine_coords[1] = y;
        machine_coords[2] = z;

    }

    return state;

}


task_state_t MachineController::line_of(float x, float y, float z) {

    //Cache the coords
    float coords[4];
    memcpy(coords, machine_coords, 4 * sizeof(float));

    //Update the local coords
    coords[0] += x;
    coords[1] += y;
    coords[2] += z;

    //Initialise a state
    task_state_t state = complete;

    //Try to move to the specified position
    switch (mode) {
        case 0:
            state = mode_0(coords[0], coords[1], coords[2]);
        default:
            break;
    }

    if (state == complete) {

        //Save coords
        memcpy(machine_coords, coords, 4 * sizeof(float));

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

task_state_t MachineController::mode_0(float x, float y, float z) {

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
        position[0] = EEPROMStorage::coordinate_interface_data.x0_offset + x;
    } else {
        position[1] = EEPROMStorage::coordinate_interface_data.x1_offset - x;
    }

    if (y0) {
        position[2] = EEPROMStorage::coordinate_interface_data.y0_offset + y;
    } else {
        position[3] = EEPROMStorage::coordinate_interface_data.y1_offset - y;
    }

    position[4] = EEPROMStorage::coordinate_interface_data.z_offset - z;

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


//-------------------------------Hotends-------------------------------


/*
 * set_hotend_temperature : this function sets the required hotend's target temperature to the given value.
 *
 */

task_state_t MachineController::set_hotend_temperature(uint8_t hotend, float temp) {

    //Pass the hand to the appropriate function in TemperatureController
    return TemperatureController::set_hotend_target(hotend, temp);

}


/*
 * get_hotend_temperature : this function reads and returns the temperature of the required hotend.
 *
 */

float MachineController::get_hotend_temperature(uint8_t hotend) {

    switch (hotend) {
        case 0:
            return Thermistors::get_temperature_hotend_0();
        case 1:
            return Thermistors::get_temperature_hotend_1();
        case 2:
            return Thermistors::get_temperature_hotend_2();
        case 3:
            return Thermistors::get_temperature_hotend_3();
        default:
            return 0;

    }


}

task_state_t MachineController::enable_hotend_regulation(uint8_t hotend, bool state) {

    //Pass the hand to the appropriate function in TemperatureController
    return TemperatureController::enable_hotend_regulation(hotend, state);

}


bool MachineController::is_hotend_regulation_enabled(uint8_t hotend) {

    return complete;

}


//-------------------------------Hotbed-------------------------------


/*
 * set_hotbed_temperature : this function sets hotbed's target temperature to the given value.
 *
 */

task_state_t MachineController::set_hotbed_temperature(float temp) {

    //Pass the hand to the appropriate function in TemperatureController
    return TemperatureController::set_hotbed_temperature(temp);

}


/*
 * get_hotbed_temperature : this function reads and returns the temperature of the hotbed.
 *
 */

float MachineController::get_hotbed_temperature() {

    //Simply read the hotbed thermistor value.
    return Thermistors::get_temperature_hotbed();

}


task_state_t MachineController::enable_hotbed_regulation(bool enable) {

    //Pass the hand to the appropriate function in TemperatureController
    return TemperatureController::enable_hotbed_regulation(enable);

}


bool MachineController::is_hotbed_regulation_enabled() {

    return complete;

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

