/*
  Vector3D.h - Part of TRACER

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

#ifndef TRACER_VECTOR3D_H
#define TRACER_VECTOR3D_H


#include "stdint.h"
#include "Triplet.h"

class Vector3D : public Triplet {

public:

    //Basic constructor;
    Vector3D() : Triplet(){}

    //Value constructor;
    Vector3D(float v0, float v1, float v2) : Triplet(v0, v1, v2) {};

    //Copy constructor;
    Vector3D(const Vector3D &src) : Triplet(src) {};

    //Destructor;
    ~Vector3D() override = default;

    //Cloner :
    void clone(const Vector3D &src);

    //---------------- Coordinates operations ----------------

public:

    //Modify a coordinate's value;
    void set(uint8_t index, float value) override;

    //Reset the vector to (0,0,0);
    void reset() override;


    //---------------- Internal operations ----------------

public:

    //Verify that the vector is null-vector;
    bool nullVector() const;

    //Determine the square of the vector's norm;
    float squareNorm()const;

    //Normalise the vector;
    bool normalise();

    //Add the given vector to our coordinates;
    void add(Vector3D &v);

    //Subtract the given vector to our coordinates;
    void subtract(Vector3D &v);

    //Multiply coordinates by a given coefficient;
    void multiply(float coefficient);

    //Divide coordinates by a given coefficient;
    void divide(float coefficient);


    //---------------- External operations ----------------

    //Compute the vector product of the provided couple;
    void vectorProduct(Vector3D &v0, Vector3D &v1);

    //Compute the dot product of the provided couple;
    float dotProduct(Vector3D &v) const;


    //---------------- Data ----------------


    //Normalisation bool
    bool unitary = false;


};


#endif //TRACER_VECTOR3D_H
