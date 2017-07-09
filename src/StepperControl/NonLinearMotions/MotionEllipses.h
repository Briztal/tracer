/*
  MotionEllipses.h - Part of TRACER

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

#ifndef CODE_MOTIONE_H
#define CODE_MOTIONE_H

#include "NonLinearMotionN.h"

#define ANGLE_FRACTION_1

class MotionEllipses : public NonLinearMotionN {

//TODO LES ELLIPSES DANS LE SENS NEGATIF NE SE TRACENT PAS

public:

    bool prepare_motion();

    void move();

protected:

    static float *const sinTable;

    bool process_position();

    void get_position(float alpha, long *pos);

    void init_position_parameters();

    long center_0, center_1;

    int begin_angle_d, count_angle_d;//rad

    float rotation_angle_r, end_angle_r;

    float A, B;//mm

    float ca, sa, cb, sb;

    void trigProcess(int x, float &cos, float &sin);
};



#endif //CODE_MOTIONBEZIER_H
