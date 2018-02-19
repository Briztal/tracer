/*
  TriRover.h - Part of TRACER

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

#ifndef TRACER_TRIROVER_H
#define TRACER_TRIROVER_H


#include <DroneControl/Rover/MatrixRover/MatrixRover.h>

#include <DroneControl/MotorRegisterer.h>

#include <Math/LinearSystem.h>


class TriRover : public MatrixRover, private MotorRegisterer {

    //-------------------------------- Initialisation --------------------------------

public:

    //Constructor;
    TriRover();

    //Destructor;
    ~TriRover() override;


    //-------------------------------- Inherited methods from SolidMultiRotor --------------------------------

private:

    //Set the common (z, pitch, roll, yaw) coordinate system;
    void setCoordinateSystem(RoverCoordinateSystem *coordinate_system) override;

    //No relations to add;
    void createRelations(LinearSystem *system) override {}


    //-------------------------------- Functions delegated to the sub-class --------------------------------

protected:

    //The function to implement in the sub-class to register motors;
    void createMotors() override = 0;

    //The function to register a motor;
    void
    registerMotor(uint8_t motor_index, float x, float y, float phi, float theta, float radius, uint8_t servo_index);


};


#endif //TRACER_TRIROVER_H
