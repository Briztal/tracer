/*
  MotionBezier.cpp - Part of TRACER

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


/*

#include "MotionBezier.h"
#include "../swap.hpp"

void MotionBezier::move() {
    ComplexMovement::enqueue_movement();
}

void MotionBezier::prepareMotion() {

    ComplexMovement::initialise_motion();

    increment_provided = false;

    if (curve_width < 2) return;

    this->nbPoints = curve_height - 1;

    float steps0=steps[0], steps1=steps[1];

    for (int i = 0; i <= nbPoints; i++) {

        polynomPoints[i][0] = (int32_t) (*(curve_t + NB_STEPPERS * i) * steps0);
        polynomPoints[i][1] = (int32_t) (*(curve_t + NB_STEPPERS * i + 1) * steps1);
    }

    //TODO NUANCER SELON SI IL Y A ACCELERTION OU PAS.

    destinations[0] = polynomPoints[0][0];
    destinations[1] = polynomPoints[0][1];

}

void MotionBezier::init_position_parameters() {

    min = 0, max = 1;
    //Polynom coefficients
    float factCoeff;
    float cx, cy;
    float coeff;
    int minus;

    last_positions[0] = last_pos_t[0] = polynomPoints[nbPoints][0];
    last_positions[1]= last_pos_t[1] = polynomPoints[nbPoints][1];

    //Calculate Polynomial coefficients
    for (int j = 0; j <= nbPoints; j++) {
        cx = cy = 0;
        factCoeff = (float) fact(nbPoints, nbPoints - j + 1);
        for (int i = 0; i <= j; i++) {
            minus = ((i + j) % 2) ? -1 : 1;
            coeff = (float) minus / (float) (fact(i, 1) * fact(j - i, 1));
            cx += coeff * polynomPoints[i][0];
            cy += coeff * polynomPoints[i][1];
        }
        polynomCoefficients[j][0] = factCoeff * cx;
        polynomCoefficients[j][1] = factCoeff * cy;
    }

}

void MotionBezier::get_position(float alpha, int32_t *pos) {
    float pow;
    int32_t tx, ty;
    tx = (int32_t) polynomCoefficients[0][0];
    ty = (int32_t) polynomCoefficients[0][1];
    pow = alpha;
    for (int j = 1; j <= nbPoints; j++) {
        tx += (int32_t) (polynomCoefficients[j][0] * pow);
        ty += (int32_t) (polynomCoefficients[j][1] * pow);
        pow *= alpha;
    }
    pos[0] = tx;
    pos[1] = ty;
}

bool MotionBezier::process_position(pos_data *p) {
    /*
    switch (position_processing_step) {
        case 0 :
            pow = alpha;
            next0 = (int32_t) polynomCoefficients[0][0];
            next1 = (int32_t) polynomCoefficients[0][1];
            processIndice = 1;
            position_processing_step = 1;
            break;
        case 1 :
            next0 += (int32_t) (polynomCoefficients[processIndice][0] * pow);
            next1 += (int32_t) (polynomCoefficients[processIndice][1] * pow);

            TODO REFORM
            if (processIndice == nbPoints) {
                get_new_position = false;
                return;
            }
            pow *= alpha;
            processIndice++;

            break;
        default :
            get_new_position = false;
            break;

    }
     */
//}










