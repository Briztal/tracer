/*
  ContinuousActions.h - Part of TRACER

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

#ifndef CODE_CONTINUOUSACTIONS_H
#define CODE_CONTINUOUSACTIONS_H

#include <stdint.h>
#include <config.h>

class ContinuousActions {

public:

    static void begin();

#define CONTINUOUS(i, name, pin, max) \
    static void set_power##i(float f);\
    static void stop_##i();

#include <config.h>

#undef CONTINUOUS

};


#endif //CODE_CONTINUOUSACTIONS_H
