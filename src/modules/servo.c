/*
  servo_module.c -  Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

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

#include <util/type.h>
#include <kernel/log.h>
#include <kernel/interface/timer.h>
#include <kernel/interface/gpio.h>
#include <kernel/krnl.h>
#include <util/string.h>
#include <kernel/ic.h>
#include <kernel/fs/inode.h>
#include <kernel/mod/auto_mod.h>
#include <kernel/interface/command.h>
#include "servo.h"


/*
 * The Servo control module manages a variable number of servo channels, each channel communicating with one servo.
 *
 * 	The servo module has build-in parameters, that can be changed in the makefile :
 * 	- the timer to used, defined by its name; An invalid name will cause module to be unloaded;
 * 	- the period of all servos;
 * 	- the tolerance.
 *
 * 	All parameters that only concern one channel;
 *
 * 	Channels can be created, deleted, and their value can be set.
 *
 * 	All channels share the same period;
 *
 * Module lifecycle :
 *
 * 	- Initialisation :
 * 		The servo timer, the total period, and the minimal duration are provided;
 * 		No channel is added for instance, as the cycle period is not know;
 *
 *
 * 	- channels setup :
 * 		channels are created, adjusted, or deleted; The ISR becomes active if channels are active, and inactive if no
 * 		channel is active;
 *
 *
 *
 * Channels lifecycle :
 *
 * 	- Channels creation :
 * 		Channels are created with a name, a gpio and a maximal duration;
 * 		Their duration is set to 0, so that they only become active when required;
 *
 * 	- Channels duration setup;
 * 		A channel's duration can be adjusted at any time, while the channel exists; If the requires duration is beyond
 * 		its maximal value, it will be decreased to it; If
 *
 * 	- Channels deletion;
 * 		A channel can be deleted at any time, via the file system; The associated GPIO will be cleared;
 *
 */


//--------------------------------------------------- Make parameters --------------------------------------------------

//If one of the macro was not provided :
#if !defined(PERIOD) || !defined(TOLERANCE) || !defined(TIMER_NAME) || !defined(MODULE_NAME) || !defined(CHANNELS_FILE)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Default period to 20 ms;
#define PERIOD 20000

//Default period to 5 us;
#define TOLERANCE 5

//Non existing timer name;
#define TIMER_NAME dummy_timer

//RDM
#define MODULE_NAME servos

//default config file;
#define CHANNELS_FILE "servo.h"

#endif


//------------------------------------------------------ Channels ------------------------------------------------------

struct servo_channel {

	//The gpio file descriptor. Used to close the gpio file at channel deletion;
	file_descriptor gpio_fd;

	//Each servo channel interfaces with a gpio pin; Access outside the ISR must be done in a critical section;
	struct gpio_interface gpio;

	//The current impulse duration; Can be written outside the ISR, for setup;
	uint32_t duration;
};


//------------------------------------------------ Channels structures -------------------------------------------------

//Define the complement channel. At init, occupies the whole period;
static struct servo_channel complement_channel = {};

//Define a servo channel struct;
#define SERVO(name, gpio, max_duration) static struct servo_channel channel_##name = {};

//Defin all servo channels;
#include "servo.h"

//Macro not used anymore
#undef SERVO



//--------------------------------------------- Channels update functions ----------------------------------------------

//Define the update function for a channel;
#define SERVO(name, gpio, max_duration)\
static void channel_update_##name(const uint32_t duration) {\
    channel_update_duration(&channel_##name.duration, duration, max_duration);\
}

//Define all update functions;
#include "servo.h"

//Macro not used anymore
#undef SERVO


//--------------------------------------------- Channels interfaces array ----------------------------------------------

/*
 * channels interfaces are stored in an array, and accessed by index during interfacing;
 */

//Initialise the interface struct for a channel;
#define SERVO(name, gpio, max_duration) {.set = channel_update_##name},

//Create the interfaces array;
struct command_if if_array[] = {

	//Add the complement channel. Never interfaced with;
	0,

	//Add each servo channel's reference;
#include "servo.h"

};

//Macro not used anymore
#undef SERVO


/*
 * We must keep track of interfaces references for neutralisation. The array below will reference them;
 */

//Write an initializer for each servo channel;
#define SERVO(name, gpio, max_duration) 0,

//Create an array to store interfaces references;
struct command_if *if_refs[] = {

	//Add the complement channel. Never interfaced with;
	0,

	//Add each servo channel's reference;
#include "servo.h"

};

//Macro not used anymore;
#undef SERVO



//---------------------------------------------- Channels array ----------------------------------------------


//There is a constant number of servo channels; The array size will be determined by the initializer;
#define SERVO(name, gpio, max_duration) &channel_##name,

