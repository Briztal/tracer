//
// Created by root on 2/13/18.
//

#ifndef TRACER_CONTROLSTAGE_H
#define TRACER_CONTROLSTAGE_H

#include "stdint.h"
#include "ControlLayer.h"


class ControlStage {

    //------------------------------------------ Initialisation ------------------------------------------

public:

    //Constructor, initialises the two fields;
    ControlStage(uint8_t chainedMode, ControlLayer *controlLayer);

    //Destructor : deletes the controlLayer;
    ~ControlStage();


    //------------------------------------------ Fields ------------------------------------------

    //The mode in which the layer must be configured when layers work in chain;
    const uint8_t chainedMode;

    //The actual control layer;
    ControlLayer *const controlLayer;

};


#endif //TRACER_CONTROLSTAGE_H
