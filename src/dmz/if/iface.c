/*
  iface.c Part of TRACER

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

#include "iface.h"

#include <stdmem.h>


//----------------------------------------------- Interfacing functions ------------------------------------------------

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

bool _iface_connect(void *const client, const void *const source, void **const ref, const size_t size) {
	
	//If the reference is not null :
	if (*ref) {
		
		//Double interfacing is not supported;
		return false;
		
	}
	
	//Copy the if structure;
	memcpy(client, source, size);
	
	//Update the reference;
	*ref = client;
	
	//Confirm;
	return true;
	
}


/**
 * iface_disconnect : disconnects the referenced interface, by copying the neutral struct in it;
 * @param ref : the ref to the struct to disconnect;
 * @param neutral_struct : the neutral struct, to copy in to the reference;
 * @param size : the size of the interface struct;
 */

void _iface_disconnect(void **const ref, const void *const neutral_struct, const size_t size) {
	
	//If the reference is not null :
	if (*ref) {
		
		//Neutralise the if;
		memcpy(ref, neutral_struct, size);
		
		//Reset the reference;
		*ref = 0;
		
	}
	
}
