/*
  SteppersData.h - Part of TRACER

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


#ifndef TRACER_STEPPERSDATA_H
#define TRACER_STEPPERSDATA_H

#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

//The stepper data structure : contains all data related to a stepper motor
struct stepper_data_t {
    float steps_per_unit;
    float maximum_speed;
    float acceleration;
    float jerk;
};


class SteppersData {

public:

    //Initialise all data and register variables in EEPROM;
    static void initialise_data();

    //Reset data to its default value;
    static void reset_data();

    //The stepper data array;
    static stepper_data_t *const steppers_data;

    static void EEPROM_registration();
};

#endif

#endif //TRACER_STEPPERSDATA_H

