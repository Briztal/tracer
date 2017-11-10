/*
  TempControl.h - Part of TRACER

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


#ifndef TRACER_CLOSEDLOOPS_H
#define TRACER_CLOSEDLOOPS_H


class TempControl {


public:

    //---------------------------Interrupt regulation functions---------------------------

    //The function to initialise the temperature regulation routine
    static inline void regulation_init();

    //The routine for temperature regulation
    static inline void temperature_regulation();


    //---------------------------State selection functions---------------------------

    //Hotends
    static void enable_hotend_regulation(uint8_t hotend);

    static void disable_hotend_regulation(uint8_t hotend);


    //Hotbed

    static void enable_hotbed_regulation();

    static void disable_hotbed_regulation();



private:

    //The temperature targets
    static float *const temp_targets;

    //The regulation flags : i-th is set if the i-th thermistor's regulation is enabled.
    static bool *const temp_regulation_enabled;


};


#endif //TRACER_CLOSEDLOOPS_H
