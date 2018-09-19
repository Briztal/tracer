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
 * Makefile must provide :
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


//--------------------------------------------------- Includes --------------------------------------------------



#include <kernel/mod/auto_mod.h>

#include <kernel/fs/inode.h>

#include <kernel/ic.h>


#include <util/macro/incr_call.h>

#include <util/string.h>
#include <kernel/mod/module_array.h>


#include "kx_sim.h"

#include "kx_pit_channel.h"


//------------------------------------------------- Internal parameters ------------------------------------------------

//The casted address of the MCR
#define MCR 	((volatile uint32_t *) MCR_REG)


//------------------------------------------------ Channel declarations ------------------------------------------------

/*
 * The PIT module interacts with a PIT peripheral.
 *
 * 	The PIT peripheral provides an array of independently configurable general-purpose timers;
 *
 * 	The PIT interface offers a timer interface for each one of there channels. It will register these structs to the
 * 	kernel at init, and un-register them at exit;
 */
/*

//CHANNEL_DECLARE will declare the channel struct and the channel init function;
#define CHANNEL_DECLARE(i) extern struct channel_specs kx_pit_channel_##i;

//Declare each channel;
INCR_CALL(NB_CHANNELS, CHANNEL_DECLARE);

//Macro not used anymore;
#undef CHANNEL_DECLARE
*/

MODULE_DECLARE_CHANNELS();

//--------------------------------------------------- Channel array ----------------------------------------------------

/*
//INIT_ARRAY will print the reference of the i-th timer channel;
#define INIT_ARRAY(i) &kx_pit_channel_##i,

//Initialize the channels data array;
static struct channel_specs *channels[NB_CHANNELS] = {INCR_CALL(NB_CHANNELS, INIT_ARRAY)};

//Macro not used anymore;
#undef INIT_ARRAY
*/


MODULE_CREATE_ARRAY(channels);



//-------------------------------------------------- File Operations --------------------------------------------------

/*
 * The channel inode will only contain the channel index;
 */
struct channel_inode {

	//The inode base;
	struct inode node;

	//The channel index;
	uint8_t channel_index;

};

//Inodes will be stored in an array;
static struct channel_inode inodes[NB_CHANNELS];


/*
 * All variables and methods in this section are private. No one can access them outside this translation unit;
 */

//Transmit the timer interface;
static bool channel_interface(const struct channel_inode *const node, void *const iface, const size_t iface_size) {

	//Cache the channel data ref;
	struct channel_specs *channel = channels[node->channel_index];

	//Eventually interface with the channel;
	return timer_if_interface(iface, &channel->ref, iface_size);

}


//Close the timer resource : will neutralise the interface;
static void channel_close(const struct channel_inode *const node) {

	//Cache the channel data ref;
	struct channel_specs *channel = channels[node->channel_index];

	//Neutralise the eventual interface;
	timer_if_neutralise(&channel->ref);
}


/*
 * The file operations for a pit channel are limited to interface, and close that neutralises the interface;
 */

static struct inode_ops channel_ops = {
	.close =  (void (*)(struct inode *)) &channel_close,
	.interface = (bool (*)(struct inode *, void *, size_t)) &channel_interface,
};


/**
 * 	kx_pit_channel_init_i : initialises the channel; Called by the pit module's init function
 */

static void channel_init(uint8_t channel_index) {

	//Cache the channel data ref;
	struct channel_specs *channel = channels[channel_index];


	/*
	 * Inode initialisation;
	 */

	//Cache the inode;
	struct channel_inode *node = inodes + channel_index;

	//Create the inode initializer;
	struct channel_inode init = {

		//Transmit operations;
		.node = INODE (&channel_ops),

		//Initialise the channel index;
		.channel_index = channel_index,

	};

	//Initialise the inode;
	memcpy(node, &init, sizeof(struct channel_inode));


	/*
	 * Fs registration;
	 */

	//Register a file with no content leading to our operations;
	fs_create(channel->name, (struct inode *) node);


	/*
	 * Timer reset;
	 */

	//Reset the timer, and set the base frequency to 1KHz
	timer_reset(&channel->ref.iface, 1000);


	/*
	 * Interrupt settings;
	 */

	//Cache the interrupt channel;
	const uint16_t int_channel = channel->interrupt_channel;

	//Register the pit handler;
	ic_set_interrupt_handler(int_channel, channel->handler);

	//Set the channel int priority;
	ic_set_interrupt_priority(int_channel, KERNEL_DRIVER_STATUS_PRIORITY);

	//Enable the channel interrupt;
	ic_enable_interrupt(int_channel);

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
KERNEL_EMBED_PERIPHERAL_MODULE(pit, &pit_init);