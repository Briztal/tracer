//
// Created by root on 5/11/18.
//

/*
 * The connection library.
 *
 *  Connections are the base for safe data transmission across the code.
 *
 *  When the question of data transmission between a chain of independent nodes (software
 *  or hardware like peripherals) arises, several problems are encountered :
 *
 *  How to (thread)safely propagate information / spaces across the chain, when data can be provided on an interrupt
 *  , and so un-interruptable basis ?
 *
 *  How to ensure that data and spaces propagation functions are regularly executed ?
 *
 *  How to ensure that a certain amount of data provided by a transmitter can effectively be processed by its receiver
 *  (enough space in its internal buffer for example);
 *
 *  How to ensure that, if the connection is closed in one part of the chain (ex : serial close, or error), the
 *  entire the chain is effectively cleaned ?
 *
 *
 *  This library intends to provide a general solution to these issues, by formalising the data connection and its
 *  components; It does not replace data processing, but formalises the data exchange through a transmission chain.
 *  It is a simple formalisation, a direct implementation for a particular transmission chain is totally possible,
 *  but the amount of pitfalls in the correct implementation of a transmission chain made me prefer this abstraction;
 *
 *  A connection is composed of :
 *      - nodes in chain, responsible of processing and / or storing data;
 *      ex : a Serial, will receive and store data internally, a buffer will receive, process and transfer data;
 *      - flux, linking nodes, in charge of transferring data from one data processor to another;
 *      ex : the serial may provide its data to a buffer, the data flux unit will take this in charge;
 *
 *  Connections are created in threads, and can be processed from :
 *      - interrupts, ex : character received in the serial;
 *      - the connection service, a service in charge of processing / cleaning different connections;
 *
 *  A connection is established in a thread, and its ownership is passed to the connection service;
 *
 */

#ifndef TRACER_STREAM_H
#define TRACER_STREAM_H

#include <stddef.h>

#include <Kernel/scheduler/semaphore.h>
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


} connection_node_t;


/*
 * As the connection is shared between its endpoints, and managed by the connection service, we will define a
 *  connection state, that will indicate to the service that the connection must be closed;
 */

typedef enum {

    //The stream is active, it can be executed;
    ACTIVE,

    //The stream has to be deleted, as soon as its mutex is released;
    CLOSED,

} connection_state_t;


/*
 * The data transfer between nodes is realised by connection flux;
 *
 *  Connection flux link two connection nodes, and so possess function pointers related to them;
 */

typedef struct {

    //--------- Linked list composition ---------;

    linked_element_t link;


    //--------- State ---------;

    //The state of the stream;
    connection_state_t state;


    //--------- Data flux function ---------;

    //The size of an element that transmits;
    size_t data_size;

    //Move data; Will take a struct in entry, must be casted;
    void (*flux_function)(connection_flux_t *);


    //--------- Size determination part ---------;

    //A function to get the max amount of data to transfer;
    size_t (*data_amount)(void *tx_struct);

    //A function to get the max amount of spaces to transfer;
    size_t (*spaces_amount)(void *rx_struct);



} connection_flux_t;


/*
 * The connection structure. It summarises the data path, with its two extremities, and eventually adds
 *  some priority data;
 */

typedef struct {

    //--------- Linked list composition ---------;

    linked_element_t link;


    //--------------- Data and spaces input nodes ---------------

    //The stream's first data processor;
    connection_node_t *data_input;

    //The stream's last data processor;
    connection_node_t *spaces_input;


} connection_t;


//------------------------------- Creation -------------------------------

//Create a data processor from its base parameters;
connection_node_t *connection_create_data_processor(void *instance, void (*destructor)(void *));

//Create a stream from a heap allocated data processor;
connection_t *connection_create(connection_node_t *node);

//Add a data processor to an existing stream;
void connection_add_node(connection_t *connection, connection_flux_t *flux, connection_node_t *node);


//------------------------------- Data flux -------------------------------

//Process a data flux;
void data_flux_process(connection_flux_t *);

//Size determination;
size_t data_flux_get_transfer_size(connection_flux_t *);


//------------------------------- Deletion -------------------------------

//Marks a stream to be deleted. Will be deleted at next iteration of stream service;
void connection_close(connection_flux_t *);


//------------------------------- Service -------------------------------

//Start the connection service;
void connection_start_service(uint32_t period_ms);

//Register a connection to the processing service;
void connection_transfer_ownership(connection_t *);

//Stop the connection service;
void connection_stop_service();


#endif //TRACER_STREAM_H
