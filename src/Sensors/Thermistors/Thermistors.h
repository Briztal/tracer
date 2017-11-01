
#ifndef TRACER_THERMISTANCES_H
#define TRACER_THERMISTANCES_H


#include <stdint.h>
#include "thermistor_data.h"


class Thermistors {

public :
    static void init();

#define THERMISTOR(i, pin, tab, size)\
public:\
    static float get_temperature_##i(const int16_t read_value);\
    static float get_temperature_##i();\
private:\
    static const uint8_t therm_size_##i;\
    static uint8_t *index_##i;

#include "../../config.h"



#undef THERMISTOR

private:

    static float ** therm_table;

    static float get_temperature(const int16_t read_value, const float *const table, const uint8_t size, uint8_t *v_index);

    static float linear_approximation(const int16_t x0, const int16_t x1, const int16_t x,  const float y0, const float y1);

};


#endif //TRACER_THERMISTANCES_H
