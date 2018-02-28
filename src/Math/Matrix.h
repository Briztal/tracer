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

#include "hardware_language_abstraction.h"

class Matrix {

    //-------------------------- Constructors --------------------------

public:

    //The default constructor : initialises a zero size matrix;
    Matrix();

    //The constructor : takes sizes in argument, and initialises the data array;
    Matrix(uint8_t height, uint8_t width);

    //The copy constructor : takes a matrix, and duplicates it entirely;
    explicit Matrix(const Matrix *const src);

    //The copy constructor with size specs : creates a matrix with the given size and copies all possible coeffs;
    Matrix(uint8_t height, uint8_t width, const Matrix *const src);

    virtual //The destructor : deletes the data array;
    ~Matrix();


    //-------------------------- Matrix models --------------------------

    //Identity matrix constructor. Will reset and set 1s in diagonal;
    static void setIdentityMatrix(Matrix *dst);


    //-------------------------- Fields --------------------------

private:

    //Height;
    const uint8_t height;

    //Width;
    const uint8_t width;


protected:

    //The data array;
    float *const data_array;


public:

    //Getter;
    float *getDataArray();

    //-------------------------- Coefficient-wise operations --------------------------


public:

    //Get a the value of a given coefficient;
    float getCoefficient(uint8_t line_index, uint8_t column_index) const;

    //Set the value of a given coefficient;
    void setCoefficient(uint8_t line_index, uint8_t column_index, float new_value);


    //Sum a whole line with the given one;
    void sumLine(const uint8_t dest_line, const uint8_t src_line);

    //Sum a whole line with the given one;
    void sumLine(const uint8_t line_index, const float *line, const float multiplier);

    //Set a line of the matrix to the given one;
    void setLine(const uint8_t line_index, const float *line, const float multiplier);

    //Divide an entire line by a given factor;
    void divideLineBy(uint8_t line_index, float factor);

    //Reset a single line in the matrix;
    void resetLine(const uint8_t line_index);

    //Get a pointer to a line of the matrix;
    const float *getLine(const uint8_t line_index) const;


    //Copy the content of src in our data array;
    void setTo(const Matrix *const src);

    //Subtract the given matrix to our data array;
    void subtract(const Matrix *const src);

    //Divide all coefficients by a given denominator;
    void divideBy(float denominator);

    //Reset the matrix to a null matrix. Doesn't change the size, only resets all coefficients to zero;
    void reset();


    //-------------------------- Minor, cofactors and determinant --------------------------

public:

    //Compute the cofactor at a given position;
    float getCofactor(uint8_t line, uint8_t column) const;


private:

    //Compute a particular minor; not public, because optimised for cofactor computation;
    float getMinor(bool *const columns_flags, uint8_t line, uint8_t disabled_line, const uint8_t size) const;


    //-------------------------- Square matrices operations --------------------------

public:

    virtual //Transpose the matrix;
    void transpose();


    //-------------------------- Inter-matrix operations --------------------------

    //R <- A * B; sizes are checked;
    static void multiply(const Matrix *const A, const Matrix *const B, Matrix *const R);

    //R <- R + A * B; sizes are checked;
    static void multiplyAndAdd(const Matrix *const A, const Matrix *const B, Matrix *const R);

    //R <- R - A * B; sizes are checked;
    static void multiplyAndSubtract(const Matrix *const A, const Matrix *const B, Matrix *const R);


    //-------------------------- Linear operation --------------------------

    //Apply the linear operation represented by the matrix on the vector;
    void apply(const float *const inputVector, float *outputVector) const;


    //-------------------------- derived constructors  --------------------------

public:

    //Compute the inverse of the matrix;
    Matrix *getInverse() const;

    //Compute the transposed of the matrix;
    Matrix *getTransposed() const;

    //Compute the cofactor matrix;
    Matrix *getCofactorMatrix() const;

    //Create a sub_matrix;
    Matrix *subMatrix(uint8_t new_height, uint8_t new_width) const;


    //-------------------------- String --------------------------

public:

    //Create a string that display the matrix;
    String toString() const;

};


#endif //TRACER_MATRIX_H
