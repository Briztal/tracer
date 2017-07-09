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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "swap.hpp"

//TODO ON ENLEVE LES REFERENCES

void swap(long &a, long &b) {
    long  tl = a;
    a = b;
    b = tl;
}

void swap(unsigned char &a, unsigned char &b) {
    unsigned char ti = a;
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




void swap(long *a, long *b) {
    long  tl = *a;
    *a = *b;
    *b = tl;
}

void swap(unsigned char *a, unsigned char  *b) {
    unsigned char  ti = *a;
    *a = *b;
    *b = ti;
}

void swap(unsigned long *a, unsigned long *b) {
    unsigned long tui = *a;
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

void swap(unsigned char *axa, bool *dira, unsigned long *dista, unsigned char *axb, bool *dirb, unsigned long *distb) {
    swap(axa, axb);
    swap(dira, dirb);
    swap(dista, distb);
}

void swap(float &va, float &stepa, float &acca, bool &dira, long &dista, long &desta,
          float &vb, float &stepb, float &accb, bool &dirb, long &distb, long &destb) {
    swap(va, vb);
    swap(stepa, stepb);
    swap(acca, accb);
    swap(dira, dirb);
    swap(dista, distb);
    swap(desta, destb);
}

