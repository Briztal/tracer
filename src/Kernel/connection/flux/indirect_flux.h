//
// Created by root on 5/12/18.
//

#ifndef TRACER_TRANSFERT_STREAM_H
#define TRACER_TRANSFERT_STREAM_H

#include "Kernel/connection/connection.h"

typedef struct {

    //--------- Composition ---------

    //The flux structure;
    cflux_t flux;


    //--------- Data flux functions ---------

    //A boolean to determine whether rx or tx is passive;
    bool tx_passive;

    //The passive side, provides a pointer to its data and receives process notifications;
    indirect_accessor_t indirect_accessor;

    //The active side, accepts the pointer and processed data;
    array_processor copier;


} indirect_flux_t;



//Construct an indirect flux where the receiver is passive;
indirect_flux_t *indirect_flux_create_tx_passive(bool critical, size_t element_size,
                                                size_t (*data_amount)(void *),
                                                size_t (*spaces_amount)(void *),
                                                indirect_accessor_t tx_accessor,
                                                array_processor rx_copier);

//Construct an indirect flux where the receiver is passive;
indirect_flux_t *indirect_flux_create_rx_passive(bool critical, size_t element_size,
                                                size_t (*data_amount)(void *),
                                                size_t (*spaces_amount)(void *),
                                                array_processor tx_copier,
                                                indirect_accessor_t rx_accessor);


#endif //TRACER_TRANSFERT_STREAM_H
