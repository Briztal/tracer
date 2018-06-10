/*
  data_flux.h Part of TRACER

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

#include "kernel/connection/connection.h"

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
    data_processor tx_copier;

    //The function for the receiver to receive its data;
    data_processor rx_copier;


} data_flux_t;


//Create an array fux in the heap;
data_flux_t *data_flux_create(bool critical, size_t element_size,
                                                size_t (*data_amount)(void *),
                                                size_t (*spaces_amount)(void *),
                                                data_processor tx_copy,
                                                data_processor rx_receive);



#endif //TRACER_COPY_STREAM_H
