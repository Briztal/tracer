/*
  LayerManager.h Part of TRACER

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


#ifndef TRACER_LAYERMANAGER_H
#define TRACER_LAYERMANAGER_H

#include "ControlStage.h"

#include "stdint.h"

#include "DataStructures/Containers/PointerContainer.h"
#include "ControlStage.h"


/*
 * The ControlManager Class. This class is in charge of storing al layers of your control model.
 *
 *  When you want to control your model, by targets or by trajectory, you will control a certain layer of the model.
 */

class ControlManager : private PointerVector<ControlStage> {


    //------------------------------------- Initialisation -------------------------------------

public:

    //Constructor : calls vector's parent constructor;
    explicit ControlManager(uint8_t size);

    //Destructor : calls Vector's destructor;
    ~ControlManager() override;


    //------------------------------------- Data insertion -------------------------------------

public:

    //Append a layer in the ascending order (the first one will focus on motors);
    void addLayer(ControlLayer *new_layer, uint8_t chainedMode);


    //------------------------------------- Data validation -------------------------------------

public:

    //Check and assign the layer scheme. If the validation completes, locks the scheme;
    bool validate();


    //------------------------------------- Status -------------------------------------

private:

    //The status flag. Set if the layer scheme is locked;
    bool locked;


    //------------------------------------- Init -------------------------------------

public:

    //Update the targetVector of a given stage;
    void initialise();


    //------------------------------------- Control -------------------------------------

public:

    //Update the mode of the given stage
    bool setMode(uint8_t stageIndex, uint8_t newMode);

    //Update the targetVector of the given stage;
    void setTarget(uint8_t stageIndex, float *target);

};


#endif //TRACER_LAYERMANAGER_H
