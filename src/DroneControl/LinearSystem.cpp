/*
  LinearSystem.cpp - Part of TRACER

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

#include <Interaction/Interaction.h>
#include "LinearSystem.h"

/*
 * Constructor : initialises the equation array;
 */
LinearSystem::LinearSystem(uint8_t nbOutputs, uint8_t nbInputs) :
        nbEquations(0), nbOutputs(nbOutputs), nbInputs(nbInputs), equations(new equation_t[nbInputs]) {

    //Initialise all equations :
    for (uint8_t equation_index = 0; equation_index < nbInputs; equation_index++) {

        //Initialise the right member;
        equations[equation_index].right_member = new CoefficientArray(nbInputs);

        //Initialise the left member;
        equations[equation_index].left_member = new CoefficientArray(nbOutputs);

    }

}

/*
 * Destructor : deletes all equations and equation array;
 */

LinearSystem::~LinearSystem() {

    //Delete all equations :
    for (uint8_t equation_index = 0; equation_index < nbInputs; equation_index++) {

        //Delete the right member;
        delete equations[equation_index].right_member;

        //Delete the left member;
        delete equations[equation_index].left_member;

    }

    //Delete the equations array;
    delete[] equations;

}


/*
 * addSimpleEquation : add an equation, whose left member comprises only one variable, with the coefficient to 1;
 */

void LinearSystem::addSimpleEquation(uint8_t left_index, float *right_member, uint8_t right_member_size) {

    //If no more equation can be added, fail;
    if (nbEquations == nbInputs)
        return;

    //First, cache the current equation's pointer;
    equation_t *equation_p = equations + nbEquations;

    //Set the only coefficient to 1 in the left member;
    equation_p->left_member->setCoefficient(left_index, 1);

    //Set all coefficients in the right member
    equation_p->right_member->setCoefficients(right_member, right_member_size);

    //Finally, increment the number of saved equations;
    nbEquations++;

}


/*
 * addEquation : add an equation;
 */

void LinearSystem::addEquation(const float *left_member, const uint8_t left_member_size, const float *right_member,
                               const uint8_t right_member_size) {

    //If no more equation can be added, fail;
    if (nbEquations == nbInputs)
        return;

    //First, cache the current equation's pointer;
    equation_t *equation_p = equations + nbEquations;

    //Set all coefficients in the left member
    equation_p->left_member->setCoefficients(left_member, left_member_size);

    //Set all coefficients in the right member
    equation_p->right_member->setCoefficients(right_member, right_member_size);

    //Finally, increment the number of saved equations;
    nbEquations++;

}


/*
 * solveSystem : this function solves the equation system, or returns a 0-size matrix if it cannot be solved;
 */

Matrix *LinearSystem::solveSystem() {

    //If all equations have not been added :
    if (nbEquations != nbInputs) {

        //Log;
        std_out("Error in LinearSystem::solveSystem : all equations have not been provided.");

        //Fail returning an empty matrix;
        return new Matrix();

    }

    //Solve the left member system, and get the number of columns to sum after the resolution;
    uint8_t sum_counter = solveLeftMember();

    //If the left member system could'nt be solved :
    if (sum_counter == 0) {

        //Log;
        std_out("Error in LinearSystem::solveSystem : The left members system couldn't be solved.");

        //Fail returning an empty matrix;
        return new Matrix();

    }

    //Then, extract the matrix of the right members system;
    Matrix *systemMatrix = extractMatrix();

    //Invert the matrix;
    Matrix *inverseMatrix = systemMatrix->getInverse();

    //Delete the system matrix;
    delete systemMatrix;

    //Sum last columns of the matrix;
    sumLastColumns(sum_counter, inverseMatrix);

    //Finally, extract the relevant sub-matrix of the inverse matrix;
    Matrix *sub_matrix = inverseMatrix->subMatrix(nbOutputs, nbInputs);

    //Delete the inverse matrix;
    delete inverseMatrix;

    //If the extraction couldn't be made properly :
    if (sub_matrix == nullptr) {

        //Log;
        std_out("Error in LinearSystem::solveSystem : Couldn't extract the sub-matrix.");

        //Fail returning an empty matrix;
        return new Matrix();

    }

    //Return the matrix that solves the system;
    return sub_matrix;
}


