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


#include "../../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef CODE_MOTIONE_H
#define CODE_MOTIONE_H

#include "NonLinearMotionN.h"

#define ANGLE_FRACTION_1

class MotionEllipses {

//TODO LES ELLIPSES DANS LE SENS NEGATIF NE SE TRACENT PAS

public:

    static bool prepare_motion();

    static void move();

protected:

    static float *const sinTable;

    static bool process_position();

    static void get_position(float alpha, long *pos);

    static void init_position_parameters();

    static long center_0, center_1;

    static int begin_angle_d, count_angle_d;//rad

    static float rotation_angle_r, end_angle_r;

    static float A, B;//mm

    static float ca, sa, cb, sb;

    static void trigProcess(int x, float &cos, float &sin);

    bool prepare_motion(unsigned char *axis_t, float *destinations, float A, float B, float begin_angle_r, float V,
                        float rotation_angle_r, float count_angle_r);
};



#endif //CODE_MOTIONBEZIER_H

#endif