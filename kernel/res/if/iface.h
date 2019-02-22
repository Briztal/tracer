/*
  iface.h Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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


#ifndef TRACER_IFACE_H
#define TRACER_IFACE_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


/*----------------------------------------------- Interfacing functions ------------------------------------------------*/

/**
 * iface_connect : connects the client interface to the source interface, and update the reference.
 * 	An appropriate size must be provided
 *
 * @param client : the client interface, to initialise;
 * @param source : the source interface to copy;
 * @param ref : the ref cache of the interface. If not zero, interface will not occur;
 * @param size : the size of all interface structs;
 * @return true if interfacing occurred, false if not;
 */

bool _iface_connect(void *client, const void *source, void **ref, size_t size);


/**
 * iface_disconnect : disconnects the referenced interface, by copying the neutral struct in it;
 * @param ref : the ref to the struct to disconnect;
 * @param neutral_struct : the neutral struct, to copy in to the reference;
 * @param size : the size of the interface struct;
 */

void _iface_disconnect(void **ref, const void *neutral_struct, size_t size);


/*----------------------------------------------------- Shortcuts ------------------------------------------------------*/

/*Passing the size can be source of errors. Both macros determine the size automatically;*/

#define iface_connect(client, source, ref) _iface_connect(client, source, (void **)(ref), sizeof(**(ref)))

#define iface_disconnect(ref, neutral) _iface_disconnect((void **)(ref), (const void *)(neutral), sizeof(**(ref)))


#endif /*TRACER_IFACE_H*/
