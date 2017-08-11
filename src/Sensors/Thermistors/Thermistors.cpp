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

#define THERMISTOR(i, tab, size)\
const int16_t therm_table_##i[size][2] = tab;\
int16_t Thermistors::get_temperature_##i(const int16_t read_value) {\
    return get_temperature(read_value, (const int16_t *)therm_table_##i, therm_size_##i, index_##i);\
}\
uint8_t tttidx##i = 0;\
uint8_t *Thermistors::index_##i = &tttidx##i;\
const uint8_t Thermistors::therm_size_##i = size;

#include "../../config.h"

#undef THERMISTOR

const int16_t arr[2][2] = {{1,2},{3,4}};/* tab;*/\
int16_t ** Thermistors::therm_table = (int16_t **) arr;\

/*
 * get_temperature : this function determines the temperature of a thermistor, given  :
 *      - a read value (most likely given by calling analogRead or equivalent)
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

#define read_t int16_t
#define index_t uint8_t
#define temp_t int16_t

#include "../../interface.h"

int16_t Thermistors::get_temperature(const read_t read_value, const int16_t *const table, const index_t size, index_t *v_index) {
    index_t index = *v_index;


    read_t value, last_value = value = table[index<<1];

    temp_t final_temp, temp, last_temp;

    if (last_value == read_value) {

        //if the current case is the read value
        return table[(index<<1)+1];

    } else if (last_value <read_value) {

        //If the current case is lower than the read value : must iterate from index+1, in increasing order.
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
            value = table[index<<1];

            //if the current case is the read value  2<<3+2
            if (value == read_value) {
                final_temp = table[(index<<1)+1];
                goto clean_and_return;
            }


            //if the current case is higher than the read value
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

        //If the current case is higher than the read value : must iterate from index-1, in decresaing order.
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
            value = table[index<<1];

            //if the current case is the read value
            if (value == read_value) {
                final_temp = table[(index<<1)+1];
                goto clean_and_return;
            }

            //if the current case is higher than the read value
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

int16_t Thermistors::linear_approximation(const int16_t x0, const int16_t x1, const int16_t x, const int16_t y0, const int16_t y1) {
    return y0 + (int16_t) ((float)(x-x0)*(float)(y1-y0)/float(x1-x0));
}

