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


#include <DroneControl/SolidMultiRotor.h>
#include <LinearSolver/LinearSystem.h>

class SingleQuadCopter : SolidMultiRotor {

public:

    SingleQuadCopter();


private:
    void setCoordinateSystem(coordinate_system_t *coordinate_system) override;

    void createMotors() override;

    void createRelations(LinearSystem *system) override;

};


#endif //TRACER_SINGLEQUADCOPTER_H
