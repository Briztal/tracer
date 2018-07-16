//
// Created by root on 7/15/18.
//

#include <kernel/kernel.h>
#include <string.h>
#include "rwbuffer.h"


//----------------------------------------------------- Init - exit ----------------------------------------------------

/*
 * rwbuffer_initialise : allocates memory for the buffer itself, and allocates and initialise the rwbuffer;
 */

rwbuffer_t *rwbuffer_initialise(size_t size) {

	//Allocate some space for the buffer;
	void *buffer_min = kernel_malloc(size);

	//Create an initializer;
	rwbuffer_t init = {

		//Save the minimal address of the buffer;
		.buffer_min = buffer_min,

		//Determine and save the maximal address of the buffer;
		.buffer_max = size + (uint8_t *) buffer_min,

		//Save the size;
		.buffer_size = size,

	};

	//Allocate, initialise and return the buffer;
	return kernel_malloc_copy(size, &init);

}


/*
 * rwbuffer_delete : deletes the buffer and the rwbuffer struct;
 */

void rwbuffer_delete(rwbuffer_t *buffer) {

	//Delete the buffer;
	kernel_free(buffer->buffer_min);

	//Delete the rwbuffer struct;
	kernel_free(buffer);

}


//----------------------------------------------------- Manipulation ----------------------------------------------------

/*
 * rwbuffer_insert : if space is available, insert data into the buffer; No size check is made, insertion is always
 * 	done;
 */

void rwbuffer_insert(rwbuffer_t *const buffer, uint8_t *data, size_t size) {

	//Cache the input address in a byte pointer;
	uint8_t *input_address = buffer->input_address;

	//Cache the maximal address. Must not be dereferenced;
	uint8_t *max_address = buffer->buffer_max;

	//Cache the buffer's minimal address;
	uint8_t *min_address = buffer->buffer_min;

	//Cache the size of the buffer;
	const size_t buffer_size = buffer->buffer_size;

	/*
	 * First, we must adjust the copy size to the buffer's limitations. A copy of size superior to the buffer size will
	 * overwrite itself. We will adjust the copy size in [0, buffer_size] and update data in consequence;
	 */

	//While the transfer size is greater than the buffer size :
	while (size > buffer_size) {

		//Decrease the transfer size of the buffer's size;
		size -= buffer_size;

		//Increase the data pointer, to ignore a part of the transfer;
		data += buffer_size;

	}

	//Cache the limit address of the copy (next to the last dereferenced address):
	uint8_t *last_address = input_address + size;

	//If a single contiguous copy would overflow the buffer (input_a + size is the limit that is not dereferenced);
	if (last_address <= max_address) {

		//Execute a single copy;
		memcpy(input_address, data, size);

		//Update the input pointer;
		input_address += size;

		//If we reached the limit address :
		if (input_address == max_address) {

			//Update to the first address;
			input_address = min_address;

		}

		//Save the input address;
		buffer->input_address = input_address;

	} else {
		//If we must make two copies, one until the buffer's end and another starting at the buffer's beginning :

		//Determine the length of the second copy;
		size_t second_size = last_address - max_address;

		//Update the length of the first copy;
		size -= second_size;

		//Execute the first copy;
		memcpy(input_address, data, size);

		//Execute the second copy;
		memcpy(min_address, data + size, second_size);

		//Update the input address;
		buffer->input_address = min_address + second_size;

	}

}


/*
 * If data is available, copy and discard the required amount of data in [dest].
 *
 * 	If there is not enough data available, copy the maximum amount of data;
 *
 * 	Returns the transfer size;
 */

size_t rwbuffer_remove(rwbuffer_t *const buffer, uint8_t *const dest, size_t size) {

	//Cache the output address in a byte pointer;
	const uint8_t *output_address = buffer->output_address;

	//Cache the input address in a byte pointer;
	const uint8_t *const input_address = buffer->input_address;

	//We must determine the maximal transfer size.

	//If both addresses are the same :
	if (output_address == input_address) {

		//The buffer is empty, no transfer;
		return 0;

	}

	//If input is after output :
	if (output_address < input_address) {

		//Transfer can be achieved in a single copy;

		//Cache the maximal transfer size;
		size_t max = input_address - output_address;

		//Minor the size of the transfer;
		if (size > max) size = max;

		//Execute the transfer;
		memcpy(dest, output_address, size);

		//Return the size of the transfer;
		return size;

	} else {

		//If output is after input, available data crosses the buffer's max and min; Two transfers may be required;

		//Cache the maximal address;
		const uint8_t *const buffer_max = buffer->buffer_max;

		//Cache the minimal address;
		const uint8_t *const buffer_min = buffer->buffer_min;


		//Cache the length to the end;
		size_t first_max_size = buffer_max - output_address;

		//If the required transfer won't dereference the buffer's limit :
		if (size <= first_max_size) {

			//Execute the transfer in one time;
			memcpy(dest, output_address, size);

			//Update the input pointer;
			output_address += size;

			//If we reached the limit address :
			if (output_address == buffer_max) {

				//Update to the first address;
				output_address = buffer_min;

			}

			//Save the input address;
			buffer->output_address = output_address;

			//Return the size of the transfer;
			return size;


		} else {

			//If the transfer must be done in two times :

			//Determine the size of the second transfer;
			size_t second_size = size - first_max_size;

			//Determine the maximal size of the second transfer;
			size_t second_max_size = input_address - buffer_min;

			//Minor the second transfer size if required;
			if (second_size > second_max_size) {
				second_size = second_max_size;
			}

			//Execute the first transfer;
			memcpy(dest, output_address, first_max_size);

			//Execute the second transfer;
			memcpy(dest + first_max_size, buffer_min, second_size);

			//Save the new output address;
			buffer->output_address = output_address + second_size;

			//Return the sum of both transfer sizes;
			return first_max_size + second_size;

		}

	}

}