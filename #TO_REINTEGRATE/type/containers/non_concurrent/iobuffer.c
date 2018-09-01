//
// Created by root on 7/15/18.
//

#include "iobuffer.h"

#include <string.h>

#include <kernel/syscall.h>


//--------------------------------------------------- Private headers --------------------------------------------------

//Delete a created buffer;
void iobuffer_delete(iobuffer_t *buffer);

//Persist an occurred insertion transfer;
void iobuffer_persist_insertion(iobuffer_t *iobuffer, size_t transfer_size);

//Persist an occurred deletion transfer;
void iobuffer_persist_deletion(iobuffer_t *iobuffer, size_t transfer_size);


//----------------------------------------------------- Init - prempt ----------------------------------------------------

/*
 * iobuffer_create : creates an IOBuffer from a couple of channels, and registers itself to active ones;
 */

void iobuffer_create(const size_t size, iochannel_t *const input_channel,
					 iochannel_t *const output_channel) {

	//If both channels are null, do nothing;
	if ((!input_channel) || (!output_channel)) {

		//Error;
		kernel_error("iobuffer.c : iobuffer_create : access channels must bot be null;");

	}

	//If both channels are equal :
	if (output_channel == input_channel) {

		//Error;
		kernel_error("iobuffer.c : iobuffer_create : access channels must not be equal;");

	}

	//Allocate some space for the buffer;
	void *const buffer_min = kernel_malloc(size);

	//Create an initializer;
	const iobuffer_t init = {

		//Save the minimal address of the buffer;
		.buffer_min = buffer_min,

		//Determine and save the maximal address of the buffer;
		.buffer_max = size + (uint8_t *) buffer_min,

		//Save the input channel;
		.input_channel = input_channel,

		//Save the output channel;
		.output_channel = output_channel,

	};

	//Create and allocate data
	iobuffer_t *const iobuffer = kernel_malloc_copy(sizeof(iobuffer_t), &init);


	//Registration is a critical section, as a section could delete the buffer as it is not completely registered;
	kernel_enter_critical_section();

	//Register the IOBuffer and the input channel;
	iobuffer_channel_register_buffer(input_channel, iobuffer);

	//Register the IOBuffer and the output channel;
	iobuffer_channel_register_buffer(output_channel, iobuffer);

	//Initialise the output transfer data;
	output_channel->transfer_size = 0;
	output_channel->data_address = buffer_min;

	//Initialise the input transfer data;
	output_channel->transfer_size = size;
	output_channel->data_address = buffer_min;

	//Enable both channels;
	iochannel_update(iobuffer->input_channel);
	iochannel_update(iobuffer->output_channel);

	//Leave the critical section;
	kernel_leave_critical_section();

}


/*
 * iobuffer_delete : deletes an existing buffer, after its two channels have unregistered themselves;
 */

void iobuffer_delete(iobuffer_t *const buffer) {

	//Delete the buffer;
	kernel_free(buffer->buffer_min);

	//Delete the iobuffer struct;
	kernel_free(buffer);

}


//------------------------------------------------ Channel Modification ------------------------------------------------

/*
 * iobuffer_channel_delete : if the channel is registered to an iobuffer, un-registers it and the other one
 * 	of the iobuffer, and deletes the iobuffer;
 */

void iobuffer_channel_delete(iochannel_t *const channel) {

	//Channel deletion is critical, as two channels can't be unregistered at the same time;
	kernel_enter_critical_section();

	//If the channel is null :
	if (!channel) {

		//Error, null pointer provided;
		kernel_error("iobuffer.c : iobuffer_unregister_channel : null pointer provided;");

		//To avoid warnings, never reached;
		return;

	}

	//Cache the channel's buffer :
	iobuffer_t *iobuffer = channel->buffer;

	//If the channel is registered to an iobuffer :
	if (iobuffer) {

		//Reset both channels; They will be marked unregistered;
		iochannel_reset(iobuffer->input_channel);
		iochannel_reset(iobuffer->output_channel);

		//Mark both channels null;
		iobuffer->input_channel = 0;
		iobuffer->output_channel = 0;

		//Delete the buffer;
		iobuffer_delete(iobuffer);

	}

	//Delete the channel;
	(*(channel->destructor))(channel);

	//Leave the critical section;
	kernel_leave_critical_section();

}


