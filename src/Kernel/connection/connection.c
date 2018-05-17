//
// Created by root on 5/11/18.
//

#include "connection.h"

#include <Kernel/kernel.h>
#include <data_structures/containers/llist.h>

#include "stdbool.h"

#include "Kernel/scheduler/tasks/service.h"

//----------------------------------- Private functions -----------------------------------


//Process a stream;
void connection_process(connection_t *connection);


//The stream process service;
void stream_service(void *);


//----------------------------------- Connection service data -----------------------------------

//The state of the service;
bool service_started = false;

//The linked list of connections owned by the service; CRITICAL
connection_t *connections = 0;


//------------------------------- Helpers -------------------------------


cnode_t *node_get_first_node(cnode_t *node) {

    //Cache a case for the previous flux;
    cflux_t *flux;

    //While there is a previous flux :
    while ((flux = node_get_previous_flux(node))) {

        //Cache the previous flux's previous node;
        node = flux_get_previous_node(flux);

    }

    //Return the first node, that has no previous flux;
    return node;

}


cnode_t *node_get_last_node(cnode_t *node) {

    //Cache a case for the previous flux;
    cflux_t *flux;

    //While there is a previous flux :
    while ((flux = node_get_next_flux(node))) {

        //Cache the previous flux's previous node;
        node = flux_get_next_node(flux);

    }

    //Return the last node, that has no next flux;
    return node;

}




//------------------------------- Creation -------------------------------

/*
 * connection_create_data_processor : creates a node in the heap from an instance and a destruction function;
 */

cnode_t *connection_create_data_processor(void *instance, void (*destructor)(void *)) {

    //Allocate some memory in the heap for the node;
    cnode_t *node = kernel_malloc(sizeof(cnode_t));

    //Initialise the node;
    *node = {
            .link = EMPTY_LINKED_ELEMENT(),
            .instance = instance,
            .close = destructor,
    };

    //Return the initialised node;
    return node;

}


/*
 * connection_create : creates a heap stored connection from an unique node;
 */

connection_t *connection_create(cnode_t *node) {

    //Allocate some heap memory for the connection;
    connection_t *connection = kernel_malloc(sizeof(connection_t));

    //Initialise the connection;
    *connection =  {
            .link = EMPTY_LINKED_ELEMENT(),
            .data_input = node,
            .spaces_input = node,
    };

    //Return the initialised connection;
    return connection;

}


/*
 * connection_add_node  Add a stream and a node to an existing connection;
 *
 *  The new node takes the place of the space input in the connection struct;
 */

void connection_add_node(connection_t *connection, cflux_t *flux, cnode_t *node) {

    //Cache the last node of the connection;
    cnode_t *spaces_input_node = connection->spaces_input;

    //Concatenate the last node and the flux;
    llist_concat((linked_element_t *) spaces_input_node, (linked_element_t *) flux);

    //Concatenate the flux and the new node;
    llist_concat((linked_element_t *) flux, (linked_element_t *) node);

    //Set the new node as the
    connection->spaces_input = node;

}


//------------------------------- Deletion -------------------------------

/*
 * connection_close : marks a connection to be deleted. Called by anyone;
 */

