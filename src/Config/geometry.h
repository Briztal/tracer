#ifndef TRACER_GEOMETRY_H
#define TRACER_GEOMETRY_H

#include <stdint.h>
#include <StepperControl/SteppersData.h>


/*
 * geometry_translate : this function defines the geometry of your machine, aka the relation between
 *      the control coordinate system and the stepper coordinate system;
 */

inline void geometry_translate(const float *const hl_coordinates, float *const steppers_coordinates) {

    //0.8 us
    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        steppers_coordinates[axis] = (SteppersData::steppers_data[axis].steps_per_unit * hl_coordinates[axis]);

    }

}

#endif
