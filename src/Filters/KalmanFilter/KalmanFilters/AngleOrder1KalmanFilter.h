//
// Created by root on 3/4/18.
//

#ifndef TRACER_ANGLEORDER1KALMANFILTER_H
#define TRACER_ANGLEORDER1KALMANFILTER_H


#include <Filters/KalmanFilter/KalmanFilter.h>

class AngleOrder1KalmanFilter : private KalmanFilter {

public:

    //Constructor;
    AngleOrder1KalmanFilter();

    //Initialiser;
    void initialise(float period, Matrix &process_noise, Matrix &measure_noise);

    //Initial state set : copycat of the parent function;
    void setInitialState(float *initial_state, Matrix &initial_P) {
        KalmanFilter::setInitialState(initial_state, initial_P);
    };

    //Compute the state;
    void compute(float angle, float angular_speed);

    //Get the computed state;
    void getState(float &angle, float &angular_speed, float &gyro_biais);

};


#endif //TRACER_ANGLEORDER1KALMANFILTER_H
