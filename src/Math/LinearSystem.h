/*
  LinearSystem.h - Part of TRACER

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

#ifndef TRACER_LINEARSOLVER_H
#define TRACER_LINEARSOLVER_H

#include "CoefficientArray.h"

#include "Matrix.h"

/*
 * The LinearSystem class. This class intends to solve equations systems that can be represented like the following :
 *
 *
 *  (S) : A * X = B * Y
 *
 *  with :
 *      - X : a vector of variables, the outputs,  of size x_size;
 *      - Y : a vector that we would know, the inputs of the system, OF SIZE y_size < x_size !!!!!
 *      - A : an invertible square matrix of size x_size;
 *      - B : a rectangle matrix, of size x_size * y_size;
 *
 *
 *  The attentive reader will have notices that in difference to standard systems, there are less input variables than
 *      output variables. The B matrix's role is to re-equilibrate the degree of the system, in introducing a
 *      linear combination of Y's values, to provide B * Y, a vector of size x_size;
 *
 *  As the matrix A can be inverted, the solution system can be expressed like below :
 *
 *      (S =>) : X = Ai * B * Y
 *
 *      where Ai is the inverse of the matrix;
 *
 *  The aim of this class is to compute Ai * B, and eventually to provide an easy way to build A and B;
 */

class LinearSystem {

    //----------------------------------------- Initialisation -----------------------------------------

public:

    //Constructor;
    LinearSystem(uint8_t nb_outputs);

    //Destructor;
    ~LinearSystem() = default;


    //----------------------------------------- Resolution -----------------------------------------

public:

    //Compute the resolution matrix;
    void solve(const Matrix &A, const Matrix &B, Matrix &result);


    //----------------------------------------- Fields -----------------------------------------

private:

    //The size of the output vector;
    const uint8_t nbOutputs;

    //The temporary matrix that will contain the inverse of A;
    Matrix temp_Ai;

};



#endif //TRACER_LINEARSOLVER_H