/*
 * solveLeftMember : this function applies the Gauss pivot algorithm for the left equation system,
 *  and applies modification in the right equation system, until the left member's representative matrix looks like :
 *
 *      nbOutputs
 *      1 0 0 ... 0
 *      0 1 0 ... 0
 *      0 0 1 ....0
 *      . . .           nbInputs
 *      . . .
 *      0 0 0 ... a
 *      | | | ... |
 *      0 0 0 ... a
 *
 *      where a = 1 / (nbInputs - nbOutputs + 1)
 *
 *      it returns the number of lines to sum after the resolution (here 3);
 */

uint8_t LinearSystem::solveLeftMember() {

    //Cache the number of equations;
    const uint8_t nb_equations = nbInputs;

    //First, we will apply the gauss pivot algorithm for the first nbOutputs-1 lines.

    //Cache the iteration counter;
    const uint8_t nb_iterations = nbOutputs - (uint8_t) 1;


    //The first step is to normalise each line for a valid coefficient, and to eliminate it in next lines;

    //For each line :
    for (uint8_t line_index = 0; line_index < nb_iterations; line_index++) {

        //Get the pointer to the current equation;
        equation_t *equation = equations + line_index;

        //Get the [line_index]-th coefficient of the left member;
        float factor = equation->left_member->getCoefficient(line_index);

        //If the factor is null, we must look for another equation that has its factor not null
        if (!factor) {

            //Declare the switch index;
            uint8_t switch_index = line_index + (uint8_t) 1;

            while (true) {

                //If no valid equation was found, the system cannot be solved;
                if (switch_index == nb_equations) {
                    return 0;
                }

                //Get the next coefficient of the left member;
                factor = (++equation)->left_member->getCoefficient(line_index);

                //If a non null factor is found, stop iterating;
                if (factor)
                    break;

                //Increment the new index;
                switch_index++;
            }

            //Switch the equations at line_index and switch_index.

            //Cache the equation at switch position;
            equation_t eq_cache = *equation;

            //modify the equation at switch position;
            *equation = equations[line_index];

            //Modify the equation at line_index position;
            equations[line_index] = eq_cache;

            equation = equations + line_index;

        }

        //Now, we must normalise the current line.

        //Cache the inverse of the factor;
        float inverse = (float)1 / factor;

        //Divide the current equation by the factor;
        equation->left_member->multiply(inverse);
        equation->right_member->multiply(inverse);


        //Now, we must eliminate the coefficient in all following equations.

        //For each of the following equations :
        for (uint8_t sub_line_index = 0; sub_line_index < nbEquations; sub_line_index++) {

            //Cache the equation :
            equation_t *sub_equation = equations + sub_line_index;

            //Cache the opposite of the [line_index]-th coefficient,;
            float sub_factor = -sub_equation->left_member->getCoefficient(line_index);

            //Add the current equation multiplied by the sub_factor to the sub_equation;;
            sub_equation->left_member->addEquation(sub_factor, equation->left_member);
            sub_equation->right_member->addEquation(sub_factor, equation->right_member);

        }

    }


    //Now, we will normalise the last regular equation :
    const uint8_t last_index = nbOutputs - (uint8_t) 1;

    //Normalise the equation;
    bool normalised = normalise(last_index, last_index, 1);

    //If the normalisation failed :
    if (!normalised)  {

        //Log;
        std_out("Error in LinearSystem::solveLeftMember : The last coefficient is null.");

        //fail;
        return 0;

    }

    //The next step is to repeat the process in the inverse order, to obtain the identity matrix;

    //For each line :
    for (uint8_t line_index = last_index; line_index < nb_iterations; line_index++) {

        //Get the pointer to the current equation;
        equation_t *equation = equations + line_index;

        //Now, we must eliminate the coefficient in all following equations.

        //For each of the following equations :
        for (uint8_t sub_line_index = line_index; sub_line_index > 0; sub_line_index--) {

            //Cache the sub equation :
            equation_t *sub_equation = equations + sub_line_index;

            //Cache the opposite of the [line_index]-th coefficient,;
            float sub_factor = -sub_equation->left_member->getCoefficient(line_index);

            //Add the current equation multiplied by the sub_factor to the sub_equation;;
            sub_equation->left_member->addEquation(sub_factor, equation->left_member);
            sub_equation->right_member->addEquation(sub_factor, equation->right_member);

        }

    }

    //Finally, we will normalise last lines to a;

    //Determine the a coefficient;
    float a = (float)1 / (nbInputs - nbOutputs + 1);


    //For every line:
    for (uint8_t line_index = last_index; line_index < nb_equations; line_index++) {

        //Normalise the line to a;
        normalised = normalise(line_index, last_index, a);

        //If the normalisation failed :
        if (!normalised)  {

            //Log;
            std_out("Error in LinearSystem::solveLeftMember : There are null lines after reorganisation.");

            //fail;
            return 0;

        }

    }

    //Return the number of lines to sum after the resolution;
    return nb_equations - last_index;

}


