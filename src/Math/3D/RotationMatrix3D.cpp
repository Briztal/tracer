/*
  RotationMatrix3D.cpp - Part of TRACER

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
#include "RotationMatrix3D.h"

//The angular limit : below, the rotation will be considered as null;
#define ANGULAR_THRESHOLD 0.001


/*
 * Default constructor : initialises to the identity matrix;
 */

RotationMatrix3D::RotationMatrix3D() : Matrix(3, 3) {

    //Set the newly created matrix to the identity (no rotation);
    setToIdentity();

}


/*
 * Constructor from vectors : will compute the rotation corresponding to (u0, u1, u0 ^ u1) with :
 *  u0 and u1 the normalised vectors from v0 and v1;
 */

RotationMatrix3D::RotationMatrix3D(Vector3D *v0, Vector3D *v1) : Matrix(3, 3) {

    buildFromVectors(v0, v1);

}


/*
 * Duplicator : simply calls the Matrix class's duplicator;
 */

RotationMatrix3D::RotationMatrix3D(RotationMatrix3D *rotationMatrix3D) : Matrix(rotationMatrix3D) {}


/*
 * buildFromVectors : this function will build the rotation matrix from the given couple of vectors;
 */

void RotationMatrix3D::buildFromVectors(Vector3D *v0, Vector3D *v1) {

    //If one of the vector is the null vector :
    if ((!v0->normalise()) || (!v1->normalise())) {

        //Log;
        std_out("Error in RotationMatrix3D::RotationMatrix3D : one of the vector is null vector; "
                        "Setting to identity matrix;");

        //Set to identity matrix;
        setToIdentity();

    }

    //First, we will modify v1 so that it is orthogonal to v0;
    float dot = Vector3D::dotProduct(v0, v1);

    //Determine the difference to apply;
    Vector3D diff = Vector3D(v0);
    diff.multiply(dot);

    //Modify v1 to be orthogonal to v0;
    v1->subtract(&diff);

    //Re-normalise v1;
    v1->normalise();

    //Determine the last vector of the base;
    Vector3D v2(0, 0, 0);
    v2.vectorProduct(v0, v1);

    //Copy all data in the matrix;
    data_array[0] = v0->x;
    data_array[1] = v1->x;
    data_array[2] = v2.x;
    data_array[3] = v0->y;
    data_array[4] = v1->y;
    data_array[5] = v2.y;
    data_array[6] = v0->z;
    data_array[7] = v1->z;
    data_array[8] = v2.z;

}


/*
 * compose : this function will multiply (on the left or on the right) the given matrix;
 */

void RotationMatrix3D::compose(const RotationMatrix3D *const A, const RotationMatrix3D *const B) {

    //right multiplication : rot <- this * A;
    multiply(A, B);

}


/*
 * getRotationAxis : this function will determine the rotation axis. It used the rotation angle to compute the
 *  norm threshold;
 */

/*
Vector3D RotationMatrix3D::getRotationAxis2(float rotation_angle) {

    //First, we will determine the norm threshold;
    float threshold;

    //Get the absolute value of the rotation angle;
    if (rotation_angle < 0) rotation_angle = -rotation_angle;

    if (abs(rotation_angle) < 1) {

        //If the rotation angle is inferior to 1, the threshold will be the fourth of the rotation angle;
        threshold = rotation_angle / (float)4;

    } else {

        //If the rotation angle is superior to 1, the threshold will be one fourth;
        threshold = (float)1 / (float)4;

    }

    //The matrix that will contain Id - this;
    Matrix *diff = new Matrix(3, 3);

    //Set diff to identity;
    Matrix::setIdentityMatrix(diff);

    //diff = Id - this;
    diff->subtract(this);

    //Now, we can compute the direction vector, by finding two non-colinear vectors of diff's columns;

    //Cache diffs data array;
    float *array = diff->getDataArray();

    //Determine the two first column vectors;
    Vector3D *v0 = new Vector3D(array[0], array[3], array[6]);
    Vector3D *v1 = new Vector3D(array[1], array[4], array[7]);
    Vector3D *v2 = new Vector3D(array[2], array[5], array[8]);

    //Get all validation bools;
    float b0 = v0->squareNorm() > threshold, b1 = v1->squareNorm() > threshold, b2 = v2->squareNorm() > threshold;

    //Cache the rotation axis;
    Vector3D *rotation_axis;

    if (b0 && b1) {

        //If b0 and b1 can be used;
         rotation_axis = Vector3D::vectorProduct(v0, v1);

    } else if (b1 && b2) {

        //If b1 and b2 can be used :
        rotation_axis = Vector3D::vectorProduct(v1, v2);

    } else if (b0 && b2) {

        //If b0 and b2 can be used :
        rotation_axis = Vector3D::vectorProduct(v0, v2);

    } else {
        //If no vector couple can be used;

        //Log;
        std_out("Error in RotationMatrix3D::getRotationAxis, the threshold is too high;");

        //Return a default vector;
        return new Vector3D(0, 0, 0);

    }

    //Normalise the rotation vector;
    if (!rotation_axis->normalise()) {

        //Log;
        std_out("Error in RotationMatrix3D::getRotationAxis, the rotation vector is null;");

    }

    //Return the normalised (or not) rotation axis;
    return rotation_axis;

}
*/


