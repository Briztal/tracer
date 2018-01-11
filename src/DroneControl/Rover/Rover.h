//
// Created by root on 11/01/18.
//

#ifndef TRACER_ROVER_H
#define TRACER_ROVER_H


#include <LinearSolver/LinearSystem.h>

class Rover {

    //------------------------------- Init -------------------------------

protected:

    //Constructor;
    Rover();

    //Destructor;
    virtual ~Rover();

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
    class RoverMotorData {

    public:

        //A simple constructor, setting regular position values;
        RoverMotorData(float x, float y, bool direction,
                  float traction_coeff, float torque_coeff,
                  float kV, float voltage,
                  float max_signal, uint8_t servo_index) :

                x(x), y(y), z(z), theta(0), phi(0), rotation_direction(direction),
                traction_coefficient(traction_coeff), torque_coefficient(torque_coeff),
                kV(kV), voltage(voltage), max_signal(max_signal), servo_index(servo_index)
        {}


        //The most complete constructor, setting all coefficients;
        RoverMotorData(float x, float y, float z, float theta, float phi, bool direction,
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
    void addMotor(RoverMotorData *motor_data);


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
    RoverMotorData **motors;

    //The number of coordinates;
    uint8_t nbCoordinates;

    //The initialisation flag : will be used to verify if the initialisation procedure was completed;
    bool initFlag;


    //------------------------------- Computation -------------------------------

    void deleteMotorsArray();

    uint8_t getCoordinatesNumber(coordinate_system_t *coordinate_system);

    bool checkControl(const Matrix *m, coordinate_system_t *coordinates, float threshold);

    float infiniteNorm(const float *matrix_line, uint8_t size);

    void failSafe();

    void addMotorsEquations(coordinate_system_t *coordinates, LinearSystem *system);
};


#endif //TRACER_ROVER_H