/*
 * normalise : this function will normalise the given equation, so that the [coefficient_index]'s coefficient
 *  is set to [normalisation_value] after;
 */

bool LinearSystem::normalise(const uint8_t equation_index, const uint8_t coefficient_index, float normalisation_value) {

    //Cache the equation pointer;
    equation_t *equation = equations + equation_index;

    //Get the value of the only remaining coefficient;
    float factor = equation->left_member->getCoefficient(coefficient_index);

    //If the factor is null, fail;
    if (factor == 0) {
        return false;
    }

    //Invert the factor;
    factor = normalisation_value / factor;

    //Normalise the left member;
    equation->left_member->multiply(factor);

    //Divide the right member;
    equation->right_member->multiply(factor);
}


/*
 * extractMatrix : this function will extract the matrix corresponding to the right members system;
 */

Matrix *LinearSystem::extractMatrix() {

    //First, cache the size of the matrix;
    const uint8_t nb_equations = nbInputs;

    //Then, create the matrix, a square one, of size [nbInputs];
    Matrix *systemMatrix = new Matrix(nb_equations, nb_equations);

    //For each equation :
    for (uint8_t equation = 0; equation < nbInputs; equation++) {

        //Cache the right member;
        CoefficientArray *rightMember = equations[equation].right_member;

        //For every coefficient in the right member :
        for (uint8_t coefficient_index = 0; coefficient_index < nb_equations; coefficient_index++) {

            //Set the coefficient in the matrix;
            systemMatrix->setCoefficient(equation, coefficient_index, rightMember->getCoefficient(coefficient_index));

        }

    }

    //Return the matrix;
    return systemMatrix;

}


/*
 * sumLastColumns : this function will sum the [sum_count] last columns of the matrix, and reset the [sum_count - 1]
 *  last ones;
 */

void LinearSystem::sumLastColumns(const uint8_t sum_count, Matrix *matrix) {

    //First, we will transpose the matrix, as only line-wise operations are supported;
    matrix->transpose();

    //Cache the sum_index and the size;
    const uint8_t size = nbInputs;
    const uint8_t sum_index = size - sum_count;

    //For all [sum_count - 1] last columns :
    for (uint8_t line_counter = sum_index + (uint8_t)1; line_counter < size; line_counter++) {

        //Add the line to the one at index sum_index;
        matrix->sumLine(sum_index, line_counter);

        //Reset the line;
        matrix->resetLine(line_counter);

    }

    //Finally, restore the matrix in its original state;
    matrix->transpose();

}