/*
  SimpleMultiRotor.h - Part of TRACER

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


#ifndef TRACER_MULTIROTOR_H
#define TRACER_MULTIROTOR_H

/*
 * The SolidMultiRotor class.
 *
 *  This class regulates the power for all drones that conform to the following model :
 *
 *      - at least one single motor;
 *      - more than (>=) 4 motors;
 *      - all motors are fixed, and push vertically in the drone's referential
 */


#include "stdint.h"

#include <LinearSolver/Matrix.h>

#include <LinearSolver/LinearSystem.h>

class SolidMultiRotor {


    //------------------------------- Init -------------------------------

protected:

    //Constructor;
    SolidMultiRotor();

    //Destructor;
    ~SolidMultiRotor();

    //------------------------------- Virtual methods -------------------------------

public:

    struct coordinate_system_t {

        //Enable the x control;
        bool x_en = false;

        //Enable the y control;
        bool y_en = false;

        //Enable the z control;
        bool z_en = false;

        //Enable the pitch control;
        bool pitch_en = false;

        //Enable the roll control;
        bool roll_en = false;

        //Enable the yaw control;
        bool yaw_en = false;

    };


protected:

    //Set the coordinates that will figure in the coordinate system;
    virtual void setCoordinateSystem(coordinate_system_t *coordinate_system) = 0;

    //The method to create motors;
    virtual void createMotors() = 0;

    //The method to create relations;
    virtual void createRelations(LinearSystem *s) = 0;


    //----------------------------------- Resolution -----------------------------------

protected:

    //Solve the physical control system;
    void solve();


    //------------------------------- Model configuration -------------------------------

protected:

    /*
     * The structure that will contain all data related to one motor;
     */
    struct motor_data_t {

        //Motor coordinates;
        float x = 0, y = 0, z = 0;

        //Motor orientation
        float theta = 0, phi = 0;

        //The motor's Kv;
        float kV = 1000;

        //The motor's traction and torque coefficients;
        float traction_coefficient = 1, torque_coefficient = 1;

        //The power voltage;
        float voltage = 11;

        //Does the motor turns in positive_rotation direction?
        bool rotation_direction = false;

        //The maximum signal value;
        float max_signal = 255;

        //The index of the servo controlling the motor;
        uint8_t servo_index = 0;

    };

    //Add a single motor to the model and return its index;
    void addMotor(motor_data_t *motor_data);



private:

    //The motors lock flag : when it is set, no motors can be added, but relations can;
    bool motors_locked;


    //------------------------------- Fields -------------------------------

protected:

    //The power matrix, used for computation and so shared with sub-classes;
    Matrix *powerMatrix;


private:

    //The number of single motors;
    uint8_t nbMotors;

    //The declared single motors;
    motor_data_t *motors;

    //The number of coordinates;
    uint8_t nbCoordinates;

    //The initialisation flag : will be used to verify if the initialisation procedure was completed;
    bool initFlag;


    //------------------------------- Computation -------------------------------

    void resetMotorsArray();

    uint8_t getCoordinatesNumber(coordinate_system_t *coordinate_system);

    bool checkControl(const Matrix *m, coordinate_system_t *coordinates, float threshold);

    float infiniteNorm(const float *matrix_line, uint8_t size);

    void failSafe();

    void addMotorsEquations(coordinate_system_t *coordinates, LinearSystem *system);
};


#endif //TRACER_MULTIROTOR_H
