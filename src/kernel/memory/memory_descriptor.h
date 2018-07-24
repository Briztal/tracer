//
// Created by root on 7/24/18.
//

#ifndef TRACER_MEM_DESC_H
#define TRACER_MEM_DESC_H


#include <stdint.h>

#include <stddef.h>

struct mem_desc_t {

	//The first address of the memory zone;
	volatile void *start_address;

	//The size of a data bloc;
	const size_t bloc_size;

	//The size between two bloc's first address.
	//0 means bloc always at the same address, bloc_spacing < bloc_size means blocs overlap;
	const size_t bloc_spacing;

	//The maximal number of blocs of the memory zone;
	const size_t nb_blocs;

};

//Get the address of a bloc;
volatile void *mem_desc_get_bloc(const struct mem_desc_t *mem_desc, size_t bloc_id);

//Get a pointer to the given offset of a particular bloc;
volatile void *mem_desc_get_bloc_member(const struct mem_desc_t *mem_desc, size_t bloc_id, size_t offset_in_bloc);


#endif //TRACER_MEM_DESC_H
