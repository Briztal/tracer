/*
  Sensors.h -  Part of TRACER

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

#ifndef TRACER_ACTIONS_H
#define TRACER_ACTIONS_H

#include "BinaryActions.h"
#include "ContinuousActions.h"

/*
 * This class is a simple formalisation. I use it to centralise hardware and
 *  data initialisation for all Action classes
 */

class Actions {

public:

    //Initialise all sensors's hardware;
    static inline void initialise_hardware() {

        //Initialise Binary actions;
        //TODO BinaryActions::initialise_hardware();

        //Initialise Binary actions;
        ContinuousActions::initialise_hardware();

        //Initialise Binary actions;
        //TODO ServoActions::initialise_hardware();

    }

    //Initialise all sensors's data;
    static inline void initialise_data() {

        //Initialise Binary actions;
        //TODO BinaryActions::initialise_data();

        //Initialise Binary actions;
        ContinuousActions::initialise_data();

        //Initialise Binary actions;
        //TODO ServoActions::initialise_data();

    }
};


#endif //TRACER_SENSORS_H
