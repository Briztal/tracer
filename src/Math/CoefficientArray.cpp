/*
  Equation.cpp - Part of TRACER

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

#include "CoefficientArray.h"

#include <math.h>
#include <DataStructures/string/tstring.h>

#include "hardware_language_abstraction.h"

/*
 * Some macros to help with index and coefficients management;
 */

//A macro to check if the index is in the array;
#define invalid_index(index) ((index) >= size)


//-------------------------------------------------- Initialisation ----------------------------------------------------

/*
 * Constructor : initialises the coefficients array;
 */

CoefficientArray::CoefficientArray(uint8_t size) : size(size), coefficients(new float[size]{0}) {}


/*
 * Destructor : delete the coefficients array;
 */

CoefficientArray::~CoefficientArray() {

    //Delete the array;
    delete[] coefficients;

}

//--------------------------------------------- Coefficients manipulation ----------------------------------------------

/*
 * getCoefficient : returns the coefficient at [index];
 */

float CoefficientArray::getCoefficient(uint8_t index) {

    //If the index is invalid, return 0 for safety;
    if (invalid_index(index))
        return 0;

    //Return the appropriate coefficient;
    return coefficients[index];

}


/*
 * setCoefficient : set the coefficient a [index] to [new_value];
 */

void CoefficientArray::setCoefficient(uint8_t index, float new_value) {


    //If the index is invalid, do nothing;
    if (invalid_index(index))
        return;

    //Modify the value only if it is not nan;
    coefficients[index] = new_value;

}


/*
 * setCoefficients : copy the content of [coefficients] into our array;
 */

void CoefficientArray::setCoefficients(const float *coefficients, uint8_t size) {

    //If the array is too long, set the maximum index;
    if (this->size < size)
        size = this->size;

    //For each value in the coefficients array :
    for (uint8_t index = 0; index < size; index++) {

        //Cache the new coefficient;
        float new_coefficient = coefficients[index];

        //Modify the value only if it is not nan;
        this->coefficients[index] = new_coefficient;

    }

}


//------------------------------------------- CoefficientArray manipulation --------------------------------------------

/*
 * addArray : add an entire array;
 */

void CoefficientArray::addArray(CoefficientArray *array) {

    //The size to copy is the minimum of both sizes.
    uint8_t size = (this->size < array->size) ? this->size : array->size;

    //For each value in the coefficients array :
    for (uint8_t index = 0; index < size; index++) {

        //Cache the new coefficient;
        float new_coefficient = array->coefficients[index];

        //Modify the value only if it is not nan;
        if (!isnanf(new_coefficient)) {
            this->coefficients[index] += new_coefficient;
        }

    }

}


/*
 * addArray : add an entire array, coefficient by coefficient, multiplied by the given factor;
 */

void CoefficientArray::addArray(float factor, CoefficientArray *array) {

    //The size to copy is the minimum of both sizes.
    uint8_t size = (this->size < array->size) ? this->size : array->size;

    //For each value in the coefficients array :
    for (uint8_t index = 0; index < size; index++) {

        //Cache the new factor;
        float new_coefficient = array->coefficients[index];

        //Modify the value only if it is not nan;
        this->coefficients[index] += factor * new_coefficient;

    }
}


/*
 * multiply : multiplies all coefficients by the given factor;
 */

void CoefficientArray::multiply(float factor) {

    //For each value in the coefficients array :
    for (uint8_t index = 0; index < size; index++) {

        //apply the factor to the coefficient;
        this->coefficients[index] *= factor;

    }

}


/*
 * toString : create a string that displays the content of the arrat;
 */

tstring CoefficientArray::toString() {

    //Initialise a string;
    tstring s = "";

    //For each coefficient :
    for (uint8_t coeff_index = 0; coeff_index < size; coeff_index++) {

        //Add its string value to the string, followed by a space;
        s += string(coefficients[coeff_index], 2) + " ";

    }

    //Return the string;
    return s;

}