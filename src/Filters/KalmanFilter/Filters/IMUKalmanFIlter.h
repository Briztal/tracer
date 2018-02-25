//
// Created by root on 2/24/18.
//

#ifndef TRACER_IMEKALMANFILTER_H
#define TRACER_IMEKALMANFILTER_H


#include <Filters/KalmanFilter/KalmanFilter.h>


class IMUKalmanFIlter {


    //------------------------------------------------- Initialisation -------------------------------------------------

    //Constructor;
    IMUKalmanFIlter(float *speed_conversion_factord, float measure_period, Matrix **process_noises,
                    Matrix **measure_noises);

    //Build the prediction matrix;
    Matrix *getPredictionMatrix(float time);

    //Build a transformation matrix;
    Matrix *getTranformationMatrix(float conversion_factor);

    //Destructor;
    ~IMUKalmanFIlter();

    //------------------------------------------------- Setup -------------------------------------------------

    //Set initial values for gravity and gyro biais;
    void setInitialData(float *accelerometer_averages, float *gyro_biais_averages);


    //------------------------------------------------- Iteration -------------------------------------------------

    //Update the internal model with a new set of measures;
    void update(float *accelero_data, float *gyro_data);

    //Get current gravity angles;
    void getGravityAngles(float *angles);

    //Get current rotation speeds;
    void getGyroSpeeds(float *speed);

    //Get current accelerations on x, y and z;
    void getAccelerations(float *accelerations);


    //------------------------------------------------- Filters -------------------------------------------------

private:

    //Filters for all axis;
    KalmanFilter **filters;


    //------------------------------------------------- Data -------------------------------------------------

private:

    //Gravity angles;
    float *const gravityAngles;

    //Angular speeds;
    float *const angularSpeeds;

};


#endif //TRACER_IMEKALMANFILTER_H
