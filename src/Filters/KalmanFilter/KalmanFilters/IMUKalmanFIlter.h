//
// Created by root on 2/24/18.
//

#ifndef TRACER_IMEKALMANFILTER_H
#define TRACER_IMEKALMANFILTER_H


#include <Filters/KalmanFilter/KalmanFilter.h>
#include <DataStructures/CoordinateSystems/Triplet.h>
#include "AngleOrder1KalmanFilter.h"


class IMUKalmanFIlter {


    //------------------------------------------------- Initialisation -------------------------------------------------

public:

    //Constructor;
    IMUKalmanFIlter(float measure_period, Matrix **process_noises, Matrix **measure_noises);

    //Simplified constructor;
    IMUKalmanFIlter(float measure_period, Matrix &process_noise, Matrix &measure_noise);

    //Initialise data;
    void initialise(float measure_period, Matrix **process_noises, Matrix **measure_noises);

    //Destructor;
    ~IMUKalmanFIlter();


    //------------------------------------------------- Setup -------------------------------------------------

private:

    //Set initial values for gravity and gyro biais;
    void setInitialData(Vector3D &accelerometer_averages, Angles3D &gyro_biais_averages);


    //------------------------------------------------- Iteration -------------------------------------------------

public:

    //Update the internal model with a new set of measures;
    void update(Vector3D &sum_forces, Triplet &gyro_data);

    //Get current gravity angles;
    void getGravity(Vector3D &gravity);

    //Get current rotation speeds;
    void getGyroSpeeds(Triplet &speed);

    //Get current accelerations on x, y and z;
    void getAccelerations(float *accelerations);



    //------------------------------------------------- Data -------------------------------------------------

private:

    //Gravity angles;
    Vector3D gravity;

    //Angular speeds;
    Triplet angularSpeeds;


    //------------------------------------------------- KalmanFilters -------------------------------------------------

private:

    //Kalman filters, one for each axis;
    AngleOrder1KalmanFilter filters[3]{AngleOrder1KalmanFilter(), AngleOrder1KalmanFilter(), AngleOrder1KalmanFilter()};



};


#endif //TRACER_IMEKALMANFILTER_H
