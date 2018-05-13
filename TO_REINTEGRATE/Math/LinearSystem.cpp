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

#include <Kernel/Interaction/Interaction.h>
#include "LinearSystem.h"


//--------------------------------------------------- Initialisation ---------------------------------------------------

/*
 * Constructor : initialises the equation array;
 */
LinearSystem::LinearSystem(uint8_t nb_outputs) : nbOutputs(nb_outputs), temp_Ai(nb_outputs, nb_outputs, false) {}



//----------------------------------------------------- Resolution -----------------------------------------------------


/*
 * solveSystem : this function solves the equation system, or returns a 0-size matrix if it cannot be solved;
 */

//TODO CORRECT THIS, TAKE THE MATRIX IN ARGUMENT;
void LinearSystem::solve(const Matrix &A, const Matrix &B, Matrix &result) {

    //First, determine the inverse of A;
    A.getInverse(temp_Ai);

    //Then, multiply Ai and B in result;
    result.multiply(temp_Ai, B);

}
