//
// Created by root on 3/3/18.
//

#ifndef TRACER_VECTOR3DTOANGLES_H
#define TRACER_VECTOR3DTOANGLES_H


//TODO MAKE A FUCKING LIBRARY DUMBAS...


#include <DataStructures/CoordinateSystems/Vector3D.h>
#include <DataStructures/CoordinateSystems/Angles3D.h>

class Vector3DToAngles {

public:

    static void convert(Vector3D &vector, Angles3D &angles);

};


#endif //TRACER_VECTOR3DTOANGLES_H
