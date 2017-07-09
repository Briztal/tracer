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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef CODE_SWAP_H
#define CODE_SWAP_H

void swap(long &a, long &b);

void swap(float &a, float &b);

void swap(bool &a, bool &b);

void swap(float &va, float &stepa, float &acca, bool &dira, long &dista, long &desta,
          float &vb, float &stepb, float &accb, bool &dirb, long &distb, long &destb);

void swap(unsigned char *axa, bool *dira, unsigned long *dista,
          unsigned char *axb, bool *dirb, unsigned long *distb);

#endif //CODE_SWAP_H
