/*
  indirect_flux.h Part of TRACER

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

#ifndef TRACER_TRANSFERT_STREAM_H
#define TRACER_TRANSFERT_STREAM_H

#include "kernel/connection/connection.h"

typedef struct {

    //--------- Composition ---------

    //The cflux structure;
    flux_t flux;


    //--------- Data cflux functions ---------

    //A boolean to determine whether rx or tx is passive;
    bool tx_passive;

    //The passive side, provides a pointer to its data and receives process notifications;
    indirect_accessor_t indirect_accessor;

    //The active side, accepts the pointer and processed data;
    array_processor copier;


} indirect_flux_t;



//Construct an indirect cflux where the receiver is passive;
indirect_flux_t *indirect_flux_create_tx_passive(bool critical, size_t element_size,
                                                size_t (*data_amount)(void *),
                                                size_t (*spaces_amount)(void *),
                                                indirect_accessor_t tx_accessor,
                                                array_processor rx_copier);

//Construct an indirect cflux where the receiver is passive;
indirect_flux_t *indirect_flux_create_rx_passive(bool critical, size_t element_size,
                                                size_t (*data_amount)(void *),
                                                size_t (*spaces_amount)(void *),
                                                array_processor tx_copier,
                                                indirect_accessor_t rx_accessor);


#endif //TRACER_TRANSFERT_STREAM_H
