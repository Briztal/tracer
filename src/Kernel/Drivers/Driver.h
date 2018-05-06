//
// Created by root on 4/27/18.
//

#ifndef TRACER_DRIVER_H
#define TRACER_DRIVER_H

#include "stdint.h"

/*
 * This class is the base abstract class for a driver;
 *
 *  It just contains the methods used by the resource manager to attribute and restore the driver;
 */

class Driver {

    /*
     * Methods require and release are friends of the driver class;
     */

    /*
    friend Driver *PeripheralManager::require(driver_type_t);

    friend void PeripheralManager::release(Driver *driver);
*/

    //--------------------------------- Object oriented methods ---------------------------------

protected:

    //The constructor is protected, so that only Driver implementations can access it;
    explicit Driver(uint8_t nb_concurrent_allocations);

private:

    //The Copy constructor is disabled;
    Driver(const Driver &) : remainingAllocationsNb(0) {};

    //The Move constructor is disabled;
    Driver(Driver &&) noexcept : remainingAllocationsNb(0) {};

    //The Copy assignment operator is disabled;
    Driver &operator=(const Driver &) {return *this;};

    //The Move assignment operator is disabled;
    Driver &operator=(Driver &&) noexcept {return *this;};


    //--------------------------------- Allocation methods ---------------------------------

private:

    //Is the driver attributable ?
    bool attributable();

    //Attribute the driver to a process_t;
    void attribute();

    //Restituate the driver to the ressource manager;
    void release();


    //--------------------------------- Fields ---------------------------------

private:

    //The number of times this driver can be allocated;
    uint8_t remainingAllocationsNb;

};


#endif //TRACER_DRIVER_H
