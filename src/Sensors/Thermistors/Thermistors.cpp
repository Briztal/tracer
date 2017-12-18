/*
  Thermistances.cpp - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "Thermistors.h"
#include <hardware_language_abstraction.h>


//------------------------------------------- Init -------------------------------------------

/*
 * initialise_hardware : this function sets all Thermistors Actuators to input mode;
 */

void Thermistors::initialise_hardware() {

#define THERMISTOR(i, name, pin, ...)\
    pin_mode_input(pin);

#include <Config/sensors_config.h>

#undef THERMISTOR

}


//------------------------------------------- Getters -------------------------------------------


#define THERMISTOR(i, name, pin, type)\
/* Variable for the i-th index
 */\
uint8_t tttidx##i = 0;\
uint8_t *Thermistors::index_##i = &tttidx##i;\
\
/*
 * Variable for the i-th size
 */\
const uint8_t Thermistors::therm_size_##i = type##_SIZE;\
\
/*
 * Lookup table for the i-th thermistor
 */\
const float therm_table_##i[type##_SIZE][2] = type##_TABLE;\
\
/*
 * get_temperature_[indice] : gets the temperature corresponding to a particular reading
 *
 *  It calls the function get_temperature with the parameters related to the thermistor [indice]
 *
 */\
float Thermistors::get_temperature_##name(const int16_t read_value) {\
    return get_temperature(read_value, (const float *)therm_table_##i, therm_size_##i, index_##i);\
}\
\
/*
 * get_temperature_[indice] : gets the temperature for Thermistor [indice]'s pin.
 *
 * It calls get_temperature_[indice] with the value read_all on the pin.
 *
 */\
float Thermistors::get_temperature_##name() {\
    int16_t read_value = (int16_t)analog_read(pin);\
    return get_temperature_##name(read_value);\
}\


#include <Config/sensors_config.h>

#undef THERMISTOR


//------------------------------------------- Temperature Processing -------------------------------------------


/*
 * get_temperature : this function determines the temperature of a thermistor, given  :
 *      - a read_data value (most likely given by calling analogRead or equivalent)
 *      - a lookup tabletable for the current thermistor
 *      - the lookup table size
 *      - the current index on the table
 *
 *  The last parameter exists for optimisation purpose. As we don't want to iterate over the whole table each time
 *      a temperature computation is required, we here take advantage of the short variation of the temperature
 *      between two computations :
 *          if the current temperature value is between 5 and 6 in the lookup table, the newt measure is likely to be
 *          between 4 and 7.
 *      So keeping the lower or upper indice is more efficient.
 *
 *  As the lookup table only provides points, a linear approximation is used to get the "real" temperature.
 *
 */

#define GET_ARG_VALUE(index) table[(index) << 1]
#define GET_TEMP(index) table[((index) << 1) + 1]

float
Thermistors::get_temperature(const int16_t read_value, const float *const table, const uint8_t size, uint8_t *v_index) {

    //Cache the lower index, and declare the upper index
    uint8_t lower_index = *v_index, upper_index = lower_index + (uint8_t) 1;

    //Current bounds, temperatures associated with lower and upper index.
    int16_t lower_value = (int16_t) GET_ARG_VALUE(lower_index);
    int16_t upper_value = (int16_t) GET_ARG_VALUE(upper_index);



    float temperature;

    //-------------------------------Bounds Adjusting-------------------------------


    //If the read_value is below the current bounds, decrease
    if (lower_value > read_value) {

        //While the read_data value is lower than the lower bound, shift bounds of one.
        while (lower_value > read_value) {

            //If lower_index is zero, we can't go lower. So, we must return the minimal value.
            if (!lower_index) {
                temperature = GET_TEMP(lower_index);
                goto clean_and_return;
            }

            //Increment indexes
            lower_index--, upper_index--;

            //Update value and lower_value
            upper_value = lower_value;
            lower_value = (int16_t) GET_ARG_VALUE(lower_index);

        }

    } else if (upper_value < read_value) {

        //While the read_data value is higher than the upper bound, shift bounds of one.
        while (upper_value < read_value) {

            if (upper_index == size - 1) {
                temperature = GET_TEMP(upper_index);
                goto clean_and_return;
            }

            //Increment indexes
            lower_index++, upper_index++;

            //Update value and lower_value
            lower_value = upper_value;
            upper_value = (int16_t) GET_ARG_VALUE(upper_index);

        }

    }


    //-------------------------------Temperature computing-------------------------------

    if (read_value == upper_value) {

        //If the read_data value is the upper bound, return the upper temperature
        temperature = GET_TEMP(upper_index);

    } else if (read_value == lower_value) {

        //If the read_data value is the lower bound, return the lower temperature
        temperature = GET_TEMP(lower_index);

    } else {

        //If the read_data value is between the lower bound and the upper bound (strictly), use an approximation

        temperature = linear_approximation(lower_value, upper_value, read_value, GET_TEMP(lower_index),
                                           GET_TEMP(upper_index));

    }

    clean_and_return :


    *v_index = lower_index;
    return temperature;
}


/*
 * linear_approximation : this function makes a linear approximation. given :
 *      P0 (x0, y0) and P1 (x1, y1), a couple of points;
 *      x, the abscissa of the point P to approximate, wo is supposedly on (P0, P1)
 *
 *  this function retrieves y the ordinate of P.
 */

float
Thermistors::linear_approximation(const int16_t x0, const int16_t x1, const int16_t x, const float y0, const float y1) {
    return y0 + ((float) (x - x0) * (y1 - y0) / float(x1 - x0));
}

