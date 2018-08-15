/*
  ControlLayer.cpp Part of TRACER

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

#include <string.h>

#include "ControlLayer.h"


//-------------------------------------- Initialisation --------------------------------------

/*
 * Constructor : initialises the child class;
 */

ControlLayer::ControlLayer(uint8_t size) :
        size(size), mode(0), child(nullptr), target(new float[size]), position(new float[size]) {}


/*
 * setChild : sets the child layer;
 */

void ControlLayer::setChild(ControlLayer *childLayer) {

    //Update the child layer;
    child = childLayer;

}

//-------------------------------------------- Control --------------------------------------------


/*
 * getPosition : copy the position in the position buffer;
 */

void ControlLayer::getPosition(float *position_buffer) {

    memcpy(position_buffer, position, size * sizeof(float));

}

