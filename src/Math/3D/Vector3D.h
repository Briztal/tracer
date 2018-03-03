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


class Vector3D {

public:

    //Constructor;
    Vector3D(float x, float y, float z);

    //Duplicator;
    Vector3D(Vector3D *);

    //Destructor;
    ~Vector3D() = default;


    //---------------- Internal operations ----------------

public:

    //Verify that the vector is null-vector;
    bool nullVector();

    //Determine the square of the vector's norm;
    float squareNorm();

    //Normalise the vector;
    bool normalise();

    //Add the given vector to our coordinates;
    void add(Vector3D *v);

    //Subtract the given vector to our coordinates;
    void subtract(Vector3D *v);

    //Multiply coordinates by a given coefficient;
    void multiply(float coefficient);

    //Divide coordinates by a given coefficient;
    void divide(float coefficient);


    //---------------- External operations ----------------

    //Compute the vector product of the provided couple;
    void vectorProduct(Vector3D *v0, Vector3D *v1);

    //Compute the dot product of the provided couple;
    static float dotProduct(Vector3D *v0, Vector3D *v1);


    //----------------

public:

    //Vector coordinates, public;
    float x, y, z;

};


#endif //TRACER_VECTOR3D_H
