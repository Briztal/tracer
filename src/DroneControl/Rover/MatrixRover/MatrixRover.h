/*
  Rover.cpp - Part of TRACER

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


#ifndef TRACER_ROVER_H
#define TRACER_ROVER_H


#include "RoverMotorData.h"

#include "RoverCoordinateSystem.h"

#include <LinearSolver/LinearSystem.h>

#include <DroneControl/MatrixDrone.h>


class MatrixRover : public MatrixDrone<RoverMotorData, RoverCoordinateSystem> {

    //------------------------------- Init -------------------------------

protected:

    //Constructor;
    MatrixRover();

    //Destructor;
    virtual ~MatrixRover();

    //------------------------------- Virtual methods -------------------------------


protected:

    //Set the coordinates that will figure in the coordinate system;
    virtual void setCoordinateSystem(RoverCoordinateSystem *coordinate_system) = 0;

    //The method to create motors;
    virtual void createMotors() = 0;

    //The method to create relations;
    virtual void createRelations(LinearSystem *s) = 0;


    //------------------------------- Overridden methods -------------------------------

    //Count the number of enabled coordinates in the coordinate system;
    uint8_t getCoordinatesNumber(RoverCoordinateSystem *coordinate_system);

    //Add all motors equations;
    void addMotorsEquations(RoverCoordinateSystem *, LinearSystem*);


    //------------------------------- Theta format -------------------------------

    //Format theta in [-Pi/2, Pi/2[
    float thetaFormat(float theta);
    
    //Check that all enabled coordinates are controllable;
    bool checkControl(const Matrix *m, RoverCoordinateSystem *coordinates);

};


#endif //TRACER_ROVER_H
