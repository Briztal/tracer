/*
  ServoActions.h -  Part of TRACER

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

#ifndef CODE_SERVOACTIONS_H
#define CODE_SERVOACTIONS_H
#include "../External/Servo.h"

class ServoActions {

public:

#define SERVO(i, name, pin, min, max) \
    static void set##i(float f);

#include "../config.h"
#undef SERVO

private:

#define SERVO(i, name, pin, min, max) \
    static Servo* servo##i;

#include "../config.h"
#undef SERVO

    void begin();
};


#endif //CODE_SERVOACTIONS_H
