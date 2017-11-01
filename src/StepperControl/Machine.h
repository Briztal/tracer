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
#include <TaskScheduler/task_state_t.h>

class Machine {


    //-------------------------------------------------Power------------------------------------------------------------

public:

    static void disable_stepper_power();


    //-----------------------------------------------Movement-----------------------------------------------------------

public:

    static void set_current_mode(uint8_t mode);

    static task_state_t zero_carriages();

    static task_state_t line_to(float x, float y, float z);

    static task_state_t line_of(float x, float y, float z);

    static task_state_t set_carriage(uint8_t carriage_id);

    static task_state_t set_speed_for_carriage(uint8_t carriage_id, float speed);

    static task_state_t set_carriage_and_speed(uint8_t carriage, float speed);


private:

    static float *const machine_coords;

    static uint8_t mode;

    static uint8_t carriage_id;

    static float *const position;

    static task_state_t mode_0(float x, float y, float z);

    static void sanity_check(float *hl_coords);

};


#endif //TRACER_COORDINATESINTERFACE_H
