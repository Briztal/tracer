
#ifndef TRACER_EEPROM_STORAGE_DATA_H
#define TRACER_EEPROM_STORAGE_DATA_H


#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

typedef struct stepper_data {

    float size;
    float steps;
    float maximum_speed;
    float acceleration;
    float jerk;

} ;

#endif


#endif //TRACER_EEPROM_STORAGE_DATA_H
