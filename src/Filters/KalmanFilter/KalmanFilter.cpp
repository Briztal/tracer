/*
  KalmanFilter.cpp - Part of TRACER

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


#include "KalmanFilter.h"


//--------------------------------------------------- Initialisation ---------------------------------------------------

/*
 * Constructor : takes sizes, initial and constant values, and rebuilds them safely;
 */

KalmanFilter::KalmanFilter(uint8_t stateSize, uint8_t measureSize, float *initialState, Matrix *initialP, Matrix *A,
                           Matrix *H, Matrix *Q, Matrix *R)
        : stateSize(stateSize), measureSize(measureSize),
          x(new float[stateSize]), P(new Matrix(initialP)),
          A(new Matrix(stateSize, stateSize, A)),
          At(new Matrix(stateSize, stateSize, A)),
          H(new Matrix(measureSize, stateSize, H)),
          Ht(new Matrix(measureSize, stateSize, H)),
          Q(new Matrix(stateSize, stateSize, Q)),
          R(new Matrix(measureSize, measureSize, R)),


          P_temp(new Matrix(stateSize, stateSize)),
          state_square_temp(new Matrix(stateSize, stateSize)),
          KG_temp_S(new Matrix(measureSize, measureSize)),
          KG_temp(new Matrix(stateSize, measureSize)) {

    //Transposed matrices have been built but not transposed.
    At->transpose();
    Ht->transpose();

    //All matrices have been properly copied, but the initial state hasn't. We must copy it.
    memcpy(x, initialState, stateSize * sizeof(float));

}


/*
 * All internal data has been instantiated at construction time, so we can delete them safely;
 */

KalmanFilter::~KalmanFilter() {

    //Delete the state vector;
    delete[] x;

    //Delete the state covariance matrix;
    delete P;

    //Delete the prediction matrix;
    delete A;

    //Delete the transposed of the prediction matrix;
    delete At;

    //Delete the transformation matrix;
    delete H;

    //Delete the transposed of the transformation matrix;
    delete Ht;

    //Delete the process noise matrix;
    delete Q;

    //Delete the measure noise matrix;
    delete R;


    //Temp matrices :

    delete P_temp;
    delete state_square_temp;
    delete KG_temp_S;
    delete KG_temp;
}

//----------------------------------------------------- Computation ----------------------------------------------------

/*
 * Compute : this function will execute all steps described in the doc file in the current folder;
 */

void KalmanFilter::compute(const float *const newMeasure) {

    //First, create a local array that will contain the predicted state;.
    float newState[stateSize];

    //Then, create a local matrix for the covariance matrix of the predicted state;
    Matrix *newP = P_temp;


    /*
     * Step 0 : Prediction.
     *
     * Predict the new state and update the new covariance matrix;
     */

    predict(newState, newP);


    /*
     * Step 1 : Measure.
     *
     * Get the innovation from the measure and the predicted state;
     */

    //Create an array that will contain the innovation;
    float innovation[measureSize];

    //Compute the innovation;
    computeInnovation(newState, newMeasure, innovation);


    /*
     * Step 2 : Computation.
     *
     * We will compute the kalman gain;
     */

    //Create a local matrix to contain the kalman gain;
    Matrix *kalmanGain = new Matrix(stateSize, measureSize);

    //Compute the kalman gain;
    computeKalmanGain(newP, kalmanGain);


    /*
     * Step 3 : Update.
     *
     * We will compute the new state and its covariance matrix;
     */

    //Update the state;
    update(newState, innovation, newP, kalmanGain);

}


/*
 * predict : this function predicts the next state and computes its covariance matrix;
 */

void KalmanFilter::predict(float *newState, Matrix *newP) {

    //Determine the next state, by applying A to the current state;
    A->apply(x, newState);


    /*
     * Covariance matrix computation;
     *
     * P+ = A * P * At + Q (matrix)
     */

    //Initialise a new matrix to A;
    newP->setTo(Q);

    //Cache the temp matrix;
    Matrix *temp = state_square_temp;

    //Multiply A and P and save the result in temp;
    Matrix::multiply(A, P, temp);

    //Multiply AP and At and add the result to newP
    Matrix::multiplyAndAdd(temp, At, newP);

}


/*
 * computeInnovation : this function computes the innovation from the measure and the current state;
 */

void KalmanFilter::computeInnovation(float *newState, const float *const newMeasure, float *innovation) {

    //Compute H * X and save it in innovation;
    H->apply(newState, innovation);

    //For every coordinate of the innovation :
    for (uint8_t i = 0; i < measureSize; i++) {

        //Compute the i-th coordinate of M - H * X and save it in innovation;
        innovation[i] = newMeasure[i] - innovation[i];

    }

}


/*
 * computeKalmanGain : this function will compute the Kalman Gain for the covariance matrix of the predicted state;
 */

void KalmanFilter::computeKalmanGain(Matrix *newP, Matrix *kalmanGain) {

    //Cache S and Si;
    Matrix *S = KG_temp_S;

    //Set S as a copy of Q;
    S->setTo(Q);

    //Cache the first temp matrix;
    Matrix *temp = KG_temp;

    //Multiply P+ and Ht;
    Matrix::multiply(newP, Ht, temp);

    //Multiply H and P+ * Ht and sum the result to S;
    Matrix::multiplyAndAdd(H, temp, S);

    //Compute the inverse of S;
    Matrix *Si = S->getInverse();

    //temp0 still contains P+ * Ht;

    //Multiply P+ * Ht and S-1 into the kalman gain matrix;
    Matrix::multiply(temp, Si, kalmanGain);


}


/*
 * update : this function will compute the new state and its covariance matrix;
 */

void
KalmanFilter::update(const float *const newState, const float *const innovation, Matrix *newP, Matrix *kalmanGain) {

    //Compute K * Y and save it in innovation;
    H->apply(innovation, x);

    //For every coordinate of the innovation :
    for (uint8_t i = 0; i < measureSize; i++) {

        //Compute the i-th coordinate of X+ - K * Y and save it in innovation;
        x[i] = newState[i] - x[i];

    }

    //Cache the temp matrix;
    Matrix *temp = state_square_temp;

    //Set temp to the identity matrix;
    Matrix::setIdentityMatrix(temp);

    //Subtract K * H to the Identity matrix;
    Matrix::multiplyAndSubtract(kalmanGain, H, temp);

    //Compute (I - K * h ) * P+ and save it in P;
    Matrix::multiply(temp, newP, P);

}


//-------------------------------------------------------- Query -------------------------------------------------------


/*
 * getState : this function copies the current state in the given buffer.
 *
 * No check is made on the size, the array must be of the correct size;
 */

void KalmanFilter::getState(float *dst) {

    //Fast copy of the state array;
    memcpy(dst, x, stateSize * sizeof(float));

}
