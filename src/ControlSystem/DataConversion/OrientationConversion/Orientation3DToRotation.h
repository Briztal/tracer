/*
  Orientation3DToRotation.h - Part of TRACER

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


#ifndef TRACER_ORIENTATION3DTOROTATION_H
#define TRACER_ORIENTATION3DTOROTATION_H


#include "../../PID.h"
#include "../../../Math/Matrix.h"

#include "Math/3D/Vector3D.h"
#include "Math/3D/RotationMatrix3D.h"
#include "../../../Math/rotation_data.h"


class Orientation3DToRotation {


    //----------------------------------- Init ---------------------------------------

public:

    //Constructor;
    Orientation3DToRotation();

    //Destructor;
    ~Orientation3DToRotation();

    //Reset all data;
    void reset();


    //----------------------------------- Computation ---------------------------------------

public:

    //Set the target orientation;
    void setTaget(Vector3D *v0, Vector3D *v1);

    //Compute the rotation data for the given orientation;
    void compute(Vector3D *current_v0, Vector3D *current_v1, rotation_data_t *rotation_data);


    //----------------------------------- Fields ---------------------------------------

private:

    //The targetVector orientation
    RotationMatrix3D *targetOrientation;

    //Temp matrices;
    RotationMatrix3D *currentOrientation;
    RotationMatrix3D *finalTransformation;

};


#endif //TRACER_ORIENTATION3DTOROTATION_H
