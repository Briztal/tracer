/*
  SimpleMultiRotor.cpp - Part of TRACER

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

#include <malloc.h>
#include <Interaction/Interaction.h>
#include <DataStructures/Matrix.h>
#include "SolidMultiRotor.h"

SolidMultiRotor::SolidMultiRotor() : nb_single_motors(0), motors(nullptr), motors_locked(false) {

    //Create a coordinate system struct;
    coordinate_system_t coordinate_system = coordinate_system_t();

    //First, let the sub-class declare its coordinate system;
    setCoordinateSystem(&coordinate_system);

    //Then, let the sub-class create all motors;
    createMotors();

    //Provide any other motor to be added;
    motors_locked = true;

    //Verify that enough motors have been added
    bool motor_nb_flag = checkMotorNumber(&coordinate_system, nb_single_motors);

    //If there are not enough motors, fail
    if (!motor_nb_flag) {

        //Log;
        std_out("Error in SolidMultiRotor::SolidMultiRotor : You didn't provide a sufficient number of motor to "
                        "be able to controll all your coordinates.");

        //TODO SET DEFAULT MATRIX;

        //Fail;
        return;

    }
}


SolidMultiRotor::~SolidMultiRotor() {

    //Free the motors arrays;
    freeMotorsArrays();

    //TODO FREE RELATIONS;

}

uint8_t SolidMultiRotor::addMotor(motor_data_t *motor_data) {

    //If the geometry is locked, fail;
    if (motors_locked) {

        //Log
        std_out("Error in SolidMultiRotor::addMotor : the geometry is locked, no motors can be added anymore.");

        //Return the default index;
        return 0;

    }

    //First, increment the number of single motors;
    uint8_t new_index = nb_single_motors++;

    //Try to reallocate the array;
    void *new_ptr = realloc(motors, nb_single_motors * sizeof(motor_data_t));

    //If the reallocation failed :
    if (!new_ptr) {

        //Restore the number of single motors;
        nb_single_motors = new_index;

        //Log
        std_out("Error in SolidMultiRotor::addMotor : the reallocation failed.");

        //Fail with default value;
        return 0;

    }

    //Copy the new motor data at the new position;
    motors[new_index] = *motor_data;

}


void SolidMultiRotor::freeMotorsArrays() {

    //free the single motors array;
    free(motors);

    //Nullify the single motors array;
    motors = nullptr;

    //Reset the number of single motors;
    nb_single_motors = 0;

}


bool SolidMultiRotor::checkMotorNumber(coordinate_system_t *coordinate_system, uint8_t nb_motors) {

    //Initialise the number of motors;
    uint8_t i = 0;

    //Count each enabled coordinate;
    if (coordinate_system->x_en) i++;
    if (coordinate_system->y_en) i++;
    if (coordinate_system->z_en) i++;
    if (coordinate_system->pitch_en) i++;
    if (coordinate_system->roll_en) i++;
    if (coordinate_system->yaw_en) i++;

    //Return false only if there are less motors than control coordinates;
    return (i <= nb_motors);

}


/*
 * computeMotorsMatrix : This function will compute the first lines of the resolution matrix;

    The following lines must be added :

Forces : the drone is submitted to a resultant of forces F(fx, fy, fz), that verify :

    fx = sum{i} ( tx_i ) =  sum{i} ( cos(phi_i) * sin(theta_i) * t_i * R_i^2 * p_i^2 );

    fy = sum{i} ( ty_i ) =  sum{i} ( sin(phi_i) * sin(theta_i) * t_i * R_i^2 * p_i^2 );

    fz = sum{i} ( tz_i ) =  sum{i} ( cos(theta_i) * t_i * R_i^2 * p_i^2 );

    (fundamental principle of dynamics)


Momentum :

    The drone is submitted to torques created by its motor's rotations, and to a momentum M(mx, my, mz),
    created by motors traction;

    If we apply the kinetics momentum theorem, we obtain :

    mx = sum{i} (-s_i * cx_i + y_i * tz_i - z_i * ty_i)

    my = sum{i} (s_i * cy_i + z_i * tx_i - x_i * tz_i)

    mz = sum{i} (-s_i * cz_i + x_i * ty_i - y_i * tx_i)

    where s_i = 1 if the motor turns in the positive direction;

 */

