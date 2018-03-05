
/*
  Orientation2DToRotation.cpp - Part of TRACER

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

#include <Interaction/Interaction.h>

#include <Math/rotation_data.h>

#include "Orientation2DToRotation.h"

#define ANGULAR_THRESHOLD (float) 0.001

#define SQRT_2_D_2 0.7071

/*
 * Constructor : initialises the target vector;
 */
Orientation2DToRotation::Orientation2DToRotation() : targetVector() {}


/*
 * setTarget : this function will copy the target vector;
 */

void Orientation2DToRotation::setTaget(Vector3D &target) {

    //First, we must normalise the target;

    //If the target is null :
    if (!target.normalise()) {

        //Log;
        std_out("Error in Orientation2DToRotation::setTaget : null vector given;");

        //Fail;
        return;

    }

    //Copy coordinates;
    targetVector.clone(target);

}


/*
 * compute : this function will compute the rotation data, in order to reach the target vector;
 */

void Orientation2DToRotation::compute(Vector3D &currentVector, rotation_data_t &rotation_data) {

    //First, we must normalise the orientation;

    //If the orientation is null :
    if (!currentVector.normalise()) {

        //Log;
        std_out("Error in Orientation2DToRotation::setTaget : null vector given;");

        //Fail;
        return;

    }

    //If vectors are not collinear, we must determine the angle;

    //First, determine the cosine of the angle (dot product of vectors);
    float cos = currentVector.dotProduct(targetVector);

    std_out("cos : " + String(cos));

    //Then, save the angle and complete;
    float angle = acosf(cos);

    //First, cache the rotation vector, to avoid two pointer accesses;
    Vector3D rotation_vector(rotation_data.rotation_vector);

    std_out("angle : " + String(angle));


    //If the rotation angle is close enough to zero :
    if (angle < ANGULAR_THRESHOLD) {

        //Set the angle to zero;
        rotation_data.rotation_angle = 0;

        //Set the default rotation vector;
        rotation_vector.reset();
        rotation_vector.set(2, 1);

        //Complete;
        return;

    }


    //If the rotation angle is close enough to pi, vectors are collinear:
    if (((float) M_PI - angle) < ANGULAR_THRESHOLD) {

        //Update the angle to pi;
        angle = (float) M_PI;

        /*
         * We are in the case where current orientation and target are opposed. This case is particular,
         *  as there exist an infinity of correct rotation vectors. The following scope determines "arbitrarily"
         *  one of there;
         */

        /*
         * We must determine a rotation vector that is not collinear with the target;
         *  We will check x, and if it is not valid, we will select y;
         */

        //Let's set the current vector to x.
        rotation_vector.reset();
        rotation_vector.set(0, 1);

        float new_dot = currentVector.dotProduct(targetVector);

        //If x was too close to the target, y won't
        if (abs(new_dot) > SQRT_2_D_2) {

            std_out("switch");
            //Let's set the current vector to y;
            rotation_vector.reset();
            rotation_vector.set(1, 1);
        }

    }

    //Save the angle;
    rotation_data.rotation_angle = angle;

    //First, let's compute the vector product of the couple of vectors;
    rotation_vector.vectorProduct(currentVector, targetVector);

    //Then, normalise this vector;
    rotation_vector.normalise();


}