//Create the channels array;
struct servo_channel *channels[] = {

	//Add the complement channel's reference;
	&complement_channel,

	//Add each servo channel's reference;
#include "servo.h"

};

//Macro not used anymore;
#undef SERVO




//--------------------------------------------------- Controller data --------------------------------------------------

//The number of channels, complement comprised;
#define NB_CHANNELS (sizeof(channels) / sizeof(struct servo_channel *))

//The currently active channel;
static size_t active_index;

//The timer file descriptor;
static file_descriptor timer_fd;

//The timer we use;
static struct timer_interface timer;

//Is the ISR routine started ?;
static volatile bool started;



//---------------------------------------------------- inode structs ---------------------------------------------------

/*
 * The channel inode is only used for interfacing. Only Interfacing data will be stored inside;
 */
struct channel_inode {

	//The inode base;
	struct inode node;

	//The channel index;
	size_t channel_index;

};


//------------------------------------------------------- Headers ------------------------------------------------------


//ISR management;
static void start();

static void stop();

static void update_isr_state();


//The ISR routine;
static void servos_isr_handler();


//Init;
static bool servo_init();


//Controller inode operations;
static void op_servo_exec();


//Channels inode operations;
static void op_channel_interface();

static void op_channel_close();

static void op_channel_delete();




//--------------------------------------------------- Channels init ---------------------------------------------------


static bool check_max_durations() {

	//Insert an incrementation of the maximal duration;
	#define SERVO(name, gpio, max_duration) + (max_duration)

	//Declare the sum of all maximal durations;
	size_t durations_sum = 0

							   //Sum all durations;
						   #include "servo.h"

	//Terminate;
	;

	//Macro not used anymore;
	#undef SERVO

	//The cycle is valid if all channels can be set to their maximal durations without overflow;
	return durations_sum < PERIOD;

}

/**
 * init_channel : TODO
 */

static bool init_channel(size_t channel_id, const char *const gpio_name) {

	//Cache the channel struct;
	struct servo_channel *channel = channels[channel_id];

	//Open the gpio file;
	file_descriptor gpio_fd = fs_open(gpio_name);

	//If the file does not exist :
	if (!gpio_fd) {

		//Log;
		kernel_log_("servo : add_channel : file not found.");

		//Fail;
		return false;

	}

	//Save the file descriptor;
	channel->gpio_fd = gpio_fd;

	//Stopped, duration of 0;
	channel->duration = 0;

	//Interface with the gpio;
	bool success = inode_interface(gpio_fd, &channel->gpio, sizeof(struct gpio_interface));


	//If the interfacing failed :
	if (!success) {

		//Close the file;
		fs_close(gpio_fd);

		//Log;
		kernel_log_("servo : init_channel : interfacing failed.");

		//Fail;
		return false;

	}

	//If the interfacing succeeded, complete;
	return true;

}


static void update_channel_duration(uint32_t *dst, uint32_t duration, uint32_t max_duration) {

	//Major the duration with its max;
	if (duration > (max_duration)) duration = (max_duration);

	//if the channel must be stopped :
	if (duration < TOLERANCE) {

		//Reset the duration;
		duration = 0;
	}

	//Cache the current complement;
	uint32_t complement = complement_channel.duration;

	//Cache the current duration;
	const uint32_t current = *dst;

	//Update the complement;
	if (duration > current) complement -= duration - current;
	else complement += current - duration;

	//If the manager is stopped, and the duration is not null :
	if ((!(started)) && (complement != PERIOD)) {

		//Start the ISR routine;
		start();

	} else if (started && (complement == PERIOD)) {
		//If the manager is started and the complement equals the period (no more active channels)

		//Stop the ISR routine;
		stop();
	}

	//Update the duration and the complement;
	*dst = duration;
	complement_channel.duration = complement;

};


//--------------------------------------------------------- ISR --------------------------------------------------------


/**
 * start : setups the isr environment, and starts the timer;
 */

static void start() {

	//Update the active channel;
	active_index = 0;

	//Mark the servo module started;
	started = true;

	//Initialise the timer : 1MHz base, ovf in the first duration,
	timer_init(&timer, 1000000, channels[0]->duration, &servos_isr_handler);

}


/**
 * start : stops the timer;
 */

static void stop() {

	//Enter a critical section;
	ic_enter_critical_section();

	//Reset the timer;
	timer_reset(&timer, 1000000);

	//Mark the servo module stopped;
	started = false;

	//Leave the critical section;
	ic_leave_critical_section();

}


/**
 * servos_isr_handler : the servo isr, called during a channel switch;
 *
 * 	It must clear the active channel's line, update the active channel, and set its line;
 */

