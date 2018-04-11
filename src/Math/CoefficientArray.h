/*
  Equation.h - Part of TRACER

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


#ifndef TRACER_EQUATION_H
#define TRACER_EQUATION_H

#include <WString.h>
#include <DataStructures/string/string.h>
#include <DataStructures/string/tstring.h>
#include "stdint.h"

class CoefficientArray {


    //------------------------------------------- Initialisation --------------------------------------------

public:

    //Constructor : initialises the coefficients array with zeros;
    explicit CoefficientArray(uint8_t size);

    //Destructor : frees the coefficients array;
    ~CoefficientArray();


    //------------------------------------------- Fields --------------------------------------------

private:

    //The equation size (the number of coefficients);
    const uint8_t size;

    //The coefficients array;
    float *coefficients;


    //------------------------------------------- Coefficients manipulation --------------------------------------------

public:

    //Get the coefficient a a given index;
    float getCoefficient(uint8_t index);

    //Set the coefficient at a given index;
    void setCoefficient(uint8_t index, float new_value);

    //Set the coefficients;
    void setCoefficients(const float* coefficients, uint8_t size);


    //--------------------------------------------- CoefficientArray manipulation ----------------------------------------------

public:

    //Add the given equation to our equation;
    void addArray(CoefficientArray *array);

    //Add the given equation multiplied by the given coefficient to our equation;
    void addArray(float factor, CoefficientArray *array);

    //Multiply all coefficients by the given factor
    void multiply(float factor);

    //Create a string displaying the data of the array;
    tstring toString();

};


#endif //TRACER_EQUATION_H