//------------------------------------------------ Transfer persistence ------------------------------------------------

/*
 * persist_transfer : notify the IOBuffer that one of its channel has completed a transfer,
 * whose size is stored in the channel pointer;
 */

void iobuffer_persist_transfer(iochannel_t *channel) {

	//Channel persistence is critical, as two channels can't call it at the same time;
	kernel_enter_critical_section();

	//If the channel is null :
	if (!channel) {

		//Error, null pointer provided;
		kernel_error("iobuffer.c : persist_transfer : null pointer provided;");

		//To avoid warnings, never reached;
		return;

	}

	//Cache the channel's buffer :
	iobuffer_t *iobuffer = channel->buffer;

	//If the channel is registered :
	if (iobuffer) {

		//Cache the effective transfer size;
		size_t transfer_size = channel->transfer_size;

		//If the provided channel is the input one :
		if (channel == iobuffer->input_channel) {

			//Persist an insertion and update both channels accesses;
			iobuffer_persist_insertion(iobuffer, transfer_size);

		} else {
			//If the provided channel is the output one :

			//Persist a deletion and update the output channel's access;
			iobuffer_persist_deletion(iobuffer, transfer_size);

		}

		//Update both channels;
		iochannel_update(iobuffer->input_channel);
		iochannel_update(iobuffer->output_channel);

	}



	//Leave the critical section;
	kernel_leave_critical_section();

}


/*
 * iobuffer_persist_insertion : mark that an insertion has been made, an update is required for both channels;
 */

void iobuffer_persist_insertion(iobuffer_t *const iobuffer, const size_t transfer_size) {

	//Cache both channels pointers;
	iochannel_t *const input_channel = iobuffer->input_channel;
	iochannel_t *const output_channel = iobuffer->output_channel;

	//Cache the insertion address;
	uint8_t *input_address = input_channel->data_address;

	//Cache the buffer's bounds;
	void *buffer_max = iobuffer->buffer_max;

	//Update the insertion address to integrate the new transfer;
	input_address += transfer_size;

	//If we reached the last address (cannot be dereferenced) :
	if (input_address == iobuffer->buffer_max) {

		//Go to the first case;
		input_address = iobuffer->buffer_min;

	}

	//Save the new insertion address;
	input_channel->data_address = input_address;

	//Determine the new maximal insertion transfer size;
	input_channel->transfer_size = buffer_max - (void *) input_address;

	//Cache the output address;
	void *output_address = output_channel->data_address;

	//Determine the new maximal removal transfer size;
	//If the input is after the output, the max size is their subtraction;
	//If not, the max size is the number of addresses between input and limit;
	output_channel->transfer_size =
		((void *)input_address >= output_address) ? (void *)input_address - output_address : buffer_max - (void *) input_address;

}


/*
 * iobuffer_persist_insertion : mark that a deletion has been made, an update is required for output channel only;
 */

void iobuffer_persist_deletion(iobuffer_t *const iobuffer, const size_t transfer_size) {

	//Cache both channels pointers;
	iochannel_t *const input_channel = iobuffer->input_channel;
	iochannel_t *const output_channel = iobuffer->output_channel;

	//Cache the insertion address;
	uint8_t *output_address = output_channel->data_address;

	//Cache the buffer's bounds;
	void *buffer_max = iobuffer->buffer_max;

	//Update the deletion address to integrate the new transfer;
	output_address += transfer_size;

	//If we reached the last address (cannot be dereferenced) :
	if (output_address == iobuffer->buffer_max) {

		//Go to the first case;
		output_address = iobuffer->buffer_min;

	}

	//Save the new deletion address;
	output_channel->data_address = output_address;

	//Cache the insertion address;
	const void *const input_address = output_channel->data_address;

	//Determine the new maximal removal transfer size;
	//If the input is after the output, the max size is their subtraction;
	//If not, the max size is the number of addresses between input and limit;
	output_channel->transfer_size =
		(input_address >= (void *)output_address) ?
		input_address - (void *)output_address :
		buffer_max - (void *) input_address;

}