static void servos_isr_handler() {


	//Disable the interruption;
	timer_disable_int(&timer);

	//Cache the active channel;
	size_t active_id = active_index;
	struct servo_channel *channel = channels[active_id];

	//The active channel and its gpio;
	const struct gpio_interface *gpio = &channel->gpio;

	//Clear the channel
	gpio_clear(gpio, gpio->pin_mask);

	//Traverse the list :
	do {

		//Focus on the next channel
		if (active_id) active_id--;
		else active_id = NB_CHANNELS;

		//Update the channel;
		channel = channels[active_id];

		//While the channel's duration is null;
	} while (!channel->duration);


	//Cache the channel's gpio,
	gpio = &channel->gpio;

	//Set the channel
	gpio_set(gpio, gpio->pin_mask);

	//Save the active channel;
	active_index = active_id;


	/*
	 * Timer setup for the next switch;
	 */

	//Program an overflow
	timer_set_ovf_value(&timer, channel->duration);

	//Clear the interrupt flag;
	timer_flag_clr(&timer);

	//Enable the servo interrupt;
	timer_enable_int(&timer);

}



/*
 * ---------------------------------------------------------------------------------------------------------------------
 * ----------------------------------------------- Kernel Interface -----------------------------------------------
 * ---------------------------------------------------------------------------------------------------------------------
 */

//----------------------------------------------------- Init - Exit ----------------------------------------------------

#define _ST(x) #x
#define ST(x) _ST(x)

static bool servo_init() {

	//If the cycle is invalid :
	if (!check_max_durations()) {

		//Log;
		kernel_log_("servo_init : the period is too small to contain all cycles at their max durations;");

		//Fail;
		return false;

	}

	//Open the timer file;
	file_descriptor fd = fs_open(ST(TIMER_NAME));

	//If the file does not exist :
	if (!fd) {

		//Log;
		kernel_log_("servo : add_channel : file not found.");

		//Fail;
		return false;

	}

	//Interface with the gpio;
	bool success = inode_interface(fd, &timer, sizeof(struct timer_interface));


	//If the interfacing failed :
	if (!success) {

		//Close the file;
		fs_close(fd);

		//Log;
		kernel_log_("servo : add_channel : interfacing failed.");

		//Fail;
		return false;

	}


	//Mark the complement channel active;
	active_index = 0;

	//Save the file descriptor;
	timer_fd = fd;

	//Mark ISR routine stopped ?;
	started = false;


	//Initialise the first channel;
	complement_channel = {

		//Occupies the whole period;
		.duration = PERIOD,

		//Neutral gpio interface;
		.gpio = neutral_gpio_interface,

		//No gpio file;
		.gpio_fd = 0,

	};

	size_t i = 1;

	//Initialize a channel and register it to the file system;
	#define SERVO(name, gpio, duration)\
		init_channel(i, gpio);\
		register_channel(i++, name);

	//Add all initializers;
	#include "servo.h"

	//Macro not used anymore;
	#undef SERVO;

	//TODO INITIALISE ALL OTHER CHANNELS;

}

KERNEL_EMBED_SYSTEM_MODULE(MODULE_NAME, &servo_init);




//------------------------------------------- Servo channel inode operations -------------------------------------------

//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.
//TODO GENERIC INTERFACING STRUCTURE THAT CONTAINS THE REFERENCE.


/*
 * channel_interface : interfaces with the required channel;
 */
static void channel_interface(

	const struct channel_inode *const node,
	struct command_if *const if_struct,
	const size_t size

) {

	//The channel inode contains the identifier of the channel to interface;
	const size_t channel_id = node->channel_index;

	//If the channel id is invalid :
	if ((!channel_id) || (channel_id > NB_CHANNELS)) {

		//Log;
		kernel_log_("servo : channel_interface : invalid index provided;")

		//Fail;
		return;
	}

	//Attempt to interface;
	bool success = command_if_interface(if_struct, if_array + channel_id, size);

	//If the interfacing succeeds :
	if (success) {

		//Save the interface reference;
		if_refs[channel_id] = if_struct;

	}

}

static void channel_close(const struct channel_inode *const node) {

	//The channel inode contains the identifier of the channel to interface;
	const size_t channel_id = node->channel_index;

	//If the channel id is invalid :
	if ((!channel_id) || (channel_id > NB_CHANNELS)) {

		//Log;
		kernel_log_("servo : chanel_close : invalid index provided;")

		//Fail;
		return;

	}

	//Cache the related interface reference;
	struct command_if *iface = if_refs[channel_id];

	//If the channel is interfaced :
	if (iface) {

		//Neutralise the interface;
		command_if_neutralise(iface);

	}

}

