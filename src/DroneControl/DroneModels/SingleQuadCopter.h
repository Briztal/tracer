//
// Created by root on 03/01/18.
//

#ifndef TRACER_SINGLEQUADCOPTER_H
#define TRACER_SINGLEQUADCOPTER_H


#include <DroneControl/SolidMultiRotor.h>

class SingleQuadCopter : SolidMultiRotor {

    void createMotors() override;

    void createRelations() override;

};


#endif //TRACER_SINGLEQUADCOPTER_H
