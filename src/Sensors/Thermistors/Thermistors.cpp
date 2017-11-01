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
#include <interface.h>

void Thermistors::init() {

#define THERMISTOR(i, pin, ...)\
    pin_mode_input(pin);

#include <config.h>

#undef THERMISTOR

}

#define THERMISTOR(i, pin, name)\
/* Variable for the i-th index
 */\
uint8_t tttidx##i = 0;\
uint8_t *Thermistors::index_##i = &tttidx##i;\
\
/*
 * Variable for the i-th size
 */\
const uint8_t Thermistors::therm_size_##i = name##_SIZE;\
\
/*
 * Lookup table for the i-th thermistor
 */\
const float therm_table_##i[name##_SIZE][2] = name##_TABLE;\
\
/*
 * get_temperature_[indice] : gets the temperature corresponding to a particular reading
 *
 *  It calls the function get_temperature with the parameters related to the thermistor [indice]
 *
 */\
float Thermistors::get_temperature_##i(const int16_t read_value) {\
    return get_temperature(read_value, (const float *)therm_table_##i, therm_size_##i, index_##i);\
}\
\
/*
 * get_temperature_[indice] : gets the temperature for Thermistor [indice]'s pin.
 *
 * It calls get_temperature_[indice] with the value read on the pin.
 *
 */\
float Thermistors::get_temperature_##i() {\
    int16_t read_value = (int16_t)analog_read(pin);\
    return get_temperature_##i(read_value);\
}\

#include <config.h>

#undef THERMISTOR


/*
 * get_temperature : this function determines the temperature of a thermistor, given  :
 *      - a read_output value (most likely given by calling analogRead or equivalent)
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


float Thermistors::get_temperature(const int16_t read_value, const float *const table, const uint8_t size, uint8_t *v_index) {
    uint8_t index = *v_index;

    int16_t value, last_value = value = (int16_t) table[index << 1];

    float final_temp;

    if (last_value == read_value) {

        //if the current case is the read_output value
        return table[(index<<1)+1];

    } else if (last_value <read_value) {

        //If the current case is lower than the read_output value : must iterate from index+1, in increasing order.
        while(true) {
            //Increment the index
            index++;

            //If top reached : return the maximum value
            if (index==size) {
                index--;
                final_temp = table[((size-1)<<1)+1];
                goto clean_and_return;
            }

            //Update value and last_value
            last_value = value;
            value = (int16_t) table[index << 1];

            //if the current case is the read_output value  2<<3+2
            if (value == read_value) {
                final_temp = table[(index<<1)+1];
                goto clean_and_return;
            }


            //if the current case is higher than the read_output value
            if (value > read_value) {

                //Not a value in the lookup table -> use an approximation
                final_temp = linear_approximation(last_value, value, read_value, table[((index-1)<<1)+1], table[(index<<1)+1]);

                //Memorise the lower value
                index--;
                goto clean_and_return;
            }

        }
    } else {


        if (!index) {
            final_temp = table[1];
            goto clean_and_return;
        }

        //If the current case is higher than the read_output value : must iterate from index-1, in decresaing order.
        while(true) {
            //Increment the index
            index--;

            //If top reached : return the maximum value
            if (!index) {
                final_temp = table[1];
                goto clean_and_return;
            }

            //Update value and last_value
            last_value = value;
            value = (int16_t) table[index << 1];

            //if the current case is the read_output value
            if (value == read_value) {
                final_temp = table[(index<<1)+1];
                goto clean_and_return;
            }

            //if the current case is higher than the read_output value
            if (value < read_value) {

                //Not a value in the lookup table -> use an approximation
                final_temp = linear_approximation(last_value, value, read_value, table[((index-1)<<1)+1], table[(index<<1)+1]);
                goto clean_and_return;
            }

        }
    }

    clean_and_return :

    *v_index = index;
    return final_temp;
}


/*
 * linear_approximation : this function makes a linear approximation. given :
 *      P0 (x0, y0) and P1 (x1, y1), a couple of points;
 *      x, the abscissa of the point P to approximate, wo is supposedly on (P0, P1)
 *
 *  this function retrieves y the ordinate of P.
 */

float Thermistors::linear_approximation(const int16_t x0, const int16_t x1, const int16_t x, const float y0, const float y1) {
    return y0 + ((float)(x-x0)*(y1-y0)/float(x1-x0));
}

