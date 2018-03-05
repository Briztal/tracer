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


#include <Interaction/Interaction.h>
#include "KalmanFilter.h"


//--------------------------------------------------- Initialisation ---------------------------------------------------

/*
 * Constructor : takes sizes, initial and constant values, and rebuilds them safely;
 */

KalmanFilter::KalmanFilter(uint8_t stateSize, uint8_t measureSize)
        : stateSize(stateSize), measureSize(measureSize),
          matricesInitialised(false),
          stateInitialised(false),
          x(new float[stateSize]),
          P(Matrix(stateSize, stateSize, false)),
          A(Matrix(stateSize, stateSize, false)),
          At(Matrix(stateSize, stateSize, false)),
          H(Matrix(measureSize, stateSize, false)),
          Ht(Matrix(stateSize, measureSize, false)),
          Q(Matrix(stateSize, stateSize, false)),
          R(Matrix(measureSize, measureSize, false)),

          P_temp(Matrix(stateSize, stateSize, false)),
          state_square_temp(Matrix(stateSize, stateSize, false)),
          KG(Matrix(stateSize, measureSize, false)),
          KG_temp(Matrix(stateSize, measureSize, false)),
          KG_temp_S(Matrix(measureSize, measureSize, false)),
          KG_temp_Si(Matrix(measureSize, measureSize, false)) {}


/*
 * initialise : this function initialises all matrices;
 */

void KalmanFilter::initialise(const Matrix &prediction_matrix, const Matrix &transformation_matrix,
                              const Matrix &process_noise, const Matrix &measure_noise) {

    if (matricesInitialised) {

        //Log;
        std_out("Error in KalmanFilter::initialise : attempted to initialise twice;");

        //Abort;
        return;

    }

    A.setTo(prediction_matrix);
    H.setTo(transformation_matrix);
    Q.setTo(process_noise);
    R.setTo(measure_noise);

    //Transposed matrices have been built but not transposed.
    At.setTransposed(prediction_matrix);
    Ht.setTransposed(transformation_matrix);

    matricesInitialised = true;


    std_out("A\n"+A.toString());
    std_out("H\n"+H.toString());
    std_out("A\n"+At.toString());
    std_out("H\n"+Ht.toString());
    std_out("Q\n"+Q.toString());
    std_out("R\n"+R.toString());



}


void KalmanFilter::setInitialState(const float *initial_state, const Matrix &initial_covariance_matrix) {

    if (!matricesInitialised) {

        //Log;
        std_out("Error in KalmanFilter::setInitialState : matrices have not been initialised;");

        //Abort;
        return;

    }

    if (stateInitialised) {

        //Log;
        std_out("Error in KalmanFilter::setInitialState : attempted to initialise twice;");

        //Abort;
        return;

    }

    //All matrices have been properly copied, but the initial state hasn't. We must copy it.
    memcpy(x, initial_state, stateSize * sizeof(float));

    //Mark the state as initialised;
    stateInitialised = true;

    //Create the new covariance matrix;
    P.setTo(initial_covariance_matrix);

}


/*
 * All internal data has been instantiated at construction time, so we can delete them safely;
 */


KalmanFilter::~KalmanFilter() {

    //Delete the state vector;
    delete[] x;

}


//----------------------------------------------------- Computation ----------------------------------------------------

/*
 * Compute : this function will execute all steps described in the doc file in the current folder;
 */

