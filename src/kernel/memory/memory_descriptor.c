//
// Created by root on 7/24/18.
//

#include "memory_descriptor.h



/*
 * mem_desc_get_bloc : computes the address of a bloc from its index;
 */

volatile void *mem_desc_get_bloc(const struct mem_desc_t *mem_desc, size_t bloc_id) {

	//If the required peripheral doesn't exist :
	if (bloc_id >= mem_desc->nb_blocs) {

		//Fail.
		return 0;

	}

	//Determine the address of the required peripheral, and add the offset to get the address of the specified register;
	return mem_desc->start_address + (size_t) mem_desc->bloc_spacing * bloc_id;

}


/*
 * mem_desc_get_bloc_member : computes the address of a bloc's member, from the index of the bloc and the member's
 * 	offset in the bloc;
 */

volatile void *mem_desc_get_bloc_member(const struct mem_desc_t *mem_desc, size_t bloc_id, size_t offset_in_bloc) {

	//If the required peripheral doesn't exist :
	if (bloc_id >= mem_desc->nb_blocs) {

		//Fail.
		return 0;

	}

	//If the provided offset is invalid (-1) :
	if (offset_in_bloc == (size_t) -1) {

		//Fail;
		return 0;

	}

	//Determine the address of the required peripheral, and add the offset to get the address of the specified register;
	return mem_desc->start_address + (size_t) mem_desc->bloc_spacing * bloc_id + offset_in_bloc;

}

