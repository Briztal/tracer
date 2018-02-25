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
    KalmanFilter(uint8_t state_size, uint8_t measure_size, Matrix *A, Matrix *H, Matrix *Q, Matrix *R);

    //Sets the current state estimated, and its covariance matrix;
    void setInitialState(float *initial_state, Matrix *initial_covariance_matrix);

    //Destructor : deletes all matrices;
    ~KalmanFilter();

    //------------------------ Sizes ------------------------

    //The size of the state vector;
    const uint8_t stateSize;

    //The state of the measure vector;
    const uint8_t measureSize;


    //------------------------ State vectors ------------------------

private:

    bool stateInitialised;

    //The current state;
    float *x;

    //The covariance matrix of the current state;
    Matrix *P;


    //------------------------ Constant Matrices ------------------------

private:

    //The prediction matrix, used to compute the prediction of the next state, and its transposed;
    const Matrix *const A;
    const Matrix * At;

    //The transformation matrix, used to compute the measure from the state, and its transposed;
    const Matrix *const H;
    const Matrix *Ht;

    //The covariance matrix of the process noise;
    const Matrix *const Q;

    //The covariance matrix of the measure noise;
    const Matrix *const R;


    //------------------------ Temporary Matrices ------------------------

    //The intermediary P matrix; square of size stateSize;
    Matrix *const P_temp;

    //The intermediary square matrix, of size stateSize;;
    Matrix *const state_square_temp;// = new Matrix(stateSize, stateSize);


    //The S Matrix in the KG computation; square of size measureSize;
    Matrix *const KG_temp_S;// = new Matrix(Q);

    //The first temporary matrix in the KG computation; size measureSize x stateSize;
    Matrix *const KG_temp;// = new Matrix(measureSize, stateSize);


    //------------------------ Processing ------------------------

public:

    //Compute a new state, with a new value;
    void compute(const float *const newMeasure);


private:

    //Predict the new value of the state;
    void predict(float *newState, Matrix *newP);

    //Compute the innovation;
    void computeInnovation(float *newState, const float *const measure, float *innovation);

    //compute the Kalman Gain
    void computeKalmanGain(Matrix *newP, Matrix *kalmanGain);

    //Update the state with all computed values;
    void update(const float *const newState, const float *const innovation, Matrix *newP, Matrix *kalmanGain);


    //------------------------ Query ------------------------

public:

    //Get the current estimated state;
    void getState(float *dst);


};


#endif //TRACER_KALMANFILTER_H