void KalmanFilter::compute(float *const measure) {

    //If the filter hasn't been initialised :
    if (!stateInitialised) {

        //Log :
        std_out("Error in KalmanFilter::compute : the filter hasn't been initialised;");

        //Fail;
        return;

    }

    //Eventually, call the wrapper on the predicted state;
    wrapMeasure(measure);

    //First, create a local array that will contain the predicted state;.
    float prediction[stateSize];

    //Then, create a local matrix for the covariance matrix of the predicted state;
    Matrix &prediction_P = P_temp;

    /*
     * Step 0 : Prediction.
     *
     * Predict the state and update the covariance matrix;
     */

    predict(prediction, prediction_P);

    //Eventually, call the wrapper on the predicted state;
    wrapState(prediction);

    //std_out("prediction : " + String(prediction[0]) + " " + String(prediction[1]) + " " + String(prediction[2]));
    //std_out("measure : " + String(measure[0]) + " " + String(measure[1]));

    /*
     * Step 1 : Measure.
     *
     * Get the innovation from the measure and the predicted state;
     */

    //Create an array that will contain the innovation;
    float innovation[measureSize];

    //Compute the innovation;
    computeInnovation(prediction, measure, innovation);

    //Eventually, call the wrapper on the predicted state;
    wrapMeasure(innovation);

    //std_out("innovation : " + String(innovation[0]) + " " + String(innovation[1]));


    /*
     * Step 2 : Computation.
     *
     * We will compute the kalman gain;
     */

    //Compute the kalman gain;
    computeKalmanGain(prediction_P, KG);


    /*
     * Step 3 : Update.
     *
     * We will compute the new state and its covariance matrix;
     */

    //Update the state;
    update(prediction, innovation, prediction_P, KG);

    //Eventually call the wrapper on the new state;
    wrapState(x);

    //std_out("state : " + String(x[0]) + " " + String(x[1]) + " " + String(x[2]));

}


/*
 * predict : this function predicts the next state and computes its covariance matrix;
 */

void KalmanFilter::predict(float *prediction, Matrix &prediction_P) {

    //Determine the next state, by applying A to the current state;
    A.apply(x, prediction);


    /*
     * Covariance matrix computation;
     *
     * P+ = A * P * At + Q (matrix)
     */

    //Initialise the prediction covatiance matrix to the process noise;
    prediction_P.setTo(Q);

    //Cache the temp matrix;
    Matrix& temp = state_square_temp;

    //Multiply A and P and save the result in temp;
    temp.multiply(A, P);

    //Multiply AP and At and add the result to prediction_P
    prediction_P.multiplyAndAdd(temp, At);

}


/*
 * computeInnovation : this function computes the innovation from the measure and the current state;
 */

void KalmanFilter::computeInnovation(const float *const prediction, const float *const measure, float *innovation) {

    //Compute H * X and save it in innovation;
    H.apply(prediction, innovation);

    //For every coordinate of the innovation :
    for (uint8_t i = 0; i < measureSize; i++) {

        //Compute the i-th coordinate of M - H * X and save it in innovation;
        innovation[i] = measure[i] - innovation[i];

    }

}


/*
 * computeKalmanGain : this function will compute the Kalman Gain for the covariance matrix of the predicted state;
 */

void KalmanFilter::computeKalmanGain(const Matrix &prediction_P, Matrix & kalmanGain) {

    //Cache S and Si;
    Matrix &S = KG_temp_S;
    Matrix &Si = KG_temp_Si;

    //Set S as a copy of Q;
    S.setTo(R);

    //Cache the first temp matrix;
    Matrix &temp = KG_temp;

    //Multiply P+ and Ht;
    temp.multiply(prediction_P, Ht);

    //Multiply H and P+ * Ht and sum the result to S;
    S.multiplyAndAdd(H, temp);

    //Compute the inverse of S;
    S.getInverse(Si);

    //temp0 still contains P+ * Ht;

    //Multiply P+ * Ht and S-1 into the kalman gain matrix;
    kalmanGain.multiply(temp, Si);

}


/*
 * update : this function will compute the next state and its covariance matrix;
 */

void
KalmanFilter::update(const float *const prediction, const float *const innovation, const Matrix &prediction_P,
                     const Matrix &kalmanGain) {

    //Compute K * Y and save it in innovation;
    kalmanGain.apply(innovation, x);


    //For every coordinate of the innovation :
    for (uint8_t i = 0; i < stateSize; i++) {

        //Compute the i-th coordinate of X+ - K * Y and save it in innovation;
        x[i] = prediction[i] + x[i];

    }




    //Cache the temp matrix;
    Matrix &temp = state_square_temp;

    //Set temp to the identity matrix;
    temp.setToIdentity();

    //Subtract K * H to the Identity matrix;
    temp.multiplyAndSubtract(kalmanGain, H);

    //Compute (I - K * h ) * P+ and save it in P;
    P.multiply(temp, prediction_P);

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
