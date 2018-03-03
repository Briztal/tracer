//
// Created by root on 3/2/18.
//

#ifndef TRACER_VECTORTOANGLES_H
#define TRACER_VECTORTOANGLES_H


#include <Math/3D/Vector3D.h>
#include "stdint.h"

//TODO MAKE A FUCKING LIBRARY DUMBAS...

class AnglesToVector3D {

    /*
     * A relation data will be made of :
     *  - Epsilon = abs (angle mod[pi / 2] - pi / 4); the lower this parameter is, the closer to k * pi / 4
     *      the angle is ;
     *  - angle : the rotation angle;
     *  - in, out: indices of (resp) input and output coordinates in the relation :
     *      output = arctan(angle) * input;
     *
     *      Reminder :
     *          - x = arctan(ay) * z <=> _0 = arctan(ay) * _2
     *          - y = arctan(az) * x <=> _1 = arctan(az) * _0
     *          - z = arctan(ax) * y <=> _2 = arctan(ax) * _1
     */

    typedef struct {

        float epsilon;

        float angle;

        uint8_t in;

        uint8_t out;


    } relation_data;


public:

    static void convert(float ax, float ay, float az, Vector3D *vector);


private:

    static void build_relation_data(relation_data *data, float epsilon, float angle, uint8_t in, uint8_t out);

    static void sortRelations(relation_data **data_array);

    static void switchRelations(relation_data **data_array, uint8_t index_0, uint8_t index_1);

    uint8_t static getEpsilons(relation_data **data, bool *equation);

    static uint8_t getRelationIndexLevel2(bool *equation, relation_data **data_array);

    static float formatAngle(float angle);
};


#endif //TRACER_VECTORTOANGLES_H
