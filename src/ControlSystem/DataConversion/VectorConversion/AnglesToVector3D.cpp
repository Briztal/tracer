//
// Created by root on 3/2/18.
//

#include "AnglesToVector3D.h"

#include <math.h>
#include <Interaction/Interaction.h>

#define ANGULAR_THRESHOLD 0.001

/*
 * AnglesToVector3D : this function will convert the given set of angles to an unitary vector;
 */

void AnglesToVector3D::convert(const Triplet &angles, Vector3D &output_vector) {

    //First, get values;
    const float *const angles_p = angles.get_data();

    float ax = angles_p[0];
    float ay = angles_p[1];
    float az = angles_p[2];

    //Format angles in ]-pi; pi]
    ax = formatAngle(ax);
    ay = formatAngle(ay);
    az = formatAngle(az);

    //First, build the output;
    output_vector.reset();

    //The left part of the equation x2 + y2 + z2 = 1
    bool equation[3]{true, true, true};

    //The right part of the equation x2 + y2 + z2 = 1
    float result = 1;


    /*
     * The first step is to build sorting data for all angles;
     */

    //Declare data;
    relation_data data_x{}, data_y{}, data_z{};

    //Build data;
    build_relation_data(data_x, 0, ax, 1, 2);
    build_relation_data(data_y, 0, ay, 2, 0);
    build_relation_data(data_z, 0, az, 0, 1);

    //Build the data array;
    relation_data *data_array[3]{&data_x, &data_y, &data_z};

    //Compute epsilons for all relations, and eventually nullify coefficients;
    uint8_t dimension = getEpsilons(data_array, equation);

    //Then, sort relations (bubble sort);
    sortRelations(data_array);

    /*
     * Now comes the resolution part.
     *
     *  As angles may have been irregular, dimension might have been lost.
     *
     *  We must check the dimension for all three iterations;
     */

    //If all angles are regular;
    if (dimension == 3) {

        /*
         * We have access to two relations, that have one common vector.
         * We will use these relations to convert to this vector, and to solve one variable;
         */

        relation_data *r0 = data_array[0];
        relation_data *r1 = data_array[1];

        //First, let's check the variable that we will focus on :

        //The pivot will be the variable we convert to;
        uint8_t pivot;

        //One relation will have to be inverted;
        bool r0_inverted = false;

        //Initialise the pivot, and check if it is correct;
        if ((pivot = r0->in) != r1->out) {

            //If the pivot is incorrect :
            pivot = r0->out;
            r0_inverted = true;

        }

        //Compute the tangent of the first relation's angle and save it;
        float tan = tanf(r0->angle);
        float square_sum = tan * tan;

        //If the ratio must be inverted :
        if (r0_inverted) square_sum = (float) 1 / square_sum;

        //Compute the tangent of the second relation's angle;
        if (r0_inverted) {
            tan = tanf(r1->angle);
        } else {
            tan = tanf(-r1->angle);
        }

        //Update the square sum;
        square_sum += tan * tan + (float) 1;

        //We will now determine the square of the variable's value;
        float square_value = 1 / square_sum;

        //The value we have computed here is the absolute value of the variable;
        result -= square_value;

        //Determine the absolute value of the variable;
        float absolute_value = sqrtf(square_value);

        //We must determine the sign of the variable. To do this, let's cache the angle of the relation where the pivot
        // is the output;
        float angle = (r0_inverted) ? r0->angle : r1->angle;

        //Finally, we can determine the exact value;
        output_vector.set(pivot, (angle < 0) ? -absolute_value : absolute_value);

        //Nullify the coefficient in the equation;
        equation[pivot] = false;

        //Reduce the dimension of 1;
        dimension--;

    }

    //Now that we have surely reduced to dimension 2, let's study the case :
    if (dimension == 2) {



        //Determine indices;
        uint8_t relation_index = getRelationIndexLevel2(equation, data_array);

        //Cache the relation pointer;
        relation_data *relation = data_array[relation_index];

        //Cache the pivot;
        uint8_t pivot = relation->in;

        //Cache the tan;
        float tan = tanf(relation->angle);

        //Determine the square value of the pivot variable :
        float square_value = result / ((float) 1 + tan * tan);

        //Subtract the absolute value to the right member;
        result -= square_value;

        //Determine the value for the pivot;
        float absolute_value = sqrtf(square_value);

        /*
         * We must determine the sign of the value. As the pivot is the input of the relation, it will only be positive
         *  if the absolute value of the angle is inferior to pi / 2;
         */

        float angle = relation->angle;
        if (angle < 0) angle = -angle;

        //If the absolute angle is inferior to pi / 2, set the absolute value. If not, set its opposite;
        output_vector.set(pivot, (angle < M_PI_2) ? absolute_value : -absolute_value);

        //Nullify the coefficient in the equation;
        equation[pivot] = false;


    }

    //We now know for sure that the dimension is 1;

    /*
     * First, let's determine the last coefficient;
     */


    uint8_t pivot = 3;

    for (; pivot--;) {
        if (equation[pivot]) {
            break;
        }
    }

    //Cache the angle;
    float angle = 0;

    //The, we must find the angle of the equation where the pivot is the output;
    for (uint8_t relation_index = 3; relation_index--;) {

        //Cache the pointer;
        relation_data *ptr = data_array[relation_index];

        //If the relation is correct :
        if (ptr->out == pivot) {

            //Save the angle and break;
            angle = ptr->angle;
            break;

        }

    }

    //Determine the last value;
    output_vector.set(pivot, (angle < 0) ? -sqrtf(result) : sqrtf(result));

}


