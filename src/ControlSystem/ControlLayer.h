/*
  ControlLayer.h Part of TRACER

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

#ifndef TRACER_CONTROLLAYER_H
#define TRACER_CONTROLLAYER_H

/*
 * This class is the virtual base for a control layer;
 *
 * A control layer represents a layer of control in your control scheme, and is defined by the following data :
 *
 *  - a child control layer, can be null;
 *  - a regulation action : given a target (float *), and a current position, it calls its child with computed data, or
 *      makes an action (sends a command to an actuator, sets data in RAM, etc...)
 *  - a set of modes. Target can be expressed in a system that is different of the one used to determine the action;
 *      The mode determines how target will be translated;
 *
 *
 *  As a programmer, your job is to implement the conversion translation function, the mode setting function, and
 *      the action function.
 *
 */

#include <stdint.h>

class ControlLayer {


    //-------------------------------- Initialisation --------------------------------

//Only accessible for sub classes;
protected:

    //Constructor : initialises the child class;
    explicit ControlLayer(uint8_t size);


public:

    //Destructor : nothing to do;
    virtual ~ControlLayer() = default;


public:

    //Set the child layer;
    void setChild(ControlLayer *childLayer);


    //-------------------------------- Control --------------------------------

public:

    //getPosition : saves the current position of the control layer in the given array;
    void getPosition(float *position_buffer);

    //The only pure virtual function of the class (defines the behaviour of the control layer);
    virtual void action() = 0;

    //The virtual function to set the mode;
    virtual bool setMode(uint8_t new_mode) = 0;

    //The virtual function to translate the given coordinates into the standard coordinates
    virtual void setTarget(float *target) = 0;


    //-------------------------------- Fields --------------------------------

protected:

    //The size of the layer (number of input_output variables)
    const uint8_t size;

    //The current mode;
    const uint8_t mode;

    //The sub-class, that is controlled by us.
    ControlLayer *child;

    //The current target of the layer;
    float *target;

    //The current position of the layer;
    float *position;

};


#endif //TRACER_CONTROLLAYER_H
