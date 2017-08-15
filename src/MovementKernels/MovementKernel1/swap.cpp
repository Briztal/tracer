/*
  swapp.cpp - Part of TRACER

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

#include "swap.hpp"

//TODO ON ENLEVE LES REFERENCES

void swap(int32_t &a, int32_t &b) {
    int32_t  tl = a;
    a = b;
    b = tl;
}

void swap(uint8_t &a, uint8_t &b) {
    uint8_t ti = a;
    a = b;
    b = ti;
}

void swap(float &a, float &b) {
    float tf = a;
    a = b;
    b = tf;
}

void swap(bool &a, bool &b) {
    bool tb = a;
    a = b;
    b = tb;
}




void swap(int32_t *a, int32_t *b) {
    int32_t  tl = *a;
    *a = *b;
    *b = tl;
}

void swap(uint8_t *a, uint8_t  *b) {
    uint8_t  ti = *a;
    *a = *b;
    *b = ti;
}

void swap(uint32_t *a, uint32_t *b) {
    uint32_t tui = *a;
    *a = *b;
    *b = tui;
}

void swap(float *a, float *b) {
    float tf = *a;
    *a = *b;
    *b = tf;
}

void swap(bool *a, bool *b) {
    bool tb = *a;
    *a = *b;
    *b = tb;
}

void swap(uint8_t *axa, bool *dira, uint32_t *dista, uint8_t *axb, bool *dirb, uint32_t *distb) {
    swap(axa, axb);
    swap(dira, dirb);
    swap(dista, distb);
}

void swap(float &va, float &stepa, float &acca, bool &dira, int32_t &dista, int32_t &desta,
          float &vb, float &stepb, float &accb, bool &dirb, int32_t &distb, int32_t &destb) {
    swap(va, vb);
    swap(stepa, stepb);
    swap(acca, accb);
    swap(dira, dirb);
    swap(dista, distb);
    swap(desta, destb);
}

