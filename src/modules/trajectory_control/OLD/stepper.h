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

//The trajectory_control data structure : contains all data related to a trajectory_control motor
typedef struct stepper_t {

    //The feedrate, ie the number of tics per high level unit;
    float feedrate;

	//The maximal speed in tics per second;
    float maximum_speed;

	//The maximal acceleration, in tics per second^2
    float acceleration;

	//The maximal discontinuous speed variation, in tics per second;
    float jerk;

};


#endif

