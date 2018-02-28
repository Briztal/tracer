//
// Created by root on 2/27/18.
//

#include <Interaction/Interaction.h>
#include "RotationMatrix3D.h"


/*
 * Default constructor : initilises to the identity matrix;
 */

RotationMatrix3D::RotationMatrix3D() : Matrix(3, 3) {

    //Set the newly created matrix to the identity (no rotation);
    Matrix::setIdentityMatrix(this);

}


/*
 * Constructor from vectors : will compute the rotation corresponding to (u0, u1, u0 ^ u1) with :
 *  u0 and u1 the normalised vectors from v0 and v1;
 */

RotationMatrix3D::RotationMatrix3D(Vector3D *v0, Vector3D *v1) : Matrix(3, 3) {

    //If one of the vector is the null vector :
    if ((!v0->normalise()) || (!v1->normalise())) {

        //Log;
        std_out("Error in RotationMatrix3D::RotationMatrix3D : one of the vector is null vector; "
                        "Setting to identity matrix;");

        //Set to identity matrix;
        Matrix::setIdentityMatrix(this);

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
    Vector3D *v2 = Vector3D::vectorProduct(v0, v1);

    //Copy all data in the matrix;
    data_array[0] = v0->x;
    data_array[1] = v1->x;
    data_array[2] = v2->x;
    data_array[3] = v0->y;
    data_array[4] = v1->y;
    data_array[5] = v2->y;
    data_array[6] = v0->z;
    data_array[7] = v1->z;
    data_array[8] = v2->z;

}


/*
 * Duplicator : simply calls the Matrix class's duplicator;
 */
RotationMatrix3D::RotationMatrix3D(RotationMatrix3D *rotationMatrix3D) : Matrix(rotationMatrix3D) {}


RotationMatrix3D* RotationMatrix3D::compose(RotationMatrix3D *A, bool rightComposition) {

    //Create the new rotation matrix;
    RotationMatrix3D *new_rotation = new RotationMatrix3D();

    if (rightComposition) {

        //right multiplication : rot <- this * A;
        Matrix::multiply(this, A, new_rotation);

    } else {

        //left multiplication : rot <- A * this;
        Matrix::multiply(A, this, new_rotation);

    }

    //Return the rotation matrix;
    return new_rotation;

}

/*
 * transpose : transposes the matrix;
 */

void RotationMatrix3D::transpose() {
    Matrix::transpose();
}


/*
 * Compute the rotation angle;
 */

float RotationMatrix3D::getRotationAngle() {

    TODO

    return 0;

}


/*
 * getRotationAxis : this function will determine the rotation axis. It used the rotation angle to compute the
 *  norm threshold;
 */
Vector3D RotationMatrix3D::getRotationAxis(float rotation_angle) {

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


