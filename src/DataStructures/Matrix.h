/*
  Matrix.h - Part of TRACER

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


#ifndef TRACER_MATRIX_H
#define TRACER_MATRIX_H

/*
 * The Matrix class. It represents a simple matrix, and handles math operations on it;
 */


#include "stdint.h"

class Matrix {

public:

    //The default constructor : initialises a zero size matrix;
    Matrix();

    //The constructor : takes sizes in argument, and initialises the data array;
    Matrix(uint8_t height, uint8_t width);

    //The destructor : deletes the data array;
    ~Matrix();

    //Create a sub_matrix;
    Matrix *subMatrix(uint8_t new_height, uint8_t new_width);


    //-------------------------- Computation --------------------------

    float getCoefficient(uint8_t line_index, uint8_t column_index);

    void setCoefficient(uint8_t line_index, uint8_t column_index, float new_value);

    void sumLine(const uint8_t line_index, const float *line, const float multiplier);

    void reset();

    void setLine(const uint8_t line_index, const float *line, const float multiplier);

    void setLine(const uint8_t line_index);

    const float *getLine(const uint8_t line_index);


    //-------------------------- Fields --------------------------

protected:

    //Height;
    const uint8_t height;

    //Width;
    const uint8_t width;

    //The data array;
    float *const data_array;


    //-------------------------- derived constructors  --------------------------

public:

    Matrix *invert();


};


#endif //TRACER_MATRIX_H
