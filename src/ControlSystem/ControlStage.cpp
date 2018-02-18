//
// Created by root on 2/13/18.
//

#include "ControlStage.h"


/*
 * Constructor :
 */

ControlStage::ControlStage(uint8_t chainedMode, ControlLayer *controlLayer)
        : chainedMode(chainedMode), controlLayer(controlLayer) {}


/*
 * Destructor : deletes the only object pointer we own.
 */

ControlStage::~ControlStage() {

    //Delete the control layer object.
    delete controlLayer;

}
