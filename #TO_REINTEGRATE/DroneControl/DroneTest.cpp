/*
  DroneTest.cpp - Part of TRACER

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

#include "DroneTest.h"

void DroneTest::createMotors() {

    /*

     Pentacopter
    registerMotor(0, 1, 1, 0, true, 1, 1, 1, 1, 1, 0);
    registerMotor(1, 1, -1, 1, false, 1, 1, 1, 1, 1, 0);
    registerMotor(2, -1, -1, 0, true, 1, 1, 1, 1, 1, 0);
    registerMotor(3, -1, 1, -1, false, 1, 1, 1, 1, 1, 0);
    registerMotor(4, 0, 1, -1, false, 1, 1, 1, 1, 1, 0);
     */

    registerMotor(0, 0, -1, 0, 0, 1, 0);
    registerMotor(1, -1, 0, (float)HALF_PI, 0, 1, 0);
    registerMotor(2, 0, 1, 0, 0, 1, 0);

}



void DroneTest::createRelations(LinearSystem *s) {

    /*
     * Pentacopter

    float t0[4]{0, 0, 0, 0}, t1[5]{1, 0, 0, 1, 2};
    s->addEquation(t0, 4, t1, 5);


     */
}
