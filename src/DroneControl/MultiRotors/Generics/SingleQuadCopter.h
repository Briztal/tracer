/*
  SingleQuadCopter.h - Part of TRACER

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


#ifndef TRACER_SINGLEQUADCOPTER_H
#define TRACER_SINGLEQUADCOPTER_H


#include <DroneControl/MultiRotors/MatrixMultiRotor/SolidMultiRotor.h>

#include <DroneControl/MotorRegisterer.h>

#include <Math/LinearSystem.h>


class SingleQuadCopter : public SolidMultiRotor, private MotorRegisterer {


    //-------------------------------- Initialisation --------------------------------

public:

    //Constructor;
    SingleQuadCopter();

    //Destructor;
    ~SingleQuadCopter() override;


    //-------------------------------- Inherited methods from SolidMultiRotor --------------------------------

private:

    //Set the common (z, pitch, roll, yaw) coordinate system;
    void setCoordinateSystem(MultiRotorCoordinateSystem *coordinate_system) override;

    //No relations to add;
    void createRelations(LinearSystem *system) override {}


    //-------------------------------- Functions delegated to the sub-class --------------------------------

protected:

    //The function to implement in the sub-class to register motors;
    void createMotors() override = 0;

    //The function to register a motor;
    void registerMotor(uint8_t motor_index, float x, float y, float z, bool direction, float traction_coeff, float torque_coeff,
                       float kV, float voltage, float max_signal, uint8_t servo_index);


};


#endif //TRACER_SINGLEQUADCOPTER_H
