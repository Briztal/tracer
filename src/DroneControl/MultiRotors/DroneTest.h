/*
  DroneTest.h - Part of TRACER

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

#ifndef TRACER_DRONETEST_H
#define TRACER_DRONETEST_H

#include <DroneControl/MultiRotors/Generics/SinglePentaCopter.h>

class DroneTest : public SinglePentaCopter {

public:

    void createMotors() override;

    void solve() override {SinglePentaCopter::solve();}

private:
    void createRelations(LinearSystem *s) override;

};


#endif //TRACER_DRONETEST_H
