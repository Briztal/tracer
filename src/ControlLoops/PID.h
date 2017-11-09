/*
  PID.h - Part of TRACER

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

#ifndef TRACER_PIDS_H
#define TRACER_PIDS_H


class PID {

    /*
     * PIDs compute a value with an input.
     *
     * This value is based on the input, the input's sum, and the input's variation.
     *  More litteraly, it verifies the following equation :
     *      output = kp * input + ki * sum + kd * variation
     *
     */


public:

#define PID(i, name, kp, ki, kd)\
    /*compute function*/\
    static float compute_##name(float error);\
    /*reset function*/\
    static void reset_##name();

#include <config.h>

#undef PID

private:

    static float *const lasts;
    static float *const sums;


};


#endif //TRACER_PIDS_H
