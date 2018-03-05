/*
  RotationMatrix.h - Part of TRACER

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


#ifndef TRACER_MATRIX3D_H
#define TRACER_MATRIX3D_H

#include "Math/Matrix.h"

#include "DataStructures/CoordinateSystems/Vector3D.h"


class RotationMatrix3D : private Matrix {


    //------------------- Initialisation -------------------

public:

    //Default constructor (returns the identity matrix)
    RotationMatrix3D();

    //Constructor;
    RotationMatrix3D(Vector3D &v0, Vector3D &v1);

    //Copy constructor;
    RotationMatrix3D(RotationMatrix3D &rotationMatrix3D);

    //Destructor;
    ~RotationMatrix3D() override = default;


    //------------------- Build -------------------

    //Resets the matrix to identity;
    void setToIdentity() override {Matrix::setToIdentity();};

    //Build the rotation matrix from vectors;
    void buildFromVectors(Vector3D &v0, Vector3D &v1);


    //------------------- Matrix operations -------------------

public:

    //Transpose / invert the matrix;
    void transpose() override {Matrix::transpose();};

    //Compose given rotation in this;
    void compose(const RotationMatrix3D &A, const RotationMatrix3D &B);


    //------------------- Parameters extraction -------------------

public:

    //Compute the rotation axis;
    float getRotationData(Vector3D &vector);


    //------------------- Standard functions -------------------

public:

    //String conversion;
    String toString() const override {return Matrix::toString();};

};


#endif //TRACER_MATRIX3D_H
