/*
  servo.c -  Part of TRACER

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



//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * Please provide in the compilation call :
 * 	- PERIOD : the period of the cycle, expressed in microseconds;
 * 	- TOLERANCE : the minimal activity time of a channel, expressed in microseconds;
 * 	- TIMER_NAME : the name of the timer to interface with. No quotes required;
 * 	- MODULE_NAME : the name the module should be embedded with. No quotes required;
 * 	- CHANNELS_FILE : the name of the file where servo channels macros are defined. Quotes required;
 *
 * 	You may use this line : in your makefile :
 *
 * 	-DPERIOD= -DTOLERANCE= -DTIMER_NAME= -DMODULE_NAME= -DCHANNELS_FILE=
 */

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


//------------------------------------------------------ Includes ------------------------------------------------------

//If one of the macro was not provided :
#include <kernel/fs/inode.h>
#include <kernel/interface/gpio.h>
#include <kernel/interface/command.h>
#include <kernel/interface/timer.h>
#include <kernel/log.h>
#include <kernel/ic.h>
#include <kernel/mod/auto_mod.h>


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


//Declare the duration update function;
static void update_channel_duration(uint32_t *, uint32_t);

//Define the update function for a channel; It will decrease the duration if requried, and call the duration updater;
#define SERVO(name, gpio, max_duration)\
static void channel_update_##name(uint32_t duration) {\
	if (duration > (uint32_t) (max_duration)) duration = (max_duration);\
    update_channel_duration(&channel_##name.duration, duration);\
}

//Define all update functions;
#include "servo.h"

//Macro not used anymore
#undef SERVO


//--------------------------------------------- Channels interfaces ----------------------------------------------

/*
 * channels interfaces are stored in an array, and accessed by index during interfacing;
 */

//Initialise the interface struct for a channel;
#define SERVO(name, gpio, max_duration) {.iface = {.set = &channel_update_##name}, .ref = 0,},

//Create the interfaces array;
static struct command_if_ref interfaces[] = {
	//Add the complement channel. Never interfaced with;
	{},

	//Add each servo channel's reference;
#include "servo.h"

};

//Macro not used anymore
#undef SERVO




//---------------------------------------------- Channels array ----------------------------------------------


//There is a constant number of servo channels; The array size will be determined by the initializer;
#define SERVO(name, gpio, max_duration) &channel_##name,

//Create the channels array;
static struct servo_channel *channels[] = {

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
static struct timer_if timer;

//Is the ISR routine started ?;
static volatile bool started;




//--------------------------------------------------------- ISR --------------------------------------------------------

/**
 * servos_isr_handler : the servo isr, called during a channel switch;
 *
 * 	It must clear the active channel's line, update the active channel, and set its line;
 */

static void servos_isr_handler() {


	//kernel_log_("servo handler.");

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
		if (!active_id) active_id = NB_CHANNELS;
		active_id--;

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
	timer_set_int_period(&timer, channel->duration);

	//Clear the interrupt flag;
	timer_flag_clr(&timer);

	//Enable the servo interrupt;
	timer_enable_int(&timer);

}


/**
 * start : setups the isr environment, and starts the timer;
 */

static void start() {

	kernel_log_("servo starting.");

	//Update the active channel;
	active_index = 0;

	//Mark the servo module started;
	started = true;

	//Initialise the timer : 1MHz base, ovf in the first duration,
	timer_init(&timer, 1000000, channels[0]->duration, &servos_isr_handler);

	kernel_log("dur : %d", channels[0]->duration);

	ic_enable_interrupts();

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


//--------------------------------------------------- Channels setup ---------------------------------------------------

/**
 * check_max_durations : this function is called once only at the module initialisation;
 *
 * 	It will determine the sum of all maximal durations and assert if it is inferior to the period.
 *
 * 	If it asserts, the module can work correctly at this configuration;
 *
 * 	If not, the module initialisation should fail, as all servos can't be set at their maximal value in teh required
 * 	period;
 *
 * 	@return false if the module has been badly configured;
 */

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
	return durations_sum <= PERIOD;

}


/**
 * init_channel : opens the required gpio, interfaces with it, and initialises the channel data;
 *
 * @return true if the initialisation was made properly. If not, the module should fail;
 */

static bool init_channel(size_t channel_id, const char *const gpio_name) {

	//Cache the channel struct;
	struct servo_channel *channel = channels[channel_id];

	//Open the gpio file;
	file_descriptor gpio_fd = fs_open(gpio_name);

	//If the file does not exist :
	if (!gpio_fd) {

		//Log;
		kernel_log("servo : %s gpio file not found.", gpio_name);

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
		kernel_log("servo : %s gpio interfacing failed.", gpio_name);

		//Fail;
		return false;

	}

	//If the interfacing succeeded, complete;
	return true;

}

/**
 * update_channel_duration : sets the duration of the required channel, and updates the complement's duration;
 *
 * @param dst : the location of the channel's current duration, and where the new one should be stored;
 * @param duration : the required duration;
 * @param max_duration : the maximal value of the duration;
 */

static void update_channel_duration(uint32_t *dst, uint32_t duration) {

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

	//Update the duration and the complement;
	*dst = duration;
	complement_channel.duration = complement;


	//If the manager is stopped, and the duration is not null :
	if ((!(started)) && (complement != PERIOD)) {

		//Start the ISR routine;
		start();

	} else if (started && (complement == PERIOD)) {
		//If the manager is started and the complement equals the period (no more active channels)

		//Stop the ISR routine;
		stop();
	}


};


//----------------------------------------------- File system interaction ----------------------------------------------

/*
 * The channel inode is only used for interfacing. Only Interfacing data will be stored inside;
 */

struct channel_inode {

