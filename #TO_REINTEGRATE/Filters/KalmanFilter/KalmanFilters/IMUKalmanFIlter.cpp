//
// Created by root on 2/24/18.
//

#include <ControlSystem/DataConversion/VectorConversion/AnglesToVector3D.h>
#include <ControlSystem/DataConversion/VectorConversion/Vector3DToAngles.h>
#include "IMUKalmanFIlter.h"



/*
Matrix *process_noise = new Matrix(3, 3);
Matrix *measure_noise = new Matrix(3, 3);

Matrix::setIdentityMatrix(process_noise);
process_noise->divideBy(100);

Matrix::setIdentityMatrix(measure_noise);
measure_noise->divideBy(100);

std_out(process_noise->toString());

std_out(measure_noise->toString());


float initial[3] {0, 0, 0};

Matrix *initialP = new Matrix(3, 3);
Matrix::setIdentityMatrix(initialP);

initialP->divideBy(10);

 */

/*
 * Constructor : initialises filters, with adequate matrices;
 */

IMUKalmanFIlter::IMUKalmanFIlter(float measure_period, Matrix **process_noises,
                                 Matrix **measure_noises)
        : gravity(Vector3D(1, 0, 0)), angularSpeeds() {

    gravity = Vector3D(1,0,0);

    initialise(measure_period, process_noises, measure_noises);

}



/*
 * Simplified constructor : same conversion factor, process_t noise and measure noise for all axis;;
 */

IMUKalmanFIlter::IMUKalmanFIlter(float measure_period, Matrix &process_noise, Matrix &measure_noise)
        : gravity(0,0,0), angularSpeeds(0,0,0) {

    //Create a temp array containing tree times the process_t noise pointer;
    Matrix *process_noises[3]{&process_noise, &process_noise, &process_noise};

    //Create a temp array containing tree times the process_t measure pointer;
    Matrix *measure_noises[3]{&measure_noise, &measure_noise, &measure_noise};

    //Initialise all data;
    initialise(measure_period, process_noises, measure_noises);

}


/*
 * initialise : this function will start all data;
 */

void IMUKalmanFIlter::initialise(float measure_period, Matrix **process_noises, Matrix **measure_noises) {

    //For each axis (x, y, z) :
    for (uint8_t i = 0; i < 3; i++) {

        //Initialise the i-th filter, with
        filters[i].initialise(measure_period, *process_noises[i], *measure_noises[i]);


        //TODO ENLEVER
        float initial[2]{0};
        Matrix initialP(3,3, false);
        initialP.setToIdentity();
        initialP.divideBy((10));

        filters[i].setInitialState(initial, initialP);

    }

}


/*
 * Destructor : this function deletes the three kalman filters;
 */

IMUKalmanFIlter::~IMUKalmanFIlter() {}


/*
 * setInitialData : this function will set initial data for all three kalman filters
 */
void IMUKalmanFIlter::setInitialData(Vector3D &accelerometer_averages, Angles3D &gyro_biais_averages) {

    //TODO.

}


void IMUKalmanFIlter::update(Vector3D &sum_forces, Triplet &gyro_data) {

    //Declare the gravity angles;
    Angles3D gravity_angles;

    //Convert the acceleration vector to angles;
    Vector3DToAngles::convert(sum_forces, gravity_angles);

    const float *const gyro = gyro_data.get_data();
    const float *const angles_p = gravity_angles.get_data();

    //Update all filters;
    for (uint8_t axis_index = 0; axis_index < 3; axis_index++) {

        //Update the filter;
        filters[axis_index].compute(angles_p[axis_index], gyro[axis_index]);


    }




    //Now we will readall data from filters;

    //For each axis :
    for (uint8_t axis_index = 0; axis_index < 3; axis_index++) {

        float angle, speed, biais;

        //Get the computed state from the filter;
        filters[axis_index].getState(angle, speed, biais);

        //Copy the angular speed into the class's array;
        angularSpeeds.set(axis_index, speed);

        //Copy the gravity angle into the local array;
        gravity_angles.set(axis_index, angle);

    }

    //Now, we must compute the gravity's coordinates;
    AnglesToVector3D::convert(gravity_angles, gravity);

}


/*
 * getGravityAngles : copies local gravity angles to external array;
 */

void IMUKalmanFIlter::getGravity(Vector3D &gravity_c) {

    //Copy all coordinates;
    gravity_c.clone(gravity);

}


/*
 * getGyroSpeeds : copies local angular speeds to external array;
 */

void IMUKalmanFIlter::getGyroSpeeds(Triplet &speeds) {

    speeds.clone(angularSpeeds);

}


void IMUKalmanFIlter::getAccelerations(float *accelerations) {

    //TODO EVENTUALLY;
}
