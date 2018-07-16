//
// Created by root on 7/15/18.
//

#include "iobuffer.h"

#include <string.h>

#include <kernel/kernel.h>


//--------------------------------------------------- Private headers --------------------------------------------------

//Delete a created buffer;
void iobuffer_delete(iobuffer_t *buffer);


//----------------------------------------------------- Init - exit ----------------------------------------------------

/*
 * iobuffer_create : creates an IOBuffer from a couple of channels, and registers itself to active ones;
 */

void iobuffer_create(size_t size, iobuffer_channel_t *input_channel, iobuffer_channel_t *output_channel) {

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
	void *buffer_min = kernel_malloc(size);

	//Create an initializer;
	iobuffer_t init = {

		//Save the minimal address of the buffer;
		.buffer_min = buffer_min,

		//Determine and save the maximal address of the buffer;
		.buffer_max = size + (uint8_t *) buffer_min,

		//Save the size;
		.buffer_size = size,

		//Save the input channel;
		.input_channel = input_channel,

		//Save the output channel;
		.output_channel = output_channel,

	};

	//Create and allocate data
	iobuffer_t *iobuffer = kernel_malloc_copy(sizeof(iobuffer_t), &init);

	//Register the IOBuffer to the input channel;
	iobuffer_channel_register_buffer(input_channel, iobuffer);

	//Register the IOBuffer to the input channel;
	iobuffer_channel_register_buffer(output_channel, iobuffer);


	//Registration is a critical section, as a section could delete the buffer as it is not completely registered;
	kernel_enter_critical_section();

	//Activate both channels;
	input_channel->unregistered = false;
	output_channel->unregistered = false;

	//Leave the critical section;
	kernel_leave_critical_section();

}


/*
 * iobuffer_delete : deletes an existing buffer, after its two channels have unregistered themselves;
 */

void iobuffer_delete(iobuffer_t *buffer) {

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

void iobuffer_channel_delete(iobuffer_channel_t *channel) {

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
		iobuffer_channel_reset(iobuffer->input_channel);
		iobuffer_channel_reset(iobuffer->output_channel);


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

void iobuffer_persist_transfer(iobuffer_channel_t *channel) {

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

			//Persist a removal and update the output channel's access;
			iobuffer_persist_removal(iobuffer, transfer_size);

		}

	}

	//Leave the critical section;
	kernel_leave_critical_section();

}

/*
 * iobuffer_persist_insertion : mark that an insertion has been made,
 */

void iobuffer_persist_insertion(iobuffer_t *iobuffer, size_t transfert_size) {

	//Cache both data pointers;


}



