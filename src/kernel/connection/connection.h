/*
  connection.h Part of TRACER

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

#ifndef TRACER_CONNECTION_H
#define TRACER_CONNECTION_H

#include "connection_t.h"

#include <kernel/scheduler/semaphore.h>

#include <data_structures/containers/llist.h>


//------------------------------- Creation -------------------------------

//Create a data processor from its base parameters;
cnode_t *connection_create_data_processor(void *instance, void (*destructor)(void *));

//Create a stream from a heap allocated data processor;
connection_t *connection_create(cnode_t *node);

//Add a data processor to an existing stream;
void connection_add_node(connection_t *connection, flux_t *flux, cnode_t *node);


//------------------------------- Deletion -------------------------------

//Marks a stream to be deleted. Will be deleted at next iteration of stream service;
void connection_close(flux_t  *);


//------------------------------- Service -------------------------------

//Start the connection service;
void connection_start_service(uint32_t period_ms);

//Register a connection to the processing service;
void connection_transfer_ownership(connection_t *);

//Stop the connection service;
void connection_stop_service();



#endif //TRACER_CONNECTION_H
