//
// Created by root on 3/2/18.
//

#include <string.h>
#include <Interaction/Interaction.h>
#include "Triplet.h"


/*
 * Default Constructor : won't do anything, as data has already been initialised;
 */

Triplet::Triplet() {}


/*
 * Value constructor : will initialise data with given values;
 */

Triplet::Triplet(float v0, float v1, float v2) {

    //Copy all data;
    data[0] = v0;
    data[1] = v1;
    data[2] = v2;

}


/*
 * Copy constructor : copies the data;
 */

Triplet::Triplet(const Triplet &src) {

    //Set our data like src;
    clone(src);

}


//----------------------------------------------- Coordinate Operations ------------------------------------------------


/*
 * Setter : will check the index and set the value;
 */

void Triplet::set(uint8_t index, float value) {

    //If the index is superior to 2 :
    if (index >= 3) {

        //Log :
        std_out("Error in Triplet::set : invalid index");

        //Fail;
        return;

    }

    //Set the data;
    data[index] = value;

}


/*
 * Full setter : set data to the given triplet;
 */

void Triplet::clone(const Triplet &src) {

    //Copy all data;
    memcpy(data, src.data, 3 * sizeof(float));

}

/*
 * reset : this function will reset the data to zero;
 */

void Triplet::reset() {

    //Fast reset;
    memset(data, 0, 3 * sizeof(float));

}


/*
 * Getter : returns the given value;
 */

float Triplet::get(uint8_t index) const {

    //If the index is superior to 2 :
    if (index >= 3) {

        //Log :
        std_out("Error in Triplet::set : invalid index");

        //Fail;
        return 0;

    }

    //Get the data;
    return data[index];

}


/*
 * get_data : the function return the pointer to the array;
 */

const float *const Triplet::get_data() const {

    //Return the pointer;
    return data;

}

tstring Triplet::toString() {

    return string(data[0], 2) +" "+string(data[1], 2)+" "+string(data[2], 2);

}
