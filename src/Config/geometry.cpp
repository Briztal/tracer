#ifndef TRACER_GEOMETRY_H
#define TRACER_GEOMETRY_H

#include <stdint.h>
#include <EEPROM/EEPROMStorage.h>


inline void geometry_translate(const float *const hl_coordinates, float *const steppers_coordinates) {

    //0.8 us
    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        steppers_coordinates[axis] = (EEPROMStorage::steppers_data[axis].steps * hl_coordinates[axis]);

    }
}

#endif