	//The inode base;
	struct inode node;

	//The channel index;
	size_t channel_index;

};


//Write the initializer for an inode;
#define SERVO(name, gpio, max) {},

//Inodes can be stored in an array, as their amount is predetermined, and never altered;
static struct channel_inode inodes[] = {

	//Add the complement;
	{},

	//Add all inodes.
#include "servo.h"

};

//Macro not used anymore;
#undef SERVO


/**
 * channel_interface : interfaces with the required channel;
 *
 * @param node : the channel's inode;
 * @param if_struct : the struct to interface;
 * @param size : the size of if_struct. Used for error check;
 * @return true if interfacing complete;
 */

static bool channel_interface(

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
		return false;
	}

	//Allow interfacing. Size check is taken in charge.
	command_if_interface(if_struct, interfaces + channel_id, size);

	//Complete;
	return true;

}


/**
 * channel_close : neutralises the channel's eventual interface;
 *
 * @param node : the channel's inode;
 */

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

	//Neutralise the interface, if interfaced;
	command_if_neutralise(interfaces + channel_id);

}


/*
 * Servo channel inodes accept following calls. Cast is required in order to downcast inodes into channel_inodes. Safe;
 */

static const struct inode_ops channel_ops = {
	.interface = (bool (*)(struct inode *, void *, size_t)) &channel_interface,
	.close = (void (*)(struct inode *)) &channel_close,

};


/**
 * channel_register : registers the channel to the file system. Inode must be initialised;
 *
 * @param channel_name
 * @param channel_index
 */

void channel_register(const char *channel_name, const size_t channel_index) {

	//Cache the inode ref;
	struct channel_inode *node = inodes + channel_index;

	//Create the inode initializer;
	struct channel_inode init = {

		//Initialise the node base, providing channel operations;
		.node = INODE(&channel_ops),

		//Save the node index;
		.channel_index = channel_index,
	};

	//Initialise the channel;
	memcpy(node, &init, sizeof(struct channel_inode));

	//Register the inode to the file system; Safe up-cast;
	fs_create(channel_name, (struct inode *) node);

}


//----------------------------------------------------- Init - Exit ----------------------------------------------------

#define _ST(x) #x
#define ST(x) _ST(x)

static bool servo_init() {

	/*
	 * Cycle check;
	 */

	//If the cycle is invalid :
	if (!check_max_durations()) {

		//Log;
		kernel_log_("servo_init : the period is too small to contain all cycles at their max durations;");

		//Fail;
		return false;

	}


	/*
	 * Timer interfacing;
	 */

	//Open the timer file;
	file_descriptor fd = fs_open(ST(TIMER_NAME));

	//If the file does not exist :
	if (!fd) {

		//Log;
		kernel_log_("servo : timer file not found.");

		//Fail;
		return false;

	}

	//Interface with the gpio;
	bool success = inode_interface(fd, &timer, sizeof(struct timer_if));


	//If the interfacing failed :
	if (!success) {

		//Close the file;
		fs_close(fd);

		//Log;
		kernel_log_("servo : timer interfacing failed.");

		//Fail;
		return false;

	}


	/*
	 * Globals init;
	 */

	//Mark the complement channel active;
	active_index = 0;

	//Save the file descriptor;
	timer_fd = fd;

	//Mark ISR routine stopped ?;
	started = false;


	/*
	 * Complement channel init;
	 */

	//Create the complement channel initializer;
	struct servo_channel cpl_init = {

		//Occupies the whole period;
		.duration = PERIOD,

		//Neutral gpio interface;
		.gpio = neutral_gpio_interface,

		//No gpio file;
		.gpio_fd = 0,

	};

	//Initialise the complement channel;
	memcpy(&complement_channel, &cpl_init, sizeof(struct servo_channel));


	/*
	 * Standard interfaces init and registration;
	 */

	//Channel 0 is the complement, has already been initialised, and must not be registered;
	size_t i = 1;

	//Initialize a channel and register it to the file system;
	#define SERVO(name, gpio, duration)\
        init_channel(i, #gpio);\
        channel_register(#name, i);\
		i++;

	//TODO SEPARATE REGISTRATION AND INITIALISATION;
	//TODO CLEANUP IF NOT PROPERLY INITIALISED : CLOSE GPIO FILES, AND UNREGISTER.

	//Add all initializers;
	#include "servo.h"

	//Macro not used anymore;
	#undef SERVO

	//Complete;
	return true;

}

//Embed the module in the executable;
KERNEL_EMBED_SYSTEM_MODULE(MODULE_NAME, &servo_init);
