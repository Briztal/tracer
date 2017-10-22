/*
  Machine.cpp - Part of TRACER

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


#ifndef TRACER_COORDINATESINTERFACE_H
#define TRACER_COORDINATESINTERFACE_H


#include <cstdint>

class Machine {


    //-------------------------------------------------Power------------------------------------------------------------

public:

    static void disable_stepper_power();

    //-----------------------------------------------Movement-----------------------------------------------------------

public:

    static void set_current_mode(uint8_t mode);

    static void home_carriages(float speed);

    static void line_to(uint8_t carriage, float x, float y, float z, float speed);


private:

    static uint8_t mode;

    static float *const position;

    static void mode_0(uint8_t carriage, float x, float y, float z, float speed);

    static void sanity_check(float *hl_coords);
};


#endif //TRACER_COORDINATESINTERFACE_H
