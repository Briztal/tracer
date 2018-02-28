//
// Created by root on 2/28/18.
//

#include "Vector3D.h"
#include <math.h>


/*
 * Constructor;
 */

Vector3D::Vector3D(float x, float y, float z) : x(x), y(y), z(z) {};


/*
 * Duplicator;
 */

Vector3D::Vector3D(Vector3D *v) : x(v->x), y(v->y), z(v->z) {}


/*
 * nullVector : this function will return true only if all coordinates are null;
 */

bool Vector3D::nullVector() {
    return (x == 0) && (y == 0) && (z == 0);
}


/*
 * squareNorm : this function will return the square of the vector's norm;
 */

float Vector3D::squareNorm() {

    //Compute the square of the norm and return it;
    return x * x + y * y + z * z;

}

/*
 * normalise : this function will normalise the vector;
 */

bool Vector3D::normalise() {

    if (nullVector())
        return false;

    //Determine the norm of the vector;
    float norm = sqrtf(squareNorm());

    //Normalise all coordinates;
    divide(norm);

    //Return true, as the vector has effectively been normalised;
    return true;

}


/*
 * add : adds the vector coordinate by coordinate;
 */

void Vector3D::add(Vector3D *v) {

    //Add all coordinates;
    x += v->x;
    y += v->y;
    z += v->z;
}


/*
 * subtract : subtracts the vector coordinate by coordinate;
 */

void Vector3D::subtract(Vector3D *v) {

    //subtract all coordinates;
    x -= v->x;
    y -= v->y;
    z -= v->z;

}


/*
 * multiply : multiply all coordinates by the given coefficient;
 */

void Vector3D::multiply(float coefficient) {

    //Multiply all coefficients;
    x *= coefficient;
    y *= coefficient;
    z *= coefficient;

}


/*
 * divide : divide all coordinates by the given coefficient;
 */

void Vector3D::divide(float coefficient) {

    //Multiply all coefficients;
    x /= coefficient;
    y /= coefficient;
    z /= coefficient;

}

/*
 * vectorProduct : this function will compute the vector product of the provided couple;
 */

Vector3D *Vector3D::vectorProduct(Vector3D *v0, Vector3D *v1) {

    //Compute all coordinates;
    float x = v0->y * v1->z - v0->z * v1->y;
    float y = v0->z * v1->x - v0->x * v1->z;
    float z = v0->x * v1->y - v0->y * v1->x;

    //Store coordinates in a new vector and return its pointer;
    return new Vector3D(0, 0, 0);

}


/*
 * dotProduct : this function will compute the dot product of the provided couple;
 */

float Vector3D::dotProduct(Vector3D *v0, Vector3D *v1) {

    //Compute and return the dot product;
    return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;

}
