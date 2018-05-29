//
// Created by root on 5/18/18.
//

#ifndef TRACER_CONNECTION_DATA_H
#define TRACER_CONNECTION_DATA_H

#include <stdbool.h>

#include <stddef.h>

#include <data_structures/containers/llist.h>


/*
 * The connection node structure.
 *
 *  A connection node is a data structure on which we possess a pointer (the instance), and a close function,
 *  responsible for cleaning memory up when the connection is closed;
 */

typedef struct {

    //--------- Linked list composition ---------;

    linked_element_t link;

    //--------- Data ---------;

    //The instance pointer;
    void *instance;

    //The instance's close;
    void (*close)(void *);


} cnode_t;


/*
 * As the connection is shared between its endpoints, and managed by the connection service, we will define a
 *  connection sequences_initialised, that will indicate to the service that the connection must be closed;
 */

typedef enum flux_state_t{

    //The flux is active, it can be executed;
            ACTIVE_FLUX,

    //The stream has to be deleted, as soon as its mutex is released;
            CLOSED_FLUX,

} flux_state_t;


//------------------------------- Connection -------------------------------

/*
 * The connection structure. It summarises the data path, with its two extremities, and eventually adds
 *  some priority data;
 */

typedef struct {

    //--------- Linked list composition ---------;

    linked_element_t link;


    //--------------- Data and spaces input nodes ---------------

    //The stream's first data processor;
    cnode_t *data_input;

    //The stream's last data processor;
    cnode_t *spaces_input;


} connection_t;



//A type representing a function that copies (from or in) a piece of data;
typedef void (*data_processor)(const void *instance, void *dest);

//A type representing a function that copies multiple elements from or in an array of data;
typedef void (*array_processor)(const void *instance, void *dest, const size_t size);


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

} indirect_accessor_t;


//------------------------------- Connection cflux -------------------------------

/*
 * The data transfer between nodes is realised by connection flux;
 *
 *  Connection flux link two connection nodes, and so possess function pointers related to them;
 */

typedef struct flux_t{

    //--------- Linked list composition ---------;

    linked_element_t link;


    //--------- Critical or not ---------;

    //Is the cflux critical ?
    bool critical;

    //The sequences_initialised of the stream;
    flux_state_t state;


    //--------- Data cflux function ---------;

    //The size of an element that transmits;
    size_t data_size;

    //Move data; Will take a struct in entry, must be casted;
    void (*flux_function)(struct flux_t *);


    //--------- Size determination part ---------;

    //A function to get the max amount of data to transfer;
    size_t (*data_amount)(void *tx_struct);

    //A function to get the max amount of spaces to transfer;
    size_t (*spaces_amount)(void *rx_struct);

} flux_t;


#endif //TRACER_CONNECTION_DATA_H
