/*
  HomingMotion.h - Part of TRACER

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

#ifndef TRACER_HOMINGMOTION_H
#define TRACER_HOMINGMOTION_H


#include <stdbool.h>
#include "../MotionScheduler.h"

class HomingMotion {
public:

    static void move();

    static unsigned char readEndStops();

private:

    static unsigned long getMaxDelay(unsigned char signature, unsigned long *delays);
};

#endif //TRACER_HOMINGMOTION_H
