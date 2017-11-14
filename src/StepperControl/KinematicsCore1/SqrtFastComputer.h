/*
  SqrtFastComputer.h - Part of TRACER

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


#ifndef TRACER_SQRTFASTCOMPUTER_H
#define TRACER_SQRTFASTCOMPUTER_H


#include <stdint.h>

class SqrtFastComputer {

public :

    uint16_t sqrt_slow(uint32_t primitive);

    uint16_t sqrt_fast(uint32_t primitive);

    void print_data();


private :

    uint32_t primary = 0;
    uint16_t primary_inf = 1, primary_sup = 1;
    uint16_t square_root = 0;
    uint16_t primary_increment = 1;

    uint16_t _sqrt_fast(bool dir, uint16_t increment);


};


#endif //TRACER_SQRTFASTCOMPUTER_H
