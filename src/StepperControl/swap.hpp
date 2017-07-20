/*
  swapp.hpp - Part of TRACER

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

#ifndef CODE_SWAP_H
#define CODE_SWAP_H

#include <stdint.h>

void swap(int32_t &a, int32_t &b);

void swap(float &a, float &b);

void swap(bool &a, bool &b);

void swap(float &va, float &stepa, float &acca, bool &dira, int32_t &dista, int32_t &desta,
          float &vb, float &stepb, float &accb, bool &dirb, int32_t &distb, int32_t &destb);

void swap(uint8_t *axa, bool *dira, uint32_t *dista,
          uint8_t *axb, bool *dirb, uint32_t *distb);

#endif //CODE_SWAP_H
