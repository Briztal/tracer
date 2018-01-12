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

#include "SolidMultiRotor.h"

/*
 * Constructor : calls parent destructor;
 */

SolidMultiRotor::SolidMultiRotor() = default;


/*
 * Destructor : calls parent destructor;
 */

SolidMultiRotor::~SolidMultiRotor() = default;


/*
 * getCoordinatesNumber : this function determines the number of enabled coordinates:
 */

uint8_t SolidMultiRotor::getCoordinatesNumber(MultiRotorCoordinateSystem *coordinate_system) {

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
    return i;

}


/*
 * addMotorsEquations : This function will compute the first lines of the resolution matrix;
 *
 * The following lines must be added :
 * /

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


void SolidMultiRotor::addMotorsEquations(MultiRotorCoordinateSystem *coordinates, LinearSystem *system) {

    //Cache the number of motors;
    const uint8_t nb_motors = nbMotors;

    //Cache the number of control coordinates;
    const uint8_t nb_coordinates = getCoordinatesNumber(coordinates);

    /*
     * Then, create the motor equations array.
     * Equations are simple (left member comprise only a 1), so a 2D array is sufficient
     */
    float right_member_array[nb_coordinates * nb_motors];

    //Then, define a macro to ease the coefficient set;
#define set_coeff(line, column, value) right_member_array[nb_coordinates * (line) + (column)] = (value);

    //Now, let's add all equation's coefficients for each motor;
    for (uint8_t motor_index = 0; motor_index < nb_motors; motor_index++) {

        //First, solve a row counter;
        uint8_t coordinate_index = 0;

        //Cache the motor data pointer;
        MultiRotorMotorData *data = motors[motor_index];

        /*
         * The R_i coefficient will be used for all computations involving this motors;
         *         R_i = kv_i * max_voltage_i / max_i;
         */
        float R = data->kV * data->voltage / data->max_signal;

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
        if (coordinates->x_en) {

            //Set the x coefficient;
            set_coeff(coordinate_index, motor_index, tx);

            //Move to the next coordinate
            coordinate_index++;

        }

        //Add the coefficient of the y translation equation, if it is enabled;
        if (coordinates->y_en) {

            //Set the y coefficient;
            set_coeff(coordinate_index, motor_index, ty);

            //Move to the next coordinate
            coordinate_index++;

        }

        //Add the coefficient of the z translation equation, if it is enabled;
        if (coordinates->z_en) {

            //Set the z coefficient;
            set_coeff(coordinate_index, motor_index, tz);

            //Move to the next coordinate
            coordinate_index++;

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
        if (coordinates->roll_en) {

            //The roll verifies : -s_i * cx_i + y_i * tz_i - z_i * ty_i
            float roll = -s * cx + data->y * tz - data->z * ty;

            //Set the roll coefficient;
            set_coeff(coordinate_index, motor_index, roll);

            //Move to the next coordinate
            coordinate_index++;

        }

        //Y momentum (pitch) : add the coefficient of the y momentum equation, if it is enabled;
        if (coordinates->pitch_en) {

            //The pitch verifies : s_i * cy_i + z_i * tx_i - x_i * tz_i
            float pitch = s * cy + data->z * tx - data->x * tz;

            //Set the pitch coefficient;
            set_coeff(coordinate_index, motor_index, pitch);

            //Move to the next coordinate
            coordinate_index++;

        }

        //Z momentum (yaw) : add the coefficient of the z momentum equation, if it is enabled;
        if (coordinates->yaw_en) {

            //The yaw verifies : -s_i * cz_i + x_i * ty_i - y_i * tx_i
            float yaw = -s * cz + data->x * ty - data->y * tx;

            //Set the yaw coefficient;
            set_coeff(coordinate_index, motor_index, yaw);

        }

    }


    /*
     * Now that we have filled the equation array, we will transmit these equations to the linear system;
     */

    //For every equation :
    for (uint8_t coordinate_index = 0; coordinate_index < nb_coordinates; coordinate_index++) {

        //The equation is simple (only one 1 coefficient in the left member), pass only the [coordinate_index]-th line;
        system->addSimpleEquation(coordinate_index, right_member_array + (coordinate_index * nb_motors), nb_motors);

    }

}


/*
 * checkControl : this function checks that all control coordinates can be properly controlled;
 *
 * An coordinate will be controllable if the coefficients of its line  in its control matrix are great enough;
 */

bool SolidMultiRotor::checkControl(const Matrix *m, MultiRotorCoordinateSystem *coordinates) {

    //As the line format is not known without the coordinate system struct, solve a line counter;
    uint8_t line_counter = 0;

    //The threshold for the drone;
    float threshold = 1;

    //As all coordinates get checked the same way, we will do it using a macro;
#define CHECK_COORDINATE(coordinate)\
    /*If the coordinate is enabled : */\
    if (coordinates->coordinate##_en) {\
        \
        /*Determine the norm of the line*/\
        float norm = infiniteNorm(m->getLine(line_counter++), nbMotors);\
        \
        /*If the norm is below the threshold : */\
        if (norm < threshold) {\
            \
            /*Log;*/\
            std_out("Error in SolidMultiRotor::checkControl : the "#coordinate\
                " coordinate line's norm is below the given threshold");\
            \
            /*Fail;*/\
            return false;\
            \
        }\
    \
    }

    //Now, check all coordinates;
    CHECK_COORDINATE(x)
    CHECK_COORDINATE(y)
    CHECK_COORDINATE(z)
    CHECK_COORDINATE(roll)
    CHECK_COORDINATE(pitch)
    CHECK_COORDINATE(yaw)

    //If no error, succeed;
    return true;

}

