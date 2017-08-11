
#ifndef TRACER_THERMISTANCES_H
#define TRACER_THERMISTANCES_H


#include <stdint.h>

class Thermistors {
#include "thermistor_data.h"

#define THERMISTOR(i, tab, size)\
public:\
    static int16_t get_temperature_##i(const int16_t read_value);\
private:\
    static const uint8_t therm_size_##i;\
    static uint8_t *index_##i;

#include "../../config.h"

#undef THERMISTOR

private:

    static int16_t ** therm_table;


    static int16_t get_temperature(const int16_t read_value, const int16_t *const table, const uint8_t size, uint8_t *v_index);

    static int16_t linear_approximation(const int16_t x0, const int16_t x1, const int16_t x,  const int16_t y0, const int16_t y1);

};


#endif //TRACER_THERMISTANCES_H
