//
// Created by root on 7/24/18.
//

#include <string.h>
#include <kernel/kernel.h>

#include "memory_transfer.h"




//-------------------------------------------------- Bloc descriptors --------------------------------------------------

/*
 * block_desc_merge : merge the couple of bloc descriptors;
 */

bool block_desc_merge(const struct blocks_desc_t *in0, const struct blocks_desc_t *in1,struct blocks_desc_t *out) {

	//If both memory zones do not have the same block size :
	if (in0->block_size != in1->block_size) {

		//Reset the output block size;
		out->block_size = 0;

		//Do nothing more, the transfer cannot be done;
		return false;

	}

	//Cache both number of blocs;
	size_t nb0 = in0->nb_blocks, nb1 = in1->nb_blocks;

	//Determine the minimal number of bloc, save it and assert if it is not null;
	return (bool) (out->nb_blocks = (nb0 < nb1) ? nb0 : nb1);

}


//------------------------------------------------ Memory map transfers ------------------------------------------------


/*
 * mmap_transfer_8 : this function copies the required amount of blocks from the src map to the dst map, one byte at the
 * 	time;
 *
 * 	The size of a bloc is always expressed in bytes;
 *
 * 	All data in both maps are used, reason why they are passed by value;
 */

void mmap_transfer_8(struct mem_map_t src, struct mem_map_t dst, const size_t blocks_size, size_t nb_blocks) {

	//For each block to read :
	while(nb_blocks--) {

		//Copy contiguous regions;
		memcpy((void *) dst.start_address, (const void *) src.start_address, blocks_size);

		//Update the write address;
		dst.start_address += dst.block_spacing;

		//Update the read address;
		src.start_address += src.block_spacing;

	}

}


/*
 * mmap_transfer_16 : this function copies the required amount of blocks from the src map to the dst map, two bytes at
 * 	the time;
 *
 * 	The size of a bloc is always expressed in bytes, it will be truncated at the beginning of the function;
 *
 * 	All data in both maps are used, reason why they are passed by value;
 */

void mmap_transfer_16(struct mem_map_t src, struct mem_map_t dst, size_t block_size, size_t nb_blocks) {

	//Check size;
	if (block_size & 1) {

		//Error, incompatible number of blocs to transfer;
		kernel_error("memory_transfer.c : mmap_transfer_16 : incompatible size;");

	}

	//If addresses are misaligned for a 2 byte transfer :
	if (((size_t)src.start_address & 1) || ((size_t) dst.start_address & 1)) {

		//Error, transfer will be misaligned;
		kernel_error("memory_transfer.c : mmap_transfer_16 : misaligned pointer;");


	}

	//First, truncate the block size;
	block_size >>= 1;

	//For each block to read :
	while(nb_blocks--) {

		//Cache the size of a bloc;
		size_t bs = block_size;

		//Cache the current src addr;
		volatile uint16_t *src_addr = src.start_address, *dst_addr = dst.start_address;

		//While there are uint16_t to copy
		while (bs--) {

			//Copy the current uint16_t and update pointers;
			*(dst_addr++) = *(src_addr++);

		}

		//Update the write address;
		dst.start_address += dst.block_spacing;

		//Update the read address;
		src.start_address += src.block_spacing;

	}

}


/*
 * mmap_transfer_16 : this function copies the required amount of blocks from the src map to the dst map, four bytes at
 * 	the time;
 *
 * 	The size of a bloc is always expressed in bytes, it will be truncated at the beginning of the function;
 *
 * 	All data in both maps are used, reason why they are passed by value;
 */

void mmap_transfer_32(struct mem_map_t src, struct mem_map_t dst, size_t block_size, size_t nb_blocks) {

	//Check size;
	if (block_size & 3) {

		//Error, incompatible number of blocs to transfer;
		kernel_error("memory_transfer.c : mmap_transfer_32 : incompatible size;");

	}

	//If addresses are misaligned for a 2 byte transfer :
	if (((size_t)src.start_address & 3) || ((size_t) dst.start_address & 3)) {

		//Error, transfer will be misaligned;
		kernel_error("memory_transfer.c : mmap_transfer_32 : misaligned pointer;");


	}

	//First, truncate the block size;
	block_size >>= 2;

	//For each block to read :
	while(nb_blocks--) {

		//Cache the size of a bloc;
		size_t bs = block_size;

		//Cache the current src addr;
		volatile uint32_t *src_addr = src.start_address, *dst_addr = dst.start_address;

		//While there are uint16_t to copy
		while (bs--) {

			//Copy the current uint16_t and update pointers;
			*(dst_addr++) = *(src_addr++);

		}

		//Update the write address;
		dst.start_address += dst.block_spacing;

		//Update the read address;
		src.start_address += src.block_spacing;

	}

}



//------------------------------------------------- Memory descriptors -------------------------------------------------

/*
 * mem_desc_get_bloc : computes the address of a block from its index;
 */

volatile void *mem_desc_get_bloc(const struct mem_desc_t *mem_desc, size_t bloc_id) {

	//If the required peripheral doesn't exist :
	if (bloc_id >= mem_desc->bloc_desc.nb_blocks) {

		//Fail.
		return 0;

	}

	//Determine the address of the required peripheral, and add the offset to get the address of the specified register;
	return
		(volatile uint8_t *)mem_desc->memory_map.start_address + (size_t) mem_desc->memory_map.block_spacing * bloc_id;

}


/*
 * mem_desc_get_bloc_member : computes the address of a block's member, from the index of the block and the member's
 * 	offset in the block;
 */

volatile void *mem_desc_get_bloc_member(const struct mem_desc_t *mem_desc, size_t bloc_id, size_t offset_in_bloc) {

	//If the required peripheral doesn't exist :
	if (bloc_id >= mem_desc->bloc_desc.nb_blocks) {

		//Fail.
		return 0;

	}

	//If the provided offset is invalid (-1) :
	if (offset_in_bloc == (size_t) -1) {

		//Fail;
		return 0;

	}

	//Determine the address of the required peripheral, and add the offset to get the address of the specified register;
	return
		(volatile uint8_t *) mem_desc->memory_map.start_address +
		(size_t) mem_desc->memory_map.block_spacing * bloc_id +
		offset_in_bloc;

}


/*
 * mem_desc_transfer : Transfer data from a memory descriptor zone to another.
 *
 * 	Returns the number of transferred blocks;
 */

size_t mem_desc_transfer(const struct mem_desc_t *src, const struct mem_desc_t *dst) {

	//Create the out bloc descriptor;
	struct blocks_desc_t out_descriptor;

	//Merge the couple of bloc descriptors; Will assert if a transfer can happen;
	bool transferable = block_desc_merge(&src->bloc_desc, &dst->bloc_desc, &out_descriptor);

	//If no transfer can happen, nothing to do;
	if (!transferable) {
		return 0;
	}

	//Execute the transfer; For instance only 8 bytes transfer is supported;
	mmap_transfer_8(src->memory_map, dst->memory_map, out_descriptor.block_size, out_descriptor.nb_blocks);

	//Determine the number copied bytes;
	return out_descriptor.nb_blocks;

}










