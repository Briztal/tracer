
#ifndef TRACER_THERMISTANCES_H
#define TRACER_THERMISTANCES_H


#include <stdint.h>
#include "thermistor_data.h"


class Thermistors {


    //------------------------------------------- Init -------------------------------------------

public :

    //Actuators Init
    static void initialise_hardware();

    //Data initialise_data : no static fields or output to reset;
    static void initialise_data() {}


    //------------------------------------------- Getters -------------------------------------------

#define THERMISTOR(i, name, pin, type)\
public:\
    static float get_temperature_##name(const int16_t read_value);\
    static float get_temperature_##name();\
private:\
    static const uint8_t therm_size_##i;\
    static uint8_t *index_##i;

#include <Config/sensors_config.h>

#undef THERMISTOR


    //------------------------------------------- Temperature Processing -------------------------------------------

private:

    //Get the temperature for a reading, a lookup table and its size, and the previous index read_data.
    static float
    get_temperature(const int16_t read_value, const float *const table, const uint8_t size, uint8_t *v_index);

    //Make a linear approximation of the ordinate,  with two given points and an abscissa.
    static float
    linear_approximation(const int16_t x0, const int16_t x1, const int16_t x, const float y0, const float y1);

};


#endif //TRACER_THERMISTANCES_H
