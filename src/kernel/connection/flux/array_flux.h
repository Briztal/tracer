/*
  array_flux.h Part of TRACER

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

#ifndef TRACER_COPY_STREAM_H
#define TRACER_COPY_STREAM_H

#include "flux.h"

/*
 * The copy stream is a stream composition, that copies data it transmits.
 *
 *  Its data cflux function determines the amount of data to transit, declares an array of the appropriate size, and
 *  passes it to the transmitter, that copies its data in it.
 *
 *  Then, it passes the array to the receiver that processes it;
 */

typedef struct {

    //--------- Composition ---------

    //The stream structure;
    flux_t flux;


    //--------- Flux functions ---------

    //The function for the transmitter to copy its data;
    array_processor tx_copier;

    //The function for the transmitter to receive its data;
    array_processor rx_copier;

} array_flux_t;


//Create an array fux in the heap;
array_flux_t *array_flux_create(bool critical, size_t element_size,
                                size_t (*data_amount)(void *),
                                size_t (*spaces_amount)(void *),
                                array_processor tx_copier,
                                array_processor rx_copier);


#endif //TRACER_COPY_STREAM_H
