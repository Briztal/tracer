//
// Created by root on 2/28/18.
//

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
    static Vector3D * vectorProduct(Vector3D *v0, Vector3D *v1);

    //Compute the dot product of the provided couple;
    static float dotProduct(Vector3D *v0, Vector3D *v1);


public:

    //Vector coordinates, public;
    float x, y, z;

};


#endif //TRACER_VECTOR3D_H
