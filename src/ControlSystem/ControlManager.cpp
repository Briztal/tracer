/*
  LayerManager.cpp Part of TRACER

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

#include "ControlManager.h"


/*
 * Constructor : calls the parent constructor;
 */

ControlManager::ControlManager(uint8_t size) : DynamicPointerBuffer(size), locked(false) {

}


/*
 * Destructor : does nothing;
 */

ControlManager::~ControlManager() = default;


/*
 * addLayer : creates a new stage and adds it to the vector;
 */

void ControlManager::addLayer(ControlLayer *newLayer, uint8_t chainedMode) {

    //If the system is locked, fail;
    if (locked)
        return;

    //Create and save the new stage;
    add(new ControlStage(chainedMode, newLayer));

}


/*
 * validate : sets up every layer, and returns true if all happened correctly;
 */

bool ControlManager::validate() {

    //Cache the size;
    const uint8_t size = getSize();


    /*
     * First, we must check that all stages can be set in the chained mode (no invalid mode);
     */

    //For every stage :
    for (uint8_t stage_index = 0; stage_index < size; stage_index++) {

        //Cache the current control stage;
        ControlStage *stage = getElement(stage_index);

        //If the chained mode can't be set (invalid mode) :
        if (!stage->controlLayer->setMode(stage->chainedMode)) {

            //Fail;
            return false;

        }

    }


    /*
     * Then, we must set the child of every layer;
     */

    //If there is only one layer, no setting required;
    if (size <= 1)
        return true;

    //Initialise the child layer;
    ControlLayer *child_layer = getElement(0)->controlLayer;

    //For all layers, except the first;
    for (uint8_t stage_index = 1; stage_index < size; stage_index++) {

        //Cache the current layer;
        ControlLayer *current_layer = getElement(0)->controlLayer;

        //Set the current layer;
        current_layer->setChild(child_layer);

        //Update the child layer;
        child_layer = current_layer;

    }

    //Lock the system;
    locked = true;

    //Complete;
    return true;

}


/*
 * initialise : this function initialises each stage at its given mode;
 */

void ControlManager::initialise() {

    //For every stage of the scheme :
    for (uint8_t stage_index = 0; stage_index < getSize(); stage_index++) {

        //Cache the stage;
        ControlStage *stage = getElement(stage_index);

        //Set the appropriate mode;
        stage->controlLayer->setMode(stage->chainedMode);

    }

}


/*
 * setMode : sets the mode for the given control layer;
 */

bool ControlManager::setMode(uint8_t stageIndex, uint8_t newMode) {

    //Verify that the required stage exists;
    if (stageIndex < getSize()) {

        //Update the targetVector for the required stage;
        return getElement(stageIndex)->controlLayer->setMode(newMode);

    }

    return false;

}


/*
 * setTarget : sets the targetVector for the given control layer;
 */

void ControlManager::setTarget(uint8_t stageIndex, float *target) {

    //Verify that the required stage exists;
    if (stageIndex < getSize()) {

        //Update the targetVector for the required stage;
        getElement(stageIndex)->controlLayer->setTarget(target);

    }

}
