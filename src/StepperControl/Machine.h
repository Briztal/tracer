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
#include <TaskScheduler/TaskScheduler.h>



class Machine {


    //-------------------------------------------------Power------------------------------------------------------------

public:

    static void disable_stepper_power();


    //-----------------------------------------------Movement-----------------------------------------------------------


    //Task interfaces :

public:

    static task_state_t carriages_reset();

    GENERATE_SCHEDULER(carriages_reset, 0);


    static task_state_t line_to(float x, float y, float z);

    GENERATE_SCHEDULER(line_to, 3, float, x, float, y, float, z);


    static task_state_t line_of(float x, float y, float z);

    GENERATE_SCHEDULER(line_of, 3, float, x, float, y, float, z);


    static task_state_t carriage_set(uint8_t carriage_id);

    GENERATE_SCHEDULER(carriage_set, 1, uint8_t, carriage_id);


    static task_state_t speed_set(uint8_t carriage_id, float speed);

    GENERATE_SCHEDULER(speed_set, 2, uint8_t, carriage_id, float, speed);


    static task_state_t carriage_speed_set(uint8_t carriage, float speed);

    GENERATE_SCHEDULER(carriage_speed_set, 2, uint8_t, carriage_id, float, speed);



private:

    static float *const machine_coords;

    static uint8_t mode;

    static uint8_t carriage_id;

    static float *const position;

    static task_state_t mode_0(float x, float y, float z);

    static void sanity_check(float *hl_coords);

};


#endif //TRACER_COORDINATESINTERFACE_H
