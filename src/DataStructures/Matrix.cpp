/*
  Matrix.cpp - Part of TRACER

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

#include <string.h>
#include <Interaction/Interaction.h>

#include "Matrix.h"


/*
 * Coefficient Manipulation Macros :
 *
 * As the matrix is represented by a 1D array, some operations will be required to manipulate coefficients;
 *
 * Macros defined below ease this process;
 */


//Get a coefficient in the matrix;
#define get_coeff(line_index, column_index) (data_array[width * (line_index) + (column_index)])

//Set a coefficient in the matrix;
#define set_coeff(line_index, column_index, value, instance) {(instance)->data_array[width * (line_index) + (column_index)] = (value);}

//A boolean expression, true if the required line doesn't exist;
#define invalid_line(line_index) ((line_index) >= height)

//A boolean expression, true if the required column doesn't exist;
#define invalid_column(column_index) ((column_index) >= width)

//A boolean expression, true if the required coefficient doesn't exist;
#define outside_array(line_index, column_index) (invalid_line(line_index) || invalid_column(column_index))


/*
 * default constructor : initialises height and width to zero, and data array to nullptr;
 */

Matrix::Matrix() : height(0), width(0), data_array(nullptr) {

}


/*
 * Constructor : initialises height and width, allocates data_array in the heap, and set all data to zero;
 */

Matrix::Matrix(uint8_t height, uint8_t width) : height(height), width(width), data_array(new float[height * width]) {

    //Initialise the whole matrix to zero;
    reset();

}


/*
 * Destructor : deletes data_array;
 */

Matrix::~Matrix() {

    //Delete the array;
    delete[] data_array;

}


/*
 * subMatrix : this function creates a new matrix, with inferior height and width than our,
 *  and the exact same data at shared coefficients;
 *
 *  All data is copied.
 */

Matrix *Matrix::subMatrix(const uint8_t new_height, const uint8_t new_width) {

    //First, let's check that the required dimensions fit in our instance. If not, return an zero-size matrix;
    if ((new_height > height) || (new_width > width))
        return new Matrix();

    //First, create a new matrix in the heap;
    Matrix *new_matrix = new Matrix(new_height, new_width);

    //Then, we will copy the respective content of the matrix.

    //For each line;
    for (uint8_t line_index = 0; line_index < new_height; line_index++) {

        //For each coefficient in the line;
        for (uint8_t column_index = 0; column_index < new_width; column_index++) {

            //Get the coefficient from our instance;
            float coeff = get_coeff(line_index, column_index);

            //Copy the coefficient in the new instance;
            set_coeff(line_index, column_index, coeff, new_matrix);

        }
    }

    //Return the newly created and filled matrix;
    return new_matrix;
}


//--------------------------------- Coefficient Manipulation ---------------------------------

/*
 * reset : sets all coefficients to zero;
 */

void Matrix::reset() {

    //Hard reset;
    memset(data_array, 0, height * width * sizeof(float));

}


/*
 * getCoefficient : returns the value of the coefficient at (line_index, column_index);
 */

float Matrix::getCoefficient(uint8_t line_index, uint8_t column_index) {

    //If the required value is outside the array, return zero by default;
    if (outside_array(line_index, column_index)) {
        return 0;
    }

    //Return the value of the required coefficient;
    return get_coeff(line_index, column_index);

}


/*
 * setCoefficient : updates the value of the coefficient at (line_index, column_index);
 */

void Matrix::setCoefficient(uint8_t line_index, uint8_t column_index, float new_value) {

    //If the required value is outside the array, fail;
    if (outside_array(line_index, column_index)) {
        return;
    }

    //Update the value of the required coefficient in our instance;
    set_coeff(line_index, column_index, new_value, this);

}


/*
 * resetLine : this function sets the entire required line to zero;
 */

void Matrix::setLine(const uint8_t line_index) {

    //If the required line is outside the array, fail;
    if (invalid_line(line_index))
        return;

    //Cache the pointer to the first coefficient to modify
    float *coefficient_p = data_array + width * line_index;

    //Reset all coefficients;
    memset(coefficient_p, 0, width * sizeof(float));

}


/*
 * setLine : sets the line coefficient by coefficient to the [line_index]-th line of the matrix;
 */

void Matrix::setLine(const uint8_t line_index, const float *line, const float multiplier) {

    //If the required line is outside the array, fail;
    if (invalid_line(line_index))
        return;

    //Cache the pointer to the first coefficient to modify
    float *coefficient_p = data_array + width * line_index;

    //For every coefficient to the required line :
    for (uint8_t column_index = 0; column_index < width; column_index++) {

        //Set the current coefficient's value as the required value, and update both data pointer;
        *(coefficient_p++) = multiplier * (*(line++));

    }

}


/*
 * sumLine : sums the line coefficient by coefficient to the [line_index]-th line of the matrix;
 */

void Matrix::sumLine(const uint8_t line_index, const float *line, const float multiplier) {

    //If the required line is outside the array, fail;
    if (invalid_line(line_index))
        return;

    //Cache the pointer to the first coefficient to modify
    float *coefficient_p = data_array + width * line_index;

    //Sum every coefficient to the required line :
    for (uint8_t column_index = 0; column_index < width; column_index++) {

        //Sum the required value to the current coefficient, and update both data pointer;
        *(coefficient_p++) += multiplier * (*(line++));

    }

}


/*
 * getLine : this function returns a pointer to the required line;
 */

const float *Matrix::getLine(const uint8_t line_index) {

    //If the line is invalid, return the line zero for safety;
    if (invalid_line(line_index)) {

        //Log;
        std_out("Error in Matrix::getLine : the requested line doesn't exist. Returning line zero.");

        //Fail
        return data_array;

    }

    //Return the appropriate line index;
    return data_array + width * line_index;

}


/*
 * invert : this function will attempt to invert the matrix using a gauss pivot algorithm;
 *
 *  If the matrix is not invertible, it will return a null pointer;
 */

Matrix *Matrix::invert() {
    return nullptr;
}