/*
 * getRotationData : this function will determine the rotation axis and angle;
 */

float RotationMatrix3D::getRotationData(Vector3D *vector) {


    /*
     * In this function, we will use the rotation matrix decomposition :
     *          1 	0 	0           x*x 	x*y 	x*z             0   -z   y
     * = c *    0 	1 	0   + t *   x*y 	y*y 	y*z     + s *   z 	 0 	-x
     *          0 	0 	1           x*z 	y*z 	z*z            -y 	 x 	 0
     */

    //Main diagonal coefficients are gonna be used all across the function :
    float m00 = data_array[0], m11 = data_array[4], m22 = data_array[8];

    /*
     * Angle computation :
     *
     * We will use the following formula : 1 + 2 * cos(angle) = tr(M) <=> angle = acos((tr(M) - 1) / 2);
     *
     */


    //Compute the arcccos argument;
    float angle_cos = (m00 + m11 + m22 - 1) / 2;

    //Compute and return the angle;
    float angle = acosf(angle_cos);


    /*
     * Now that we have the angle, there are two limit cases that we must handle. The first is the null rotation,
     *  with an angle close to zero;
     */

    //If the absolute angle is smaller than a given threshold (angular limit) :
    if (angle < ANGULAR_THRESHOLD) {

        //Set vertical vector;
        vector->z = 1, vector->x = vector->y = 0;

        //Return null rotation;
        return 0;

    }


    /*
     * If the angle is acceptable, we can use two methods :
     *  - Use formulas based on sin. The method can be used on ]0, 3 pi / 4]. After, the formula degenerates
     *      because of the sin;
     *  - Instead, determine vector signs, and use m00, m11 and m22 to get their values;
     */

    //If the angle is in ]0; 3 pi / 4 ] :
    if (angle <= (float) M_PI * (float) 3 / float(4)) {

        /*
         *  We will use following formulas;
         *
         *  x = (m21 - m12)/(2 * angle_sin)
         *  y = (m02 - m20)/(2 * angle_sin)
         *  z = (m10 - m01)/(2 * angle_sin)
         *
         */

        //Determine the absolute sine of the angle (|sin| = sqrt(1 - cos ^ 2));
        float abs_sin = sqrtf(1 - angle_cos * angle_cos);

        //Compute the denominator;
        float denominator = 1 / ((float) 2 * abs_sin);

        //Update all coordinates :

        //x = (m21 - m12) * denominator;
        vector->x = (data_array[7] - data_array[5]) * denominator;

        //y = (m02 - m20) * denominator;
        vector->y = (data_array[2] - data_array[6]) * denominator;

        //z = (m10 - m01) * denominator;
        vector->z = (data_array[3] - data_array[1]) * denominator;


    } else {

        /*
         * One thing we can remark, is that coefficient from the main diagonal contain the square of x, y and z,
         *  plus a constant.
         *
         * To gain in precision, in the case of an angle superior to 3 * PI / 4, we will compute these square values,
         * by subtracting the constant ( = angle_cos)
         */

        float den = (float) 1 / ((float) 1 - angle_cos);

        m00 = (m00 - angle_cos) * den;
        m11 = (m11 - angle_cos) * den;
        m22 = (m22 - angle_cos) * den;


        /*
         * The second limit case is the rotation of an angle of pi (turnaround);
         */

        if ((float) M_PI - angle < (float) ANGULAR_THRESHOLD) {

            //Set the angle to pi;
            angle = (float) M_PI;

            //Take the square root of all coordinates, as turnaround in both directions are equivalent;
            vector->x = sqrtf(m00), vector->y = sqrtf(m11), vector->z = sqrtf(m22);

            return angle;

        }


        /*
         * If the angle is in ] 3 pi / 4; pi [
         *
         *  We will use following formulas;
         *
         *  x = sign_of(m21 - m12) * sqrt(m00 - cos_angle)
         *  y = sign_of(m02 - m20) * sqrt(m11 - cos_angle)
         *  z = sign_of(m10 - m01) * sqrt(m22 - cos_angle)
         *
         */

        float temp;

        //Update x :
        temp = sqrtf(m00);
        vector->x = (data_array[7] > data_array[5]) ? temp : -temp;

        //Update y :
        temp = sqrtf(m11);
        vector->y = (data_array[2] - data_array[6]) ? temp : -temp;

        //Update z :
        temp = sqrtf(m22);
        vector->z = (data_array[3] - data_array[1]) ? temp : -temp;

    }

    //Finally, return the angle;
    return angle;


}

