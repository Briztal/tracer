/*
  Orientation3DToRotation.cpp - Part of TRACER

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



#include "Orientation3DToRotation.h"


/*
 * Constructor : initialises all matrices;
 */

Orientation3DToRotation::Orientation3DToRotation()
        : targetOrientation(), currentOrientation(), finalTransformation() {}


/*
 * reset : resets the target matrix to identity;
 */

void Orientation3DToRotation::reset() {

    //Reset the matrix to identity;
    targetOrientation.setToIdentity();

}


void Orientation3DToRotation::setTaget(Vector3D &v0, Vector3D &v1) {

    //Rebuild the orientation matrix;
    targetOrientation.buildFromVectors(v0, v1);

}


/*
 * compute : this function will determine the rotation that is required to go from the current orientation to
 *  the target orientation;
 */

void Orientation3DToRotation::compute(Vector3D &current_v0, Vector3D &current_v1, rotation_data_t *rotation_data) {

    //Build the current orientation matrix;
    currentOrientation.buildFromVectors(current_v0, current_v1);

    //Transpose the current orientation (as currentOrientation is symmetric, its inverse is its transposed);
    currentOrientation.transpose();

    //Now, we will determine the product (current ^ -1) * target;
    finalTransformation.compose(currentOrientation, targetOrientation);

    //Finally, we can now extract rotation data from the rotation product;
    rotation_data->rotation_angle = finalTransformation.getRotationData(rotation_data->rotation_vector);

}
