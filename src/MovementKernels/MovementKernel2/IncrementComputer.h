/*
  IncrementComputer.h - Part of TRACER

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

#ifndef TRACER_INCREMENTCOMPUTER_H
#define TRACER_INCREMENTCOMPUTER_H


class IncrementComputer {

public:

    static float extract_increment(void (*get_position)(float, float *), float point, float increment,
                                   const uint32_t distance_target);

private :

    static uint32_t get_max_dists(int32_t *p0, int32_t *p1);

    void static get_stepper_position(void (*get_position)(float, float *), float point, int32_t *positions);

};


#endif //TRACER_INCREMENTCOMPUTER_H
