/*
  Orientation2DToRotation.h - Part of TRACER

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


#ifndef TRACER_ORIENTATION2DTOROTATION_H
#define TRACER_ORIENTATION2DTOROTATION_H

#include <Math/3D/Vector3D.h>

#include <Math/rotation_data.h>

#include <Math/3D/RotationMatrix3D.h>


class Orientation2DToRotation {


    //----------------------------------- Init ---------------------------------------

public:

    //Constructor;
    Orientation2DToRotation();

    //Destructor;
    ~Orientation2DToRotation();


    //----------------------------------- Computation ---------------------------------------

public:

    //Set the target vector;
    void setTaget(Vector3D *target);

    //Compute the rotation data for the given position;
    void compute(Vector3D *currentVector, rotation_data_t *rotation_data);


    //----------------------------------- Fields ---------------------------------------

private:

    //The targetVector orientation
    Vector3D *targetVector;

};


#endif //TRACER_ORIENTATION2DTOROTATION_H
