/*
  MultiRotorCoordinateSystem.h - Part of TRACER

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


#ifndef TRACER_MULTIROTORMOTORDATA_H
#define TRACER_MULTIROTORMOTORDATA_H


class MultiRotorCoordinateSystem {

public:

    //Create a default constructor, that will initialise all to false
    MultiRotorCoordinateSystem() = default;

    //Enable the x control;
    bool x_en = false;

    //Enable the y control;
    bool y_en = false;

    //Enable the z control;
    bool z_en = false;

    //Enable the pitch control;
    bool pitch_en = false;

    //Enable the roll control;
    bool roll_en = false;

    //Enable the yaw control;
    bool yaw_en = false;

};


#endif //TRACER_MULTIROTORMOTORDATA_H
