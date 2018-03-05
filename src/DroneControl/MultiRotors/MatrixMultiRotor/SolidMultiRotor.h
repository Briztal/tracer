/*
  SimpleMultiRotor.h - Part of TRACER

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


#ifndef TRACER_MULTIROTOR_H
#define TRACER_MULTIROTOR_H

/*
 * The SolidMultiRotor class.
 *
 *  This class regulates the power for all drones that conform to the following model :
 *
 *      - at least one single motor;
 *      - more than (>=) 4 motors;
 *      - all motors are fixed, and push vertically in the drone's referential
 */


#include <Math/Matrix.h>


#include <DroneControl/MatrixDrone.h>

#include "MultiRotorCoordinateSysem.h"

#include "MultiRotorMotorData.h"


class SolidMultiRotor : public MatrixDrone<MultiRotorMotorData, MultiRotorCoordinateSystem> {


    //------------------------------- Init -------------------------------

protected:

    //Constructor;
    SolidMultiRotor();

     //Destructor;
     virtual ~SolidMultiRotor();


    //------------------------------- Virtual methods -------------------------------

protected:

    //Set the coordinates that will figure in the coordinate system;
    virtual void setCoordinateSystem(MultiRotorCoordinateSystem *coordinate_system) = 0;

    //The method to create motors;
    virtual void createMotors() = 0;

    //The method to create relations;
    virtual void createRelations(LinearSystem *s) = 0;


    //------------------------------- Overridden methods -------------------------------

    //Get the enabled number of coordinates of the coordinate system;
    uint8_t getCoordinatesNumber(MultiRotorCoordinateSystem *coordinate_system);

    //Add all equations related to motors;
    void addMotorsEquations(MultiRotorCoordinateSystem *coordinates, LinearSystem *system);

    //Check that all enabled coordinates are controllable;
    bool checkControl(const Matrix *m, MultiRotorCoordinateSystem *coordinates);

};


#endif //TRACER_MULTIROTOR_H
