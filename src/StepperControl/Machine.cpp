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
#include <EEPROMStorage/EEPROMStorage.h>
#include "Machine.h"
#include "MachineInterface.h"
#include "StepperController.h"
#include "TrajectoryTracer.h"
#include <interface.h>



//---------------------------------------------------Power--------------------------------------------------------------

void Machine::disable_stepper_power() {

    while(TrajectoryTracer::started);
    StepperController::disable();

}


//-------------------------------------------------Movement-------------------------------------------------------------


/* Machine Structure :
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
 */


void Machine::home_carriages(float speed) {

    position[0] = position[1] = position[2] = position[3] = 0;

    //TODO DETERMINE THE CARRIAGE THAT MOVES THE MOST AND CHOOSE THE SPEED GROUP ACCORDINGLY

    //MachineInterface::set_speed_group(0);

    //MachineInterface::set_speed_for_group(0, speed);

    MachineInterface::linear_movement(position);

}


void Machine::set_current_mode(uint8_t new_mode) {
    mode = new_mode;
}

void Machine::line_to(uint8_t carriage, float x, float y, float z, float speed) {

    switch (mode) {
        case 0:
            mode_0(carriage, x, y, z, speed);
            break;
        default:
            break;
    }
}


/*
 * mode_0 : single carriage mode.
 *
 *  In this mode, every carriage can be moved independently.
 *
 */

void Machine::mode_0(uint8_t carriage, float x, float y, float z, float speed) {


    bool x0, y0;

    MachineInterface::set_speed_group(carriage);
    MachineInterface::set_speed_for_group(carriage, speed);

    switch (carriage) {
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
            return;
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

    //sanity_check(position);

    for (uint8_t i = 0; i<NB_AXIS; i++) {
        CI::echo(String(i)+" "+String(position[i]));
    }

    MachineInterface::linear_movement(position);

    CI::echo("BITE");

}


/*
 * sanity_check : this function verifies that the required destination coordinates are valid.
 *
 *  It verifies that coordinates on each axis are not negative (beyond the endstop) and that the space between
 *      carriages allows them not to be in contact.
 *
 */
void Machine::sanity_check(float *position) {

    //First, we must check that coordinates on moving axis are not negative :
    float * f;
    for (int axis = 0; axis<5; axis++) {
        f = position + axis;
        if (*f<0) *f = 0;
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
        if (tmp<0) {
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
        if (tmp<0) {
            position[2] += tmp;
            position[3] = 0;
        }
    }


}


uint8_t Machine::mode;

float tmpos[NB_AXIS];
float *const Machine::position = tmpos;
