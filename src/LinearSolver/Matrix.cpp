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



//---------------------------------------------------- Constructors ----------------------------------------------------


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

    std_out("sizes : " + String(this->height) + " " + String(this->width));

}


/*
 * Destructor : deletes data_array;
 */

Matrix::~Matrix() {

    //Delete the array;
    delete[] data_array;

}


//---------------------------------------------- Coefficient Manipulation ----------------------------------------------

/*
 * getCoefficient : returns the value of the coefficient at (line_index, column_index);
 */

float Matrix::getCoefficient(uint8_t line_index, uint8_t column_index) const {

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
 * sumLine : sums the dst line to the src line;
 */

void Matrix::sumLine(const uint8_t dest_line, const uint8_t src_line) {

    //If the required line is outside the array, fail;
    if (invalid_line(dest_line) || invalid_line(src_line))
        return;

    std_out("valid lines");

    //Cache the pointer to the first coefficient to modify
    float *src_p = data_array + width * src_line;
    float *dst_p = data_array + width * dest_line;

    //Sum every coefficient to the required line :
    for (uint8_t column_index = 0; column_index < width; column_index++) {

        //Sum the src value to the destination one, and update both data pointer;
        *(dst_p++) += (*(src_p++));

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
 * divideLineBy : divide all coefficients by [factor];
 */

void Matrix::divideLineBy(uint8_t line_index, float factor) {

    //If the line index is invalid, do nothing;
    if (invalid_line(line_index)) {
        return;
    }

    //Invert the factor;
    factor = (float) 1 / factor;

    //Cache the pointer to the first coefficient to modify
    float *coefficient_p = data_array + width * line_index;

    //Divide every coefficient of the required line :
    for (uint8_t column_index = 0; column_index < width; column_index++) {

        //Divide the coefficient;
        *(coefficient_p++) *= factor;

    }

}


/*
 * getLine : this function returns a pointer to the required line;
 */

const float *Matrix::getLine(const uint8_t line_index) const {

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
 * resetLine : this function sets the entire required line to zero;
 */

void Matrix::resetLine(const uint8_t line_index) {

    //If the required line is outside the array, fail;
    if (invalid_line(line_index))
        return;

    //Cache the pointer to the first coefficient to modify
    float *coefficient_p = data_array + width * line_index;

    //Reset all coefficients;
    memset(coefficient_p, 0, width * sizeof(float));

}


//------------------------------------------------ Matrices Operations -------------------------------------------------


/*
 * getCofactor : this function determines the cofactor at the given position;
 */

float Matrix::getCofactor(uint8_t line, uint8_t column) const {

    //If the line index of the column index are outside the matrix, return 0 for safety;
    if (invalid_column(column) && invalid_line(line))
        return 0;

    //Initialise a column flag array;
    bool column_flags[height];

    //Set all flags (enable all columns);
    memset(column_flags, true, height * sizeof(bool));

    //Disable the appropriate column;
    column_flags[column] = false;

    //Determine the absolute value of the cofactor;
    float cofactor = getMinor(column_flags, 0, line, height - (uint8_t) 1);

    //If the sum of line and column is a pair, oppose the cofactor;
    if ((line + column) & 1)
        cofactor = -cofactor;

    //Return the cofactor;
    return cofactor;

}


/*
 * getMinor : this function computes the determinant of the sub-matrix described with the provided arguments :
 *
 *  columns_flags : an array of booleans. If the i-th flag is set, then the i-th column will appear in the sun matrix;
 *
 *  line : the line to start the sub-matrix with; all line before are not present in it;
 *
 *  disabled_line : the index of a particular line that doesn't figure in the sub_matrix;
 *
 *  size : the size of the sub-matrix;
 */

float Matrix::getMinor(bool *const columns_flags, uint8_t line, uint8_t disabled_line, const uint8_t size) const {

    if (line == disabled_line)
        line++;

    if (size == 1) {
        //If the sub-matrix is of size 1 :

        //We must get the only remaining column_index;

        //Initialise the column_index counter;
        uint8_t column_index = 0;

        while (true) {

            //if no more columns are available :
            if (column_index == width) {

                //Log;
                std_out("Error in Matrix::getMinor : no more columns are enabled.");

                //Fail;
                return 0;
            }

            //If the column is enabled, stop incrementing;
            if (columns_flags[column_index])
                break;


            //Check the next column;
            column_index++;
        }

        //Determine and return the single value of the matrix;
        return get_coeff(line, column_index);

    }

    //Initialise the result variable to zero;
    float result = 0;

    //If the sub_matrix is of size 2 or greater : we will develop the current line;
    uint8_t next_line = line + (uint8_t) 1;


    //First, find the next available line;
    if (next_line == disabled_line) next_line++;


    //Then, develop this line.

    //Initialise a column counter;
    uint8_t column_counter = 0;

    //For every column :
    for (uint8_t column_index = 0; column_index < width; column_index++) {

        //If the column is disabled, skip it
        if (!columns_flags[column_index])
            continue;

        //If the column is enabled, develop over it;
        float factor = get_coeff(line, column_index);

        //Eventually change its sign in function of the column counter;
        if (column_counter & (uint8_t) 1) factor = -factor;

        //Disable the current column;
        columns_flags[column_index] = false;

        float minor = factor * getMinor(columns_flags, next_line, disabled_line, size - (uint8_t) 1);

        //Add the determinant to the result;
        result += minor;

        //Re-enable the current column;
        columns_flags[column_index] = true;

        //Increment the column counter;
        column_counter++;
    }

    //Return the computed determinant;
    return result;

}


/*
 * transpose : this function will transpose the matrix, without making a copy of it;
 */

void Matrix::transpose() {

    if (height != width) {
        std_out("ERROR in Matrix::transpose() : the matrix is not a square matrix;");
        return;
    }

    //For every row :
    for (uint8_t row_index = 0; row_index < height; row_index++) {

        //For the half part of the row :
        for (uint8_t column_index = 0; column_index < row_index; column_index++) {

            //Cache value (i,j);
            float coefficient = get_coeff(row_index, column_index);

            //Copy value (j, i) in (i, j);
            set_coeff(row_index, column_index, get_coeff(column_index, row_index), this);

            //Copy the cached value in (j, i);
            set_coeff(column_index, row_index, coefficient, this);

        }

    }

}


/*
 * divideBy : this function will divide all coefficients by the provided denominator;
 */

void Matrix::divideBy(float denominator) {

    //For every row :
    for (uint8_t row_index = 0; row_index < height; row_index++) {

        //For the half part of the row :
        for (uint8_t column_index = 0; column_index < width; column_index++) {

            //Get the coefficient, and divideBy it by the denominator;
            float coefficient = get_coeff(row_index, column_index) / denominator;

            //Save the new coefficient;
            set_coeff(row_index, column_index, coefficient, this)

        }

    }

}


/*
 * reset : sets all coefficients to zero;
 */

void Matrix::reset() {

    //Hard reset;
    memset(data_array, 0, height * width * sizeof(float));

}


//----------------------------------------------- Derived constructors  ------------------------------------------------


/*
 * invert : this function will attempt to getInverse the matrix using a gauss pivot algorithm;
 *
 *  If the matrix is not invertible, it will return a null pointer;
 */

Matrix *Matrix::getInverse() const {

    //First, check that the matrix is a square matrix;
    if (height != width) {
        std_out("ERROR in Matrix::getInverse() : the matrix is not a square matrix;");
        return nullptr;
    }

    //Create the cofactor matrix;
    Matrix *cofactorMatrix = getCofactorMatrix();

    //Get the determinant, by multiplying coefficient by coefficient the first line;
    float determinant = 0;

    //For each position of the first line :
    for (uint8_t column_index = 0; column_index < width; column_index++) {

        //Multiply the two coefficients, and add the result to the determinant;
        determinant += get_coeff(0, column_index) * cofactorMatrix->getCoefficient(0, column_index);

    }

    //If the matrix is not invertible :
    if (!determinant) {

        //Log;
        std_out("Error in Matrix::getInverse : the determinant of the matrix is null.");

        //Fail;
        return nullptr;

    }

    /*

    //Display the original matrix;
    std_out("Original matrix :\n" + toString());

    //Display the determinant matrix;
    std_out("Determinant : "+String(determinant));

    //Display the cofactor matrix;
    std_out("Cofactors matrix :\n" + cofactorMatrix->display());

     */
    //Transpose the cofactor matrix;
    cofactorMatrix->transpose();


    /*
    //Display the cofactor matrix;
    std_out("Transposed cofactors matrix :\n" + cofactorMatrix->display());

     */
    //Divide the cofactor matrix by the determinant to get the inverted matrix;
    cofactorMatrix->divideBy(determinant);

    /*
    //Display the cofactor matrix;
    std_out("inverted matrix : \n" + cofactorMatrix->display());
     */

    //Return the inverted matrix;
    return cofactorMatrix;

}


/*
 * getCofactorMatrix : this function computes and returns the cofactor matrix, associated with [this] matrix;
 */

Matrix *Matrix::getCofactorMatrix() const {

    if (height != width) {
        std_out("ERROR in Matrix::getInverse() : the matrix is not a square matrix;");
        return nullptr;
    }

    //Create a new matrix;
    Matrix *cofactorsMatrix = new Matrix(height, width);

    //For each line :
    for (uint8_t row_index = 0; row_index < height; row_index++) {

        //For each coefficient of the line :
        for (uint8_t column_index = 0; column_index < width; column_index++) {

            //Get the cofactor for this position, and save it in the cofactors matrix;
            cofactorsMatrix->setCoefficient(row_index, column_index, getCofactor(row_index, column_index));

        }

    }

    //Return the cofactor matrix;
    return cofactorsMatrix;

}


/*
 * subMatrix : this function creates a new matrix, with inferior height and width than our,
 *  and the exact same data at shared coefficients;
 *
 *  All data is copied.
 */

Matrix *Matrix::subMatrix(const uint8_t new_height, const uint8_t new_width) const {

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
            new_matrix->setCoefficient(line_index, column_index, coeff);
        }

    }

    //Return the newly created and filled matrix;
    return new_matrix;
}


//------------------------------------------------------- String  ------------------------------------------------------


/*
 * display : this function creates a string that contains the matrix.
 */
String Matrix::toString() const {

    //Create the return string;
    String s = "";

    //For every row :
    for (uint8_t row_index = 0; row_index < height; row_index++) {

        //For every coefficient in the row :
        for (uint8_t column_index = 0; column_index < width; column_index++) {

            //Append the numeric value to the string;
            s += String(get_coeff(row_index, column_index), 3) + " ";

        }

        //Add a carriage return for the next line;
        s += "\n";

    }

    //Return the string;
    return s;

}
