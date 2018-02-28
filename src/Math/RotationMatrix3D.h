//
// Created by root on 2/27/18.
//

#ifndef TRACER_MATRIX3D_H
#define TRACER_MATRIX3D_H


#include "Vector3D.h"

#include "Matrix.h"

class RotationMatrix3D : private Matrix {


    //------------------- Initialisation -------------------

public:

    //Default constructor (returns the identity matrix)
    RotationMatrix3D();

    //Constructor;
    RotationMatrix3D(Vector3D *v0, Vector3D *v1);

    //Copier;
    explicit RotationMatrix3D(RotationMatrix3D *rotationMatrix3D);

    //Destructor;
    ~RotationMatrix3D() = default;


    //------------------- Matrix operations -------------------

public:

    //Compose the rotation, on the right if rightMultiplication is set, and on the left if not;
    RotationMatrix3D * compose(RotationMatrix3D *A, bool rightComposition);

    //Transpose / invert the matrix;
    void transpose();


    //------------------- Parameters extraction -------------------

public:

    //Compute the rotation angle;
    float getRotationAngle();

    //Compute the rotation axis;
    Vector3D getRotationAxis(float rotation_angle);

};


#endif //TRACER_MATRIX3D_H
