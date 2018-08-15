/*
  MatrixDrone.h - Part of TRACER

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

#ifndef TRACER_MATRIXDRONE_H
#define TRACER_MATRIXDRONE_H

#include <Math/LinearSystem.h>
#include <Kernel/Interaction/Interaction.h>

/*
 * MatrixDrone : this class is an abstract base for variable geometry drones models, with the condition that
 *  their behavior can be represented by a matrix system, when the geometry is fixed;
 *
 *  ie to a fixed geometry corresponds and unique Matrix system.
 *
 *  This system defines the model of the drone, ie the relation between the targets and command signals.
 *
 *  Targets can be of multiple types (forces, speeds), as can be command signals (speed, power targets);
 *
 *
 *  This class is a pure virtual model specification, that implements a set of functions for our abstract drone model;
 *
 */

class MatrixDrone {


    //------------------------------- public methods -------------------------------

public:

    /*
     * Update geometry :
     *
     *  this function may be used to regenerate the matrix model, after an effective geometry modification;
     *
     *  This function depends on the geometry, and so, only calls virtual pure functions;
     */

    virtual void updateGeometry(float *geometryParameters);


    /*
     * Compute : this function will compute command signals for a given array of targets;
     *
     *  It results in a simple matrix multiplication, and so, is implemented in this class;
     */

    void compute(float *targets);

    //------------------------------- Virtual methods -------------------------------


protected:

    //The method to create relations;
    virtual void createRelations(Matrix &A, Matrix &B) = 0;



    //------------------------------- Fields -------------------------------

protected:

    //The power matrix, used for computation and so shared with sub-classes;
    Matrix systemMatrix;


protected:

    //The number of single motors;
    uint8_t nbMotors;


private:

    //The number of coordinates;
    uint8_t nbCoordinates;



    //The initialisation flag : will be used to verify if the initialisation procedure was completed;
    bool initFlag;


};

//#include "MatrixDrone.cpp"

#endif //TRACER_MATRIXDRONE_H