bool SolidMultiRotor::computeMotorsMatrix(coordinate_system_t *coordinaes) {

    //First, cache the number of motors;
    const uint8_t size = nb_single_motors;

    //Then, create the problem's matrix : a square matrix of the problem's size;
    Matrix matrix = Matrix(size, size);

    //Now, let's add all equation's coefficients for each motor;
    for (uint8_t column_index = 0; column_index < size; column_index++) {

        //First, initialise a row counter;
        uint8_t line_index = 0;

        //Cache the motor data pointer;
        motor_data_t *data = motors + column_index;

        /*
         * The R_i coefficient will be used for all computations involving this motors;
         *         R_i = kv_i * max_voltage_i / max_i;
         */
        float R = data->kV + data->voltage / data->max_signal;

        //Cache the square of R;
        float R2 = R * R;

        //sin(theta) is used twice. We will compute it once;
        float sin_theta = sinf(data->theta);

        /*
         * Ratios : these coefficients are used to get vector (traction and momentum) coordinates;
         * rx = cos(phi_i) * sin(theta_i);
         * ry = sin(phi_i) * sin(theta_i);
         * rz = cos(theta_i);
         */
        float rx = cosf(data->phi) * sin_theta;
        float ry = sinf(data->phi) * sin_theta;
        float rz = cosf(data->theta);


        //------------------------- Traction -------------------------

        //Cache the product of the traction coefficient and R^2;
        float t_R2 = data->traction_coefficient * R2;

        /*
         * As tractions are used for momentum, we will compute them now;
         * tx_i = cos(phi_i) * sin(theta_i) * t_i * R_i^2 * p_i^2;
         * ty_i = sin(phi_i) * sin(theta_i) * t_i * R_i^2 * p_i^2;
         * tz_i = cos(theta_i) * t_i * R_i^2 * p_i^2;
         */
        float tx = rx * t_R2;
        float ty = ry * t_R2;
        float tz = rz * t_R2;


        //Add the coefficient of the x translation equation, if it is enabled;
        if (coordinaes->x_en) {

            matrix.setCoefficient(line_index++, column_index, tx);

        }

        //Add the coefficient of the y translation equation, if it is enabled;
        if (coordinaes->y_en) {

            matrix.setCoefficient(line_index++, column_index, ty);

        }

        //Add the coefficient of the z translation equation, if it is enabled;
        if (coordinaes->z_en) {

            matrix.setCoefficient(line_index++, column_index, tz);

        }


        //------------------------- Momentum -------------------------

        //Determine the sign of the rotation;
        float s = (data->rotation_direction) ? 1 : -1;

        //Cache the product of the torque coefficient and R^2;
        float c_R2 = data->torque_coefficient * R2;

        /*
         * Now we will determine motor torques;
         * cx_i = cos(phi_i) * sin(theta_i) * c_i * R_i^2 * p_i^2;
         * cy_i = sin(phi_i) * sin(theta_i) * c_i * R_i^2 * p_i^2;
         * cz_i = cos(theta_i) * c_i * R_i^2 * p_i^2;
         */
        float cx = rx * c_R2;
        float cy = ry * c_R2;
        float cz = rz * c_R2;

        //X momentum (roll) : add the coefficient of the x momentum equation, if it is enabled;
        if (coordinaes->roll_en) {

            //The roll verifies : -s_i * cx_i + y_i * tz_i - z_i * ty_i
            float roll = -s * cx + data->y * tz - data->z * ty;

            //Set the roll coefficient;
            matrix.setCoefficient(line_index++, column_index, roll);

        }

        //Y momentum (pitch) : add the coefficient of the y momentum equation, if it is enabled;
        if (coordinaes->roll_en) {

            //The pitch verifies : s_i * cy_i + z_i * tx_i - x_i * tz_i
            float pitch = s * cy + data->z * tx - data->x * tz;

            //Set the pitch coefficient;
            matrix.setCoefficient(line_index++, column_index, pitch);

        }

        //Z momentum (yaw) : add the coefficient of the z momentum equation, if it is enabled;
        if (coordinaes->yaw_en) {

            //The yaw verifies : -s_i * cz_i + x_i * ty_i - y_i * tx_i
            float yaw = -s * cz + data->x * ty - data->y * tx;

            //Set the pitch coefficient;
            matrix.setCoefficient(line_index, column_index, yaw);

        }

    }

}


void SolidMultiRotor::computePowers(float power, float rotation_x, float rotation_y, float rotation_z) {

}
