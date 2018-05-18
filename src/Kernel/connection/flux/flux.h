//
// Created by root on 5/17/18.
//

#ifndef TRACER_FLUX_H
#define TRACER_FLUX_H

#include <stddef.h>

#include <Kernel/connection/connection.h>

#include <data_structures/containers/llist.h>

s

//Process a data cflux;
void flux_process(flux_t *);

//Size determination;
size_t data_flux_get_transfer_size(flux_t *, void *prev_instance, void *next_instance);



//Flux query helper;
static inline flux_t *node_get_previous_flux(cnode_t *node) {

    //Cast and return the previous cflux;
    return ((flux_t *)node->link.prev);

}

//Flux query helper;
static inline flux_t *node_get_next_flux(cnode_t *node) {

    //Cast and return the next cflux;
    return ((flux_t *)node->link.next);

}

//Node query helper;
static inline cnode_t *flux_get_previous_node(flux_t *flux) {

    //Cast and return the previous node;
    return ((cnode_t *)flux->link.prev);

}

//Node query helper;
static inline cnode_t *flux_get_next_node(flux_t *flux) {

    //Cast and return the next node;
    return ((cnode_t *)flux->link.prev);

}

//Instance query helper;
static inline cnode_t *flux_get_previous_nodes_instance(flux_t *flux) {
    return flux_get_previous_node(flux)->instance;
}

//Instance query helper;
static inline cnode_t *flux_get_next_nodes_instance(flux_t *flux) {
    return flux_get_next_node(flux)->instance;
}



/*
 * cflux_initialise : an inline to initialise correctly a cflux;
 */

static inline void cflux_initialise(flux_t *flux, bool critical, size_t size, void (*flux_function)(flux_t *),
                             size_t (*data_amount)(void *), size_t (*spaces_amount)(void *tx_struct)) {

    *flux = (flux_t) {
            .link = EMPTY_LINKED_ELEMENT(),
            .critical = critical,
            .state = ACTIVE_FLUX,
            .data_size = size,
            .flux_function = (void (*)(struct flux_t *)) flux_function,
            .data_amount = data_amount,
            .spaces_amount = spaces_amount,
    };


}



#endif //TRACER_FLUX_H
