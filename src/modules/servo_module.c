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
#include <kernel/fs/dfs.h>


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


//--------------------------------------------------- Structs --------------------------------------------------

struct servo_channel {

	//Servo channels are stored in linked lists; Access outside the ISR must be done in a critical section;
	struct list_head head;

	//The gpio file descriptor. Used to close the gpio file at channel deletion;
	struct dfs_file *const gpio_fd;

	//Each servo channel interfaces with a gpio pin; Access outside the ISR must be done in a critical section;
	const struct gpio_interface gpio;

	//The maximal duration (in us) of an impulse on the channel; Constant
	const uint32_t duration_max;

	//The current impulse duration; Can be written outside the ISR, for setup;
	uint32_t duration;
};


struct servo_manager {

	/*
	 * Channels lists. Access outside the ISR must be done in a critical section;
	 */

	//The first element of the channels list. This particular channel is the complement;
	// It is adjusted so that the duration of the complete cycle matches the period;
	struct servo_channel *const channels;

	//The currently active channel;
	struct servo_channel *active_channel;

	/*
	 * Constant data, provided at compile time;
	 */

	//The timer we use;
	const struct timer_interface timer;

	//The period of all servo channels;
	const uint32_t period;//TODO MACRO-ISE

	//The time tolerance;
	const uint32_t tolerance;//TODO MACRO-ISE


	//Is the ISR routine started ?;
	volatile bool started;

	//The total available time for new channels;
	uint32_t available_time;


};


//------------------------------------------------------- Globals ------------------------------------------------------

//There exists only one servo controller, for interrupt;
static struct servo_manager servos;


//----------------------------------------------------- Init - Exit ----------------------------------------------------

//TODO


//--------------------------------------------------------- ISR --------------------------------------------------------

/*
 * servo_service_start : this function starts the servo interrupt routine.
 */

static void start() {
	//TODO

}


/*
 * servo_service_stop : stop the interrupt routine;
 */

static void stop() {
	//TODO

}


static void update_isr_state() {

	//Cache manager parameters;
	const uint32_t complement = servos.channels->duration;
	const uint32_t period = servos.period;
	const bool started = servos.started;
	

	//If the manager is stopped, and the duration is not null :
	if ((!(started)) && (complement != period)) {

		//Start the ISR routine;
		start();

	} else if (started && (complement == period)) {
		//If the manager is started and the complement equals the period (no more active channels)

		//Stop the ISR routine;
		stop();

	}

}


/*
 * servo_isr_handler : the servo isr, called during a channel switch;
 * 	
 * 	It must clear the active channel's line, update the active channel, and set its line;
 */

void servo_isr_handler() {

	//Cache the servo timer pointer;
	const struct timer_interface *const timer = &servos.timer;

	//Disable the interruption;
	timer_disable_int(timer);

	
	//The active channel and its gpio;
	const struct servo_channel *active_channel = servos.active_channel;
	const struct gpio_interface *gpio = &active_channel->gpio;
	
	//Clear the channel
	gpio_clear(gpio, gpio->pin_mask);

	//Update the active channel and its gpio;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;d
	//TODO NOPE, TRAVERSE THE LIST UNTIL A NON NULL IS FOUND;
	active_channel = active_channel->head.next;
	gpio = &active_channel->gpio;
	
	//Set the channel
	gpio_set(gpio, gpio->pin_mask);

	//Save the active channel;
	servos.active_channel = active_channel;

	
	/*
	 * Timer setup for the next switch;
	 */
	
	//Program an overflow
	timer_set_ovf_value(timer, active_channel->duration);
	
	//Clear the interrupt flag;
	timer_flag_clr(timer);

	//Enable the servo interrupt;
	timer_enable_int(timer);

}


//--------------------------------------------------- Channels setup ---------------------------------------------------


static struct servo_channel *add_channel(const char *gpio_name, const uint32_t duration_max) {

	//If the new channel doesn't fit :
	if (duration_max > servos.available_time) {

		//Log;
		kernel_log_("servo : add_channel : not enough time.");

		//Fail;
		return 0;

	}

