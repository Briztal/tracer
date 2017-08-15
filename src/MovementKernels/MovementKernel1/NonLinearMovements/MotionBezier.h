/*
  MotionBezier.h - Part of TRACER

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

#ifndef CODE_MOTIONBEZIER_H
#define CODE_MOTIONBEZIER_H

#include "NonLinearMovement.h"

class MotionBezier : public ComplexMovement {

public:

    void prepareMotion();

    void move();

protected:

    //Curve Points Fields
    int32_t polynomPoints[MAX_CURVE_POINTS][2];
    float polynomCoefficients[MAX_CURVE_POINTS][2];
    int nbPoints;

    //Coords Processing variables
    float pow;
    int processIndice;

    bool process_position();

    void get_position(float alpha, int32_t *pos);

    void init_position_parameters();

};



#endif //CODE_MOTIONBEZIER_H
