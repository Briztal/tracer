//
// Created by root on 4/27/18.
//

#include "Driver.h"

/*
 * Constructor : initialises the number of remaining allocations to its maximum;
 */

Driver::Driver(uint8_t nb_concurrent_allocations) : remainingAllocationsNb(nb_concurrent_allocations) {}


/*
 * attributable : asserts only if the number of remaining allocation is not null;
 */

bool Driver::attributable() {
    return remainingAllocationsNb;
}

/*
 * attribute : decreases the number of time the driver can be allocated. If the driver can't be allocated,
 *  It generates a Driver panic;
 */

void Driver::attribute() {

    //If the number of remaining allocation is zero, Driver panic;
    if (!remainingAllocationsNb) {

        //TODO DRIVER PANIC;
        return;

    }

    //If the driver can be allocated, decrease the remaining number of allocations;
    remainingAllocationsNb--;

}


/*
 * release : releases the driver one time;
 */

void Driver::release() {

    //Increment the number of times the driver can be allocated;
    remainingAllocationsNb++;

}
