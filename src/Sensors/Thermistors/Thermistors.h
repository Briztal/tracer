
#ifndef TRACER_THERMISTANCES_H
#define TRACER_THERMISTANCES_H


#include <stdint.h>
#include "thermistor_data.h"


class Thermistors {

public :
    static void init();

#define THERMISTOR(i, pin, name)\
public:\
    static float get_temperature_##i(const int16_t read_value);\
    static float get_temperature_##i();\
private:\
    static const uint8_t therm_size_##i;\
    static uint8_t *index_##i;

#include <config.h>


#undef THERMISTOR

private:

    //Get the temperature for a reading, a lookup table and its size, and the previous index read.
    static float get_temperature(const int16_t read_value, const float *const table, const uint8_t size, uint8_t *v_index);

    //Make a linear approximation of the ordinate,  with two given points and an abscissa.
    static float linear_approximation(const int16_t x0, const int16_t x1, const int16_t x,  const float y0, const float y1);

};


#endif //TRACER_THERMISTANCES_H
