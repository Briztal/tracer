/*
  KalmanFilter.h - Part of TRACER

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


#ifndef TRACER_KALMANFILTER_H
#define TRACER_KALMANFILTER_H


#include <Math/Matrix.h>

class KalmanFilter {

    //------------------------ Initialisation ------------------------

public:

    //Constructor : takes sizes, initial and constant values, and rebuilds them safely;
    KalmanFilter(uint8_t state_size, uint8_t measure_size);

    //Initialisation function.
    void initialise(const Matrix &prediction_matrix, const Matrix &transformation_matrix, const Matrix &process_noise, const Matrix &measure_noise);

    //Sets the current state estimated, and its covariance matrix;
    void setInitialState(const float *initial_state, const Matrix &initial_covariance_matrix);

    //Destructor : deletes all matrices;
    ~KalmanFilter();

    //------------------------ Sizes ------------------------

    //The size of the state vector;
    const uint8_t stateSize;

    //The state of the measure vector;
    const uint8_t measureSize;


    //------------------------ State vectors ------------------------

private:

    //The matrices initialisation flag. Set when all matrices have been initialised;
    bool matricesInitialised;

    //The state initialisation flag; set when the initial state has been initialised;
    bool stateInitialised;

    //The current state;
    float *const x;

    //The covariance matrix of the current state;
    Matrix P;


    //------------------------ Constant Matrices ------------------------

private:

    //The prediction matrix, used to compute the prediction of the next state, and its transposed;
    Matrix A;
    Matrix At;

    //The transformation matrix, used to compute the measure from the state, and its transposed;
    Matrix H;
    Matrix Ht;

    //The covariance matrix of the process_t noise;
    Matrix Q;

    //The covariance matrix of the measure noise;
    Matrix R;


    //------------------------ Temporary Matrices ------------------------

    //The intermediary P matrix; square of size stateSize;
    Matrix P_temp;

    //The intermediary square matrix, of size stateSize;;
    Matrix state_square_temp;// = new Matrix(stateSize, stateSize);

    //The matrix in the KG computation; size measureSize x stateSize;
    Matrix KG;// = new Matrix(measureSize, stateSize);

    //The first temporary matrix in the KG computation; size measureSize x stateSize;
    Matrix KG_temp;// = new Matrix(measureSize, stateSize);

    //The S Matrix in the KG computation; square of size measureSize;
    Matrix KG_temp_S;// = new Matrix(Q);

    //The inverse of S Matrix in the KG computation; square of size measureSize;
    Matrix KG_temp_Si;// = new Matrix(Q);


    //------------------------ Processing ------------------------

public:

    //Compute a new state, with a new value;
    void compute(float *const measure);


private:

    //Predict the new value of the state;
    void predict(float *prediction, Matrix &prediction_P);

    //Compute the innovation;
    void computeInnovation(const float *prediction, const float *measure, float *innovation);

    //compute the Kalman Gain
    void computeKalmanGain(const Matrix &prediction_P, Matrix &kalmanGain);

    //Update the state with all computed values;
    void update(const float *const prediction, const float *const innovation, const Matrix &prediction_P,
                const Matrix &kalmanGain);


    /*
     * post process_t methods to wrap vectors in their coordinate systems;
     */

protected:

    //A post process_t function to eventually wrap the innovation in the correct coordinate system;
    virtual void wrapMeasure(float *measure) {};

    //A post process_t function to eventually wrap the state in the correct coordinate system;
    virtual void wrapState(float *innovation) {};


    //------------------------ Query ------------------------

public:

    //Get the current estimated state;
    void getState(float *dst);


};


#endif //TRACER_KALMANFILTER_H