/*
 * formatAngle : this function will format the angles;
 */

float AnglesToVector3D::formatAngle(float angle) {

    //While the angle is below -pi :
    while (angle <= -(float) M_PI) {

        //Add 2pi;
        angle += M_2_PI;

    }

    //While the angle is beyond pi :
    while (angle > (float) M_PI) {

        //Subtract 2 pi;
        angle -= M_2_PI;

    }

    //Return the modified angle;
    return angle;

}

/*
 * getIndices : this function will get the two indices of trues in the array (array of 3);
 */

uint8_t AnglesToVector3D::getRelationIndexLevel2(bool *equation, relation_data **data_array) {

    //First, we will get indices of
    uint8_t i0 = 0, i1 = 0;

    if (!equation[0]) {
        i0 = 1, i1 = 2;
    } else if (!equation[1]) {
        i0 = 2, i1 = 0;
    } else if (!equation[2]) {
        i0 = 0, i1 = 1;
    }

    //For each relation :
    for (uint8_t relation_index = 3; relation_index--;) {

        //Cache the relation;
        relation_data *r = data_array[relation_index];

        //Verify the couple of indices;
        if (((r->in == i0) && (r->out == i1)) || ((r->in == i1) && (r->out == i0))) {

            //If we have found a compatible equation, return its index;
            return relation_index;

        }

    }

    return 0;

}


/*
 * build_relation_data  this function will simply copy data inside the struct;
 */

void AnglesToVector3D::build_relation_data(relation_data &data, float epsilon, float angle, uint8_t in, uint8_t out) {

    //Simply copy all data;
    data.angle = angle;
    data.epsilon = epsilon;
    data.in = in;
    data.out = out;

}


/*
 * getEpsilons : this function will compute the epsilon for a given angle;
 *
 *  If it detects an angle that is proportional to pi / 2, it will nullify the corresponding variable
 *      in the coefficient array;
 *
 *  Reminder : epsilon(angle) = abs (angle mod[pi / 2] - pi / 4);
 *
 */

uint8_t AnglesToVector3D::getEpsilons(relation_data **data, bool *equation) {

    //Start by defining the dimension of the problem to solve;
    uint8_t dimension = 3;

    //For each relation :
    for (uint8_t i = 3; i--;) {

        //Cache the relation
        relation_data *relation = data[i];

        //Cache the angle;
        float angle = relation->angle;

        //A flag, set if we rotated a pair number of times;
        bool pair_rotation = true;

        //Reduce the angle while it is superior to pi / 2 :
        while (angle > (float) M_PI_2) {
            angle -= M_PI_2;
            pair_rotation = !pair_rotation;
        }

        //Increase the angle while it is inferior to 0 :
        while (angle < (float) 0) {
            angle += M_PI_2;
            pair_rotation = !pair_rotation;
        }

        //If the angle is below the zero threshold :
        if (angle < ANGULAR_THRESHOLD) {

            //If we rotated a pair number of time, the in variable is null;
            if (pair_rotation) {
                if (equation[relation->out]) {
                    equation[relation->out] = false;
                    dimension--;
                }
            } else {
                //If we rotated an impair number of time, the out variable is null;
                if (equation[relation->in]) {
                    equation[relation->in] = false;
                    dimension--;
                }
            }


        } else if ((float) M_PI_2 - angle < ANGULAR_THRESHOLD) {

            //If we rotated a pair number of time, the out variable is null;
            if (pair_rotation) {
                if (equation[relation->in]) {
                    equation[relation->in] = false;
                    dimension--;
                }
            } else {
                //If we rotated an impair number of time, the out variable is null;
                if (equation[relation->out]) {
                    equation[relation->out] = false;
                    dimension--;
                }
            }

        }


        //Get the difference with PI / 4;
        float r = angle - (float) M_PI_2;

        //return the absolute value of the difference;
        relation->epsilon = (r < 0) ? -r : r;

    }

    //Return the dimension;
    return dimension;

}


/*
 * sortRelations : this function will sort the three relations of the array;
 */

void AnglesToVector3D::sortRelations(relation_data **data_array) {

#define SWITCH_IF_INFERIOR(index0, index1)\
    if (data_array[index0]->epsilon < data_array[index1]->epsilon) {\
        switchRelations(data_array, index0, index1);\
    }\

    //Execute the three steps of a bubble sort on a size-3 array;
    SWITCH_IF_INFERIOR(0, 1);
    SWITCH_IF_INFERIOR(1, 2);
    SWITCH_IF_INFERIOR(0, 1);

    //Undef the macro for safety;
#undef SWITCH_IF_INFERIOR

}


/*
 * switchRelations : this function will switch the two concerned pointers of the array;
 */

void AnglesToVector3D::switchRelations(relation_data **data_array, uint8_t index_0, uint8_t index_1) {

    //Save the first case;
    relation_data *ptr = data_array[index_0];

    //Put the second case in the first case;
    data_array[index_0] = data_array[index_1];

    //Put the saved first case in the second case;
    data_array[index_1] = ptr;

}

