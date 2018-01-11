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


#include <LinearSolver/Matrix.h>

#include <LinearSolver/LinearSystem.h>

class SolidMultiRotor {


    //------------------------------- Init -------------------------------

protected:

    //Constructor;
    SolidMultiRotor();

     //Destructor;
     virtual ~SolidMultiRotor();

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
     virtual void solve();


    //------------------------------- Model configuration -------------------------------

protected:

    /*
     * The structure that will contain all data related to one motor;
     */
    class MotorData {

    public:

        //A simple constructor, setting regular position values;
        MotorData(float x, float y, float z, bool direction,
                  float traction_coeff, float torque_coeff,
                  float kV, float voltage,
                  float max_signal, uint8_t servo_index) :

                x(x), y(y), z(z), theta(0), phi(0), rotation_direction(direction),
                traction_coefficient(traction_coeff), torque_coefficient(torque_coeff),
                kV(kV), voltage(voltage), max_signal(max_signal), servo_index(servo_index)
        {}


        //The most complete constructor, setting all coefficients;
        MotorData(float x, float y, float z, float theta, float phi, bool direction,
                  float traction_coeff, float torque_coeff,
                  float kV, float voltage,
                  float max_signal, uint8_t servo_index) :

                x(x), y(y), z(z), theta(theta), phi(phi), rotation_direction(direction),
                traction_coefficient(traction_coeff), torque_coefficient(torque_coeff),
                kV(kV), voltage(voltage), max_signal(max_signal), servo_index(servo_index)
        {}

    public:

        //------------------- Position constants -------------------

        //Motor coordinates;
        float x, y, z;

        //Motor orientation
        float theta, phi;

        //Does the motor turns in trigonometric direction?
        bool rotation_direction;

        //------------------- Dynamics constants -------------------

        //The motor's traction and torque coefficients;
        float traction_coefficient;

        float torque_coefficient;

        //------------------- Voltage values -------------------
        //The motor's Kv;
        float kV;

        //The power voltage;
        float voltage;


        //------------------- Command values -------------------

        //The maximum signal value;
        float max_signal;

        //The index of the servo controlling the motor;
        uint8_t servo_index;


    };

    //Add a single motor to the model and return its index;
    void addMotor(MotorData *motor_data);


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
    MotorData **motors;

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