	//Open the gpio file;
	struct dfs_file *gpio_fd = dfs_open(gpio_name);

	//If the file does not exist :
	if (!gpio_fd) {

		//Log;
		kernel_log_("servo : add_channel : file not found.");

		//Fail;
		return 0;

	}

	//Create the servo channel initializer; Gpio will be interfaced with it;
	struct servo_channel init = {

		//Save the file descriptor;
		.gpio_fd = gpio_fd,

		//Save the duration;
		.duration_max = duration_max,

		//Stopped, duration of 0;
		.duration = 0,

	};

	//Interface with the gpio;
	bool success = file_op_interface(gpio_fd, (void *) &init.gpio, sizeof(struct gpio_interface));


	//If the interfacing failed :
	if (!success) {

		//Close the file;
		dfs_close(gpio_fd);

		//Log;
		kernel_log_("servo : add_channel : interfacing failed.");

		//Fail;
		return 0;

	}

	//Allocate some data for the channel;
	struct servo_channel *channel = kmalloc(sizeof(struct servo_channel));

	//Link the channel to itself;
	init.head.prev = init.head.next = channel;


	//Initialise the channel;
	memcpy(channel, &init, sizeof(struct servo_channel));

	//Update the available time;
	servos.available_time -= duration_max;

	//Access to the channels list is critical;
	ic_enter_critical_section();

	//Add the channel at the end of the channels list;
	list_concat((struct list_head *) channel, (struct list_head *) servos.channels);

	//Access to the channels list is critical;
	ic_leave_critical_section();

	//No start, as the channel has a duration of 0;

	//Return the channel;
	return channel;

}


//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;
//TODO INITIALISE THE FIRST CHANNEL AS THE COMPLEMENT;


/*
 * servo_module_remove_channel : removes a channel from the servo controller;
 */

static void remove_channel(struct servo_channel *const channel) {

	//If we attempt to remove the first channel (complement)
	if (channel == servos.channels) {

		//Log;
		kernel_log_("servo : remove_channel : attempted to remove the complement");

		//Fail;
		return;

	}

	//Access to the channels list is critical;
	ic_enter_critical_section();

	//If the channel we remove is the active one :
	if (channel == servos.active_channel) {

		//The previous one becomes the active channel;
		servos.active_channel = channel->head.prev;

	}

	//Remove the channel from the channels list;
	list_remove((struct list_head *) channel);

	//No more access to the list;
	ic_leave_critical_section();


	//Cache the channel's gpio;
	const struct gpio_interface *gpio = &channel->gpio;

	//Clear the channel's gpio
	gpio_clear(gpio, gpio->pin_mask);

	//Close the gpio file;
	dfs_close(channel->gpio_fd);

	//Update the complement;
	servos.channels->duration += channel->duration;

	//Update the available time;
	servos.available_time += channel->duration_max;

	//Finally, delete the channel;
	kfree(channel);

}


static void channel_set_value(struct servo_channel *const channel, uint32_t duration) {


	/*
	 * The duration must be adjusted, to take bounds into account;
	 */

	//Cache the maximal duration;
	const uint32_t dmax = channel->duration_max;

	//The duration can't be greater than its maximal duration;
	if (duration > dmax) {
		duration = dmax;
	}

	//The duration can't be lower than the tolerance;
	if (duration < servos.tolerance) {
		duration = 0;
	}


	/*
	 * We must update the complement, as we modify the duration;
	 */

	//Cache the previous duration;
	const uint32_t current_duration = channel->duration;

	//Cache the previous complement;
	uint32_t complement = servos.channels->duration;

	//If the duration is not changed :
	if (current_duration == duration) {

		//Do nothing;
		return;

	} else if (current_duration < duration) {
		//If the duration is decreased :

		//Increase the complement;
		complement += duration - current_duration;

	} else {
		//If the duration is increased :

		//Decrease the complement;
		complement -= current_duration - duration;

	}

	//Update the complement;
	servos.channels->duration = complement;

	//Update the channel's duration;
	channel->duration = duration;


	/*
	 * One channel has been modified, and the ISR routine must be eventually started or stopped;
	 */

	update_isr_state();
}

