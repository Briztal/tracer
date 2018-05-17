//
// Created by root on 5/17/18.
//

#ifndef TRACER_FLUX_H
#define TRACER_FLUX_H

#include "Kernel/connection/connection.h"

#include <stddef.h>

#include <data_structures/containers/llist.h>


//A type representing a function that copies (from or in) a piece of data;
typedef void (*data_processor)(void *instance, void *dest);

//A type representing a function that copies multiple elements from or in an array of data;
typedef void (*array_processor)(void *instance, void *dest, size_t size);


/*
 * A struct allowing to process data directly:
 *  - get a pointer to processable data;
 *  - process data;
 *  - notify that a certain number of data were processed;
 */
typedef struct {

    //Get a pointer to process data;
    void *(*get_ptr)(void *tx_struct);

    //Discard a certain number of elements;
    void (*data_processed)(void *tx_struct, size_t transfer_size);

} indirect_accessor_t
;


//------------------------------- Connection flux -------------------------------

/*
 * The data transfer between nodes is realised by connection flux;
 *
 *  Connection flux link two connection nodes, and so possess function pointers related to them;
 */

typedef struct {

    //--------- Linked list composition ---------;

    linked_element_t link;


    //--------- Critical or not ---------;

    //Is the flux critical ?
    bool critical;

    //The state of the stream;
    connection_state_t state;


    //--------- Data flux function ---------;

    //The size of an element that transmits;
    size_t data_size;

    //Move data; Will take a struct in entry, must be casted;
    void (*flux_function)(cflux_t *);


    //--------- Size determination part ---------;

    //A function to get the max amount of data to transfer;
    size_t (*data_amount)(void *tx_struct);

    //A function to get the max amount of spaces to transfer;
    size_t (*spaces_amount)(void *rx_struct);

} cflux_t;


//Flux initialisation function;
inline void cflux_initialise(cflux_t *flux, bool critical, size_t size, void (*flux_function)(cflux_t *),
                             size_t (*data_amount)(void *), size_t (*spaces_amount)(void *tx_struct));

//Instance query helper;
cnode_t *flux_get_previous_nodes_instance(cflux_t *flux);

//Instance query helper;
cnode_t *flux_get_next_nodes_instance(cflux_t *flux);

//Process a data flux;
void data_flux_process(cflux_t *);

//Size determination;
size_t data_flux_get_transfer_size(cflux_t *, void *prev_instance, void *next_instance);



//Flux query helper;
inline cflux_t *node_get_previous_flux(cnode_t *node) {

    //Cast and return the previous flux;
    return ((cflux_t *)node->link.prev);

}

//Flux query helper;
inline cflux_t *node_get_next_flux(cnode_t *node) {

    //Cast and return the next flux;
    return ((cflux_t *)node->link.next);

}

//Node query helper;
inline cnode_t *flux_get_previous_node(cflux_t *flux) {

    //Cast and return the previous node;
    return ((cnode_t *)flux->link.prev);

}

//Node query helper;
inline cnode_t *flux_get_next_node(cflux_t *flux) {

    //Cast and return the next node;
    return ((cnode_t *)flux->link.prev);

}

//Instance query helper;
inline cnode_t *flux_get_previous_nodes_instance(cflux_t *flux) {
    return flux_get_previous_node(flux)->instance;
}

//Instance query helper;
inline cnode_t *flux_get_next_nodes_instance(cflux_t *flux) {
    return flux_get_next_node(flux)->instance;
}



/*
 * cflux_initialise : an inline to initialise correctly a flux;
 */

inline void cflux_initialise(cflux_t *flux, bool critical, size_t size, void (*flux_function)(cflux_t *),
                             size_t (*data_amount)(void *), size_t (*spaces_amount)(void *tx_struct)) {

    *flux = {
            .link = EMPTY_LINKED_ELEMENT(),
            .critical = critical,
            .state = ACTIVE,
            .data_size = size,
            .flux_function = flux_function,
            .data_amount = data_amount,
            .spaces_amount = spaces_amount,
    };


}



#endif //TRACER_FLUX_H