void connection_close(cflux_t *flux) {

    //First, disable all interrupts;
    kernel_enter_critical_section();//TODO NOT A CRITICAL SECTION !!! USE A MUTEX !!

    //Cache the flux's previous node;
    cnode_t *node = flux_get_previous_node(flux);

    //Get the node node of the list;
    node = node_get_first_node(node);

    //While the node has a next flux to mark deletable :
    while ((flux = node_get_next_flux(node))) {

        //Mark the flux as closed;
        flux->state = CLOSED;

        //Update the node;
        node = flux_get_next_node(flux);

    }

    //Stream will be deleted by the stream service at appropriate time;

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * _connection_close : will effectively close the entire connection. Called by the service;
 */

void _connection_close(connection_t *connection) {

    //Enter a critical section;
    kernel_enter_critical_section(); //TODO NO CRITICAL SECTION, MUTEX + CRITICAL SECTION ON CRITICAL NODE DELETON

    /*
     * This function is exclusively called from the connection stream, same as the flux processor.
     * It can safely delete a connection;
     */

    //Cache the first data processor of the list;
    cnode_t *node = connection->data_input;

    //Cache a space for the next connection flux;
    cflux_t *flux;

    //While there is an output flux :
    while ((flux = node_get_next_flux(node))) {

        //Delete the node;
        node->close(node->instance);

        //Cache the next processor, so that next iteration can safely use it;
        node = flux_get_next_node(flux);

        //Delete the flux;
        kernel_free(flux);

    }

    //Delete the last node;
    node->close(node->instance);

    //Now, we can reset the connection;  Will be fred by the connection service;
    connection->data_input = connection->spaces_input = 0;

    //Leave the critical section;
    kernel_leave_critical_section();

}




//------------------------------- Service -------------------------------

/*
 * connection_process : this function will process the entire connection;
 */

void connection_process(connection_t *connection) {

    //TODO EMPTY STREAMS MUST BE DELETED IN THE SERVICE;

    //Cache the first node of the list;
    cnode_t *node = connection->data_input;

    //Cache a space for the next eventual flux;
    cflux_t *flux;

    //While there is a flux after the current node :
    while ((flux = node_get_next_flux(node))) {

        //If the data flux has to be deleted :
        if (flux->state == CLOSED) {

            //Delete the data flux; It will be made empty, and will be removed during the service's next iteration;
            _connection_close(connection);

            //Stop here;
            return;

        }

        //If the data flux is active, process it;
        data_flux_process(flux);

        //Update the current node as the successor of the current flux;
        node = flux_get_next_node(flux);

    }

}


/*
 * connection_start_service : starts the connection service;
 */

void connection_start_service(uint32_t period_ms) {

    //TODO SERVICE STATE MUTEX;
    //Enter the critical section;
    kernel_enter_critical_section();

    //If the service is already started, do nothing;
    if (service_started)
        return;

    //Mark the service started;
    service_started = true;

    //Leave the critical section;
    kernel_leave_critical_section();

    //Add the service;
    service_add_permanent(stream_service, period_ms, period_ms);

}

/*
 * Register a connection to the processing service;
 */

void connection_transfer_ownership(connection_t *connection) {

    //The access to connections is critical;
    kernel_enter_critical_section();

    //We will insert the provided connection at the beginning of the connection list;

    //If the first place is already occupied :
    if (connections) {

        //Concatenate the existing list to the new connection;
        llist_concat((linked_element_t *) connection, (linked_element_t *) connections);

    }


    //Mark the new connection to be the list beginning;
    connections = connection;

    //Leave the critical section;
    kernel_leave_critical_section();

}

/*
 * connection_stop_service : stop the connection service;
 */

/*
void connection_stop_service() {

    //TODO SERVICE STATE MUTEX;

    //If the service is already started, do nothing;
    if (!service_started)
        return;

    //TODO REMOVAL PATCH;

}
 */


/*
 * connection_delete : remove the connection from the list and return its next element in the chain;
 */

connection_t *connection_delete(connection_t *connection) {

    //The modification of connections is critical;
    kernel_enter_critical_section();

    //Cache the previous and the next element;
    linked_element_t *prev = connection->link.prev;
    linked_element_t *next = connection->link.next;

    //If the previous element is null, we have the first element of the chain;
    if (!prev) {

        //Replace connections with the next element;
        connections = (connection_t *) next;

    } else {

        //As previous is not null, we can lint next to it;
        prev->next = next;

        //If next is not null, we can link next to it;
        if (next) {
            next->prev = prev;
        }

    }

    //Leave the critical section;
    kernel_leave_critical_section();

    //Return the next element (evt 0);
    return (connection_t *) next;

}


/*
 * The stream service.
 *
 *  This function is called by the scheduler only, to process connections;
 *
 *  Its functions are :
 *      - To delete closed connections;
 *      - To process active connections;
 */

void stream_service(void *args) {

    //The access to connections is critical;
    kernel_enter_critical_section();

    //First, cache the sections;
    connection_t *connection = connections;

    //Leave the critical section;
    kernel_leave_critical_section();

    //While the current connection is to process :
    while (connection) {

        //If the connection is null (closed at previous iteration) :
        if (!connection->data_input || !connection->spaces_input) {

            //Remove the connection and cache the next connection to process;
            connection = connection_delete(connection);

        } else {

            //Process the connection;
            connection_process(connection);

            //Update the connection to the next of the current one;
            connection = (connection_t *) connection->link.next;

        }

    }

}
