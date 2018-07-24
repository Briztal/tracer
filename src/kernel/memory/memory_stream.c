//
// Created by root on 7/17/18.
//

#include "memory_stream.h"

#include <string.h>

//----------------------------------------------------- Init - Exit ----------------------------------------------------

/*
 * stream_read : reads blocs from the stream memory; Returns the size of the read zone, zero if bad direction;
 */

size_t stream_read(struct stream_memory_t *const stream, const size_t nb_blocs, void *const dst,
				   const size_t dst_size) {

	//If the stream is an input stream :
	if (stream->input_stream) {

		//Error, wrong direction. Do nothing;
		return 0;

	}

	//TODO CHECK FOR DST OVERFLOW, DST_SIZE IS NOT USED

	//Determine how many blocs are available;
	size_t blocs_to_read = (*(stream->available_blocs))(stream);

	//Major the number of blocs to read;
	if (blocs_to_read > nb_blocs) {
		blocs_to_read = nb_blocs;
	}

	//Cache size and spacings of blocs;
	const size_t bloc_size = stream->bloc_size, bloc_spacing = stream->bloc_spacing;

	//Save a copy of the number of blocs read;
	const size_t read_blocs = blocs_to_read;

	//Cache the reading address;
	const volatile uint8_t *read_address = stream->start_address;

	//Cache the writing address;
	uint8_t *write_address = dst;

	//For each bloc to read :
	while(blocs_to_read--) {

		//If we are about to read the last bloc :
		if (!blocs_to_read) {

			//Read the clear register;
			*stream->clear_register;

		}

		//Copy contiguous regions;
		memcpy(write_address, (const void *) read_address, bloc_size);

		//Update the write address;
		write_address += bloc_size;

		//Update the read address;
		read_address += bloc_spacing;

	}

	//Discard all read blocs;
	(*(stream->discard_blocs))(stream, read_blocs);


	//Determine the number copied bytes;
	return read_blocs * bloc_size;

}



/*
 * stream_read : copies blocs to the stream memory; Returns the size of the copied zone, zero if bad direction:
 */

size_t stream_write(struct stream_memory_t *const stream,  const size_t nb_blocs, const void *const src,
					const size_t dst_size) {

	//If the stream is an input stream :
	if (stream->input_stream) {

		//Error, wrong direction. Do nothing;
		return 0;

	}

	//TODO CHECK FOR DST OVERFLOW, DST_SIZE IS NOT USED

	//Determine how many blocs are available;
	size_t blocs_to_read = (*(stream->available_blocs))(stream);

	//Major the number of blocs to read;
	if (blocs_to_read > nb_blocs) {
		blocs_to_read = nb_blocs;
	}

	//Cache size and spacings of blocs;
	const size_t bloc_size = stream->bloc_size, bloc_spacing = stream->bloc_spacing;

	//Save a copy of the number of blocs read;
	const size_t read_blocs = blocs_to_read;

	//Cache the reading address;
	const uint8_t *read_address = src;

	//Cache the writing address;
	volatile uint8_t *write_address = stream->start_address;

	//For each bloc to read :
	while(blocs_to_read--) {

		//If we are about to read the last bloc :
		if (!blocs_to_read) {

			//Read the clear register;
			*stream->clear_register;

		}
		//Copy contiguous regions;
		memcpy((void *) write_address, read_address, bloc_size);

		//Update the write address;
		write_address += bloc_spacing;

		//Update the read address;
		read_address += bloc_size;

	}

	//Discard all read blocs;
	(*(stream->discard_blocs))(stream, read_blocs);


	//Determine the number copied bytes;
	return read_blocs * bloc_size;


}
