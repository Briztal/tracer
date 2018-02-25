//
// Created by root on 2/24/18.
//

#include <Interaction/Interaction.h>
#include "IMUKalmanFIlter.h"

/*
 * Constructor : initialises filters, with adequate matrices;
 */

IMUKalmanFIlter::IMUKalmanFIlter(float *conversion_factors, float measure_period, Matrix **process_noises,
                                 Matrix **measure_noises) 
        : gravityAngles(new float[3]), angularSpeeds(new float[3]) {


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

    //The prediction matrix will be the same for all three axis;
    Matrix *prediction = getPredictionMatrix(measure_period);

    //The transformation matrix depends on the conversion factor of each axis, not defined here;
    Matrix *transformation;

    //To ease the initialisation process, we will iterate over all three axis;

    //For each axis (x, y, z) :
    for (uint8_t i = 0; i < 3; i++) {

        //Initialise the transformation matrix with the correct factor;
        transformation = getTranformationMatrix(conversion_factors[i]);

        //Initialise the i-th filter, with
        filters[i] = new KalmanFilter(3, 2, prediction, transformation, process_noises[i], measure_noises[i]);

        //Delete the transformation matrix;
        delete transformation;

    }

    //Now, we must delete the prediction matrix;
    delete prediction;

}

/*
 * Destructor : this function deletes the three kalman filters;
 */

IMUKalmanFIlter::~IMUKalmanFIlter() {

    //Delete all three filters;
    for (uint8_t axis_index = 0; axis_index < 3; axis_index++) {

        delete filters[axis_index];

    }

}


/*
 * getPredictionMatrix : this function will build the following matrix : 
 * 
 * 
 *      | 1  0  0 |
 *      | t  1  0 |
 *      | 0  0  1 |
 *              
 *      with t = time;
 */

Matrix *IMUKalmanFIlter::getPredictionMatrix(float time) {

    Matrix *prediction = new Matrix(3, 3);

    Matrix::setIdentityMatrix(prediction);

    prediction->setCoefficient(1, 0, time);

    std_out("Prediction Matrix :");

    std_out(prediction->toString());

}


/*
 * getTranformationMatrix : this function will build the following matrix : 
 * 
 * 
 *   | sr  0   sr |
 *   | 0   1   0  | 
 *   
 *   with sr = conversion factor;
 *   
 */

Matrix *IMUKalmanFIlter::getTranformationMatrix(float conversion_factor) {

    Matrix *transformation = new Matrix(2, 3);

    transformation->setCoefficient(0, 0, conversion_factor);
    transformation->setCoefficient(1, 1, 1);
    transformation->setCoefficient(0, 2, conversion_factor);

    std_out("Transformation Matrix :");

    std_out(transformation->toString());

}


/*
 * setInitialData : this function will set initial data for all three kalman filters
 */
void IMUKalmanFIlter::setInitialData(float *accelerometer_averages, float *gyro_biais_averages) {

    //TODO.

}


void IMUKalmanFIlter::update(float *accelero_data, float *gyro_data) {

    //Declare an array of two floats, to contain measures for all axis;
    float measure[2]{0};

    //Update all filters;
    for (uint8_t axis_index = 0; axis_index < 3; axis_index++) {

        //Update measure array;
        measure[0] = gyro_data[axis_index];
        measure[1] = accelero_data[axis_index];

        //Update the filter;
        filters[axis_index]->compute(measure);

    }

    //Now we will read and compute data from filters;
    
    //Gravity data from filters;
    float gravity_data[3];

    //For each axis :
    for (uint8_t axis_index = 0; axis_index < 3; axis_index++) {

        //Get the computed state from the filter;
        filters[axis_index]->getState(measure);

        //Copy the angular speed into the class's array;
        angularSpeeds[axis_index] = measure[0];

        //Copy the gravity angle into the local array;
        gravity_data[axis_index] = measure[1];

    }

    //Now, we must compute the gravity's polar angle and azimuth;

    //TODO

}


/*
 * getGravityAngles : copies local gravity angles to external array;
 */

void IMUKalmanFIlter::getGravityAngles(float *angles) {

    //Fast copy;
    memcpy(angles, gravityAngles, 2 * sizeof(float));

}


/*
 * getGyroSpeeds : copies local angular speeds to external array;
 */

void IMUKalmanFIlter::getGyroSpeeds(float *speeds) {

    //Fast copy;
    memcpy(speeds, angularSpeeds, 3 * sizeof(float));

}


void IMUKalmanFIlter::getAccelerations(float *accelerations) {

    //TODO EVENTUALLY;
}
