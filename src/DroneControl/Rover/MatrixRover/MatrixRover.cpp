/*
  MatrixRover.cpp - Part of TRACER

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

#include <Interaction/Interaction.h>
#include "MatrixRover.h"


/*
 * Constructor : only initialises all fields;
 */

MatrixRover::MatrixRover() = default;


/*
 * Destructor : frees the motor array, and deletes the matrix;
 */

MatrixRover::~MatrixRover() = default;


/*
 * getCoordinatesNumber : this function determines the number of enabled coordinates:
 */

uint8_t MatrixRover::getCoordinatesNumber(RoverCoordinateSystem *coordinate_system) {

    //Initialise the number of motors;
    uint8_t i = 0;

    //Count each enabled coordinate;
    if (coordinate_system->x_en) i++;
    if (coordinate_system->y_en) i++;
    if (coordinate_system->yaw_en) i++;

    //Return false only if there are less motors than control coordinates;
    return i;

}


/*
 * addMotorsEquations : This function will compute the first lines of the resolution system;
 *
 * We have three equations to add.
 * /

 Cf Rover_physics.h :

    The global x speed :

        Sx = Sum_i(Sx_i) = Sum_i( 2 * Pi * r_i * cos(theta_i) * cos(phi_i + theta_i) * w_i );


    The global y speed :

        Sy = Sum_t(Sy_i) = Sum_i( 2 * Pi * r_i * cos(theta_i) * sin(phi_i + theta_i) * w_i );


    The global momentum :

        M = Sum_i(m_i) = Sum_i( x_i * Sy_i - y_i * Sx_i );

 */

void MatrixRover::addMotorsEquations(RoverCoordinateSystem *coordinates, LinearSystem *system) {

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
        RoverMotorData *data = 0;//motors[motor_index];



        /*
         * Constants :
         *
         * The only value we will cache is the value 2 * Pi * cos(theta), that is used for both tx an ty coefficients;
         *
         * We must also cache the value of theta, as its bounds have to be controlled;
         */

        //The correct value for theta;
        float theta = thetaFormat(data->theta);

        //The coefficient;
        float coefficient = (float) 6.28318530717958647 * data->radius * cosf(theta);

        /*
         * Speeds :
         *
         *  As speeds are used for traction and momentum, we will cache them;
         */

        //The x and y speed coefficient;
        float sx = coefficient * cosf(data->phi + theta);

        float sy = coefficient * sinf(data->phi + theta);


        //Add the coefficient of the x translation equation, if it is enabled;
        if (coordinates->x_en) {

            //Set the x coefficient;
            set_coeff(coordinate_index, motor_index, sx);

            //Move to the next coordinate
            coordinate_index++;

        }

        //Add the coefficient of the y translation equation, if it is enabled;
        if (coordinates->y_en) {

            //Set the y coefficient;
            set_coeff(coordinate_index, motor_index, sy);

            //Move to the next coordinate
            coordinate_index++;

        }


        //------------------------- Momentum -------------------------

        //Z momentum (yaw) : add the coefficient of the z momentum equation, if it is enabled;
        if (coordinates->yaw_en) {

            //The yaw verifies : -s_i * cz_i + x_i * ty_i - y_i * tx_i
            float yaw = data->x * sy - data->y * sx;

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
        //TODO system->addSimpleEquation(coordinate_index, right_member_array + (coordinate_index * nb_motors), nb_motors);

    }

}


/*
 * thetaFormat : this function will determine the angular value of theta that belongs to [-Pi/2, Pi/2[;
 */

float MatrixRover::thetaFormat(float theta) {

    //As this calculation required the PI/2 and -PI/2 values, we will macro them;

#ifdef HALF_PI
#undef HALF_PI
#endif

    //The float value of PI/2;
#define HALF_PI ((float) 1.5707963267948966192313216916398)

    //The float value of -PI/2;
#define M_HALF_PI ((float) -1.5707963267948966192313216916398)


    //Force theta to be superior or equal to -PI/2
    while(theta < M_HALF_PI){
        theta += HALF_PI;
    }

    //Force theta to be strictly inferior to PI/2
    while (theta >= HALF_PI){
        theta -= HALF_PI;
    }


    //Undefine macros for safety;
#undef HALF_PI

#undef M_HALF_PI

    //Return the new value of theta;
    return theta;
}



/*
 * checkControl : this function checks that all control coordinates can be properly controlled;
 *
 * An coordinate will be controllable if the coefficients of its line  in its control matrix are great enough;
 */

bool MatrixRover::checkControl(const Matrix *m, RoverCoordinateSystem *coordinates) {

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
        float norm = 0;/*infiniteNorm(m->getLine(line_counter++), nbMotors);*/\
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
    CHECK_COORDINATE(yaw)

    //If no error, succeed;
    return true;

}

