//
// Created by root on 3/4/18.
//

#include <DataStructures/CoordinateSystems/Angles3D.h>
#include "AngleOrder1KalmanFilter.h"

/*
 * Constructor : initialises dimensions and matrices;
 */

AngleOrder1KalmanFilter::AngleOrder1KalmanFilter() : KalmanFilter(3, 2) {}


/*
 * This function will start matrices of the kalman filter;
 */

void AngleOrder1KalmanFilter::initialise(float period, Matrix &process_noise, Matrix &measure_noise) {

    Matrix prediction(3, 3, false), transformation(2, 3, false);

    prediction.setToIdentity();
    prediction.setCoefficient(1, 0, period);


    /*
     * this function will build the following matrices :
     *
     *
     * prediction :
     *
     *      | 1  0  0 |
     *      | t  1  0 |
     *      | 0  0  1 |
     *
     *  with t = time;
     *
     * transformation :
     *
     *   | 1   0   1  |
     *   | 0   1   0  |
     *
     */

    transformation.setToIdentity();
    transformation.setCoefficient(0, 2, 1);

    //Finally, let's start all matrices;
    KalmanFilter::initialise(prediction, transformation, process_noise, measure_noise);

}


/*
 * compute : this function will compute the new state with the given angle and angular speed;
 */

void AngleOrder1KalmanFilter::compute(float angle, float angular_speed) {

    //Cache data into an array;
    float measure[2]{angular_speed, angle};

    //Compute the new state;
    KalmanFilter::compute(measure);

}

/*
 * getState : interrogates superclass and copy its data;
 */

void AngleOrder1KalmanFilter::getState(float &angle, float &angular_speed, float &biais) {

    //Cache state;
    float state[3];

    //Get state;
    KalmanFilter::getState(state);

    //Copy state;
    angular_speed = state[0];
    angle = state[1];
    biais = state[2];

}

//------------------------------------------------------ Wrappers ------------------------------------------------------


void AngleOrder1KalmanFilter::wrapMeasure(float *measure) {

    //Simply wrap the angle;
    measure[1] = Angles3D::wrapAngle_PI(measure[1]);
}

void AngleOrder1KalmanFilter::wrapState(float *state) {

    //Simply wrap the angle;
    state[1] = Angles3D::wrapAngle_PI(state[1]);

}






