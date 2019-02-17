/*
  kx_pit.c Part of TRACER

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


//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * mk64fx512.mk must provide :
 * 	- MCR_REG : address of the MCR register;
 * 	- NB_CHANNELS : number of channels;
 */


//If one of the macro was not provided :
#if !defined(MCR_REG) || !defined(MCR_REG)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define    MCR_REG      0
#define    NB_CHANNELS  4

#endif


//------------------------------------------------------ Includes ------------------------------------------------------

#include <kernel/exec/mod_hook>

#include <fs/iinode.h>

#include <kernel/hard/except.h>

#include <macro/incr_call.h>

#include <stdmem.h>

#include <kernel/exec/module_array.h>

#include "kx_sim.h"

#include "kx_pit_n.h"


//------------------------------------------------- Internal parameters ------------------------------------------------

//The casted address of the MCR
#define MCR 	((volatile uint32_t *) MCR_REG)


//------------------------------------------------- Specs declarations -------------------------------------------------

MODULE_CREATE_SPECS_ARRAY(channels)


//----------------------------------------- Channels dynamic data declarations -----------------------------------------

/*
 * Channels dynamic data simply consist on if references;
 */

static struct timer_if *if_refs[NB_CHANNELS] = {0};



//--------------------------------------------------- File Operations --------------------------------------------------

/*
 * The channel iinode will only contain the channel index;
 */
struct channel_inode {

	//The iinode base;
	struct iinode node;

	//The channel index;
	uint8_t channel_index;

};

//Inodes will be stored in an array;
static struct channel_inode inodes[NB_CHANNELS];


/*
 * All variables and methods in this section are private. No one can access them outside this translation unit;
 */

//Transmit the timer if;
static bool channel_interface(const struct channel_inode *const node, void *const iface, const size_t iface_size) {

	//Cache the channel index,
	uint8_t channel_index = node->channel_index;

	//printkf("interfacing with %d ", channel_index);

	//Cache the channel specs ref;
	const struct channel_specs *channel = channels[node->channel_index];

	//Cache the if ref ref;
	struct timer_if **if_ref = (if_refs + channel_index);
	
	//Eventually if with the channel;
	return iface_connect(iface, &channel->iface, if_ref);

}


//Close the timer resource : will neutralise the if;
static void channel_close(const struct channel_inode *const node) {

	//Neutralise the eventual if;
	iface_disconnect(if_refs + node->channel_index, &neutral_timer_if);

}


/*
 * The file operations for a pit channel are limited to if, and close that neutralises the if;
 */

static struct inode_ops channel_ops = {
	.close =  (void (*)(struct iinode *)) &channel_close,
	.interface = (bool (*)(struct iinode *, void *, size_t)) &channel_interface,
};


/**
 * 	kx_pit_channel_init_i : initialises the channel; Called by the pit module's init function
 */

static void channel_init(uint8_t channel_index) {

	//Cache the channel data ref;
	const struct channel_specs *channel = channels[channel_index];


	/*
	 * Inode initialisation;
	 */

	//Cache the iinode;
	struct channel_inode *node = inodes + channel_index;

	//Create the iinode initializer;
	struct channel_inode init = {

		//Transmit operations;
		.node = INODE (&channel_ops),

		//Initialise the channel index;
		.channel_index = channel_index,

	};

	//Initialise the iinode;
	memcpy(node, &init, sizeof(struct channel_inode));


	/*
	 * Fs registration;
	 */

	//Register a file with no content leading to our operations;
	fs_create(channel->name, (struct iinode *) node);


	/*
	 * Timer reset;
	 */

	//Reset the timer;
	timer_reset(&channel->iface);


	/*
	 * Interrupt settings;
	 */

	//Cache the interrupt channel;
	const uint16_t int_channel = channel->interrupt_channel;

	//Register the pit handler;
	irq_set_handler(int_channel, channel->handler);

	//Set the channel int priority;
	irq_set_priority(int_channel, KERNEL_DRIVER_STATUS_PRIORITY);

	//Enable the channel interrupt;
	irq_enable(int_channel);

}



//----------------------------------------------------- Init - Exit ----------------------------------------------------

static bool pit_init() {

	//Enable PIT clock gating;
	sim_enable_PIT_clock_gating();

	//Enable clocks for all PITs;
	*MCR = 0x00;

	//Write the call to the initialisation function;
	#define TIMER_INIT(i) channel_init(i);

	//Write each channel's init call;
	INCR_CALL(NB_CHANNELS, TIMER_INIT);

	//Macro not used anymore;
	#undef TIMER_INIT

	//Complete;
	return true;

}



//Embed the PIT module in the executable;
KERNEL_HOOK_MODULE(PERIPHERAL_MODULE, pit, &pit_init)