/*
  Vector3D.cpp - Part of TRACER

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


#include "Vector3D.h"
#include <math.h>



/*
 * cloner : this function will clone the given vector;
 */

void Vector3D::clone(const Vector3D &src) {

    //Clone the data;
    Triplet::clone(src);

    //Clone the unitary flag;
    unitary = src.unitary;

}


//----------------------------------------------- Coordinate Operations ------------------------------------------------

/*
 * Setter : will check the index and set the value;
 */

void Vector3D::set(uint8_t index, float value) {

    //If the index is superior to 2 :
    if (index >= 3) {

        //Log :
        //std_out("Error in Triplet::set : invalid index");

        //Fail;
        return;

    }

    //Set the data;
    data[index] = value;

    //Reset the unitary flag;
    unitary = false;

}


/*
 * reset : resets the vector to (0,0,0);
 */

void Vector3D::reset() {

    //Reset the array;
    Triplet::reset();

    //Mark the vector as not unitary;
    unitary = false;

}


//----------------------------------------------- Internal Operations ------------------------------------------------

/*
 * nullVector : this function will return true only if all coordinates are null;
 */

bool Vector3D::nullVector() const {
    return (data[0] == 0) && (data[1] == 0) && (data[2] == 0);
}


/*
 * squareNorm : this function will return the square of the vector's norm;
 */

float Vector3D::squareNorm() const {

    if (unitary) return 1;

    //Compute the square of the norm and return it;
    float temp = data[0];
    float res = temp * temp;

    temp = data[1];
    res += temp * temp;

    temp = data[2];
    res += temp * temp;

    return res;

}


/*
 * normalise : this function will normalise the vector;
 */

bool Vector3D::normalise() {

    if (unitary)
        return true;

    if (nullVector())
        return false;

    //Determine the norm of the vector;
    float norm = sqrtf(squareNorm());

    //Normalise all coordinates;
    divide(norm);

    //Mark the vector as true;
    unitary = true;

    //Return true, as the vector has effectively been normalised;
    return true;

}


/*
 * add : adds the vector coordinate by coordinate;
 */

void Vector3D::add(Vector3D &v) {

    //Add all coordinates;
    for (uint8_t i = 3; i--;) {
        data[i] += v.data[i];
    }

    //Reset the unitary flag;
    unitary = false;

}


/*
 * subtract : subtracts the vector coordinate by coordinate;
 */

void Vector3D::subtract(Vector3D &v) {

    //subtract all coordinates;
    for (uint8_t i = 3; i--;) {
        data[i] -= v.data[i];
    }

    //Reset the unitary flag;
    unitary = false;

}


/*
 * multiply : multiply all coordinates by the given coefficient;
 */

void Vector3D::multiply(float coefficient) {

    //Multiply all coefficients;
    for (uint8_t i = 3; i--;) {
        data[i] *= coefficient;
    }

    //Reset the unitary flag;
    unitary = false;

}


/*
 * divide : divide all coordinates by the given coefficient;
 */

void Vector3D::divide(float coefficient) {

    //Multiply all coefficients;
    for (uint8_t i = 3; i--;) {
        data[i] /= coefficient;
    }

    //Reset the unitary flag;
    unitary = false;

}


/*
 * vectorProduct : this function will compute the vector product of the provided couple;
 */

void Vector3D::vectorProduct(Vector3D &v0, Vector3D &v1) {

    float x = v0.data[0], y = v0.data[1], z = v0.data[2];
    float vx = v1.data[0], vy = v1.data[1], vz = v1.data[2];

    //Compute all coordinates;
    data[0] = y * vz - z * vy;
    data[1] = z * vx - x * vz;
    data[2] = x * vy - y * vx;

}


/*
 * dotProduct : this function will compute the dot product of the provided couple;
 */

float Vector3D::dotProduct(Vector3D &v0) const {

    //Compute and return the dot product;
    return data[0] * v0.data[0] + data[1] * v0.data[1] + data[2] * v0.data[2];

}
