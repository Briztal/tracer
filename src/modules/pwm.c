//
// Created by root on 9/12/18.
//


//-------------------------------------------------- Module array head -------------------------------------------------

#include <kernel/mod/module_array.h>


//--------------------------------------------------- Make parameters --------------------------------------------------

#if !defined(TOLERANCE) || !defined(TIMER_NAME)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Default period to 5 us;
#define TOLERANCE 5

//Non existing timer name;
#define TIMER_NAME dummy_timer

//RDM
#define MODULE_NAME servos

#endif

//------------------------------------------------------- Headers ------------------------------------------------------


#include <kernel/debug/debug.h>
#include <kernel/log.h>
#include <kernel/mod/auto_mod.h>

#include <kernel/ic.h>

#include <kernel/interface/timer.h>
#include <kernel/interface/gpio.h>
#include <kernel/fs/inode.h>

#include <util/string.h>

#include "pwm_channel.h"


//------------------------------------------------- Channels const data ------------------------------------------------

MODULE_CREATE_SPECS_ARRAY(specs);


//------------------------------------------------------- Structs ------------------------------------------------------

struct channel_data {

	//--------------- ISR PART ---------------

	//Channels are stored and sorted; Non IRQ can access only in a critical section;
	struct list_head head;

	//The duration between this pins's toggle and the previous one;
	// Non IRQ can access only when channel is inactive. IRQ has full access;
	uint32_t toogle_delay;

	//A flag set if the channel must be de-activated; Set by IRQ, cleared by non-IRQ;
	bool inactive;

	//The direction of the toogle; Non IRQ can access only when channel is inactive. IRQ has full access;
	bool next_state;



	//--------------- NON ISR PART ---------------

	//The duration of a high state; Can be modified asynchronously to setup the pwm;
	uint32_t high_duration;

	//The duration of a low state; Can be modified asynchronously to setup the pwm;
	uint32_t low_duration;

	//The gpio file descriptor;
	file_descriptor gpio_fd;

	//The gpio interface; Non IRQ can access only in a critical section;
	struct gpio_if gpio;

	//The ref of the struct interfaced with the channel;
	struct command_if *iface_ref;

};


//---------------------------------------------------- Manager Data ----------------------------------------------------

//The list containing active channels; Non IRQ can access only in a critical section;
struct channel_data *active_channels;

//The channels array;
struct channel_data channels_data[NB_CHANNELS];

//The status flag. Set when the IRQ routine is enabled.
bool started;

//The timer file descriptor;
file_descriptor timer_fd;

//The timer interface;
struct timer_if pwm_timer;



//------------------------------------------------------- Headers ------------------------------------------------------


static void toggle_channel(struct channel_data *channel);

static struct channel_data *channel_insert(struct channel_data *first, struct channel_data * new_channel);

static void pwm_isr();

//------------------------------------------------------- PWM irq ------------------------------------------------------

/**
 * start : setups the isr environment, and starts the timer;
 */

static void start() {

	//If the IRQ routine is already started, fail;
	if (started) {
		return;
	}

	//If there are no active channels :
	if (!active_channels) {
		return;
	}

	//Mark the pwm module started;
	started = true;

	//Initialise the timer : 1MHz base, ovf immediately;
	timer_init(&pwm_timer, 1000000, 1, &pwm_isr);

}


/**
 * start : stops the timer;
 */

static void stop() {

	//Enter a critical section;
	ic_enter_critical_section();

	//Reset the timer;
	timer_reset(&pwm_timer, 1000000);

	//Mark the pwm module stopped;
	started = false;

	//Reset the active channels list;
	active_channels = 0;

	//Leave the critical section;
	ic_leave_critical_section();

}


/**
 * pwm_irq : this function is the core of the pwm manager. It does the following :
 *
 * 	For each active GPIO, until the sum of delays is greater than the tolerance) :
 *
 * 		- remove the GPIO from the active list;
 *
 * 		- if the GPIO must be de-activated :
 *
 * 			- insert the GPIO in the inactive list;
 *
 * 			- if the GPIO was the only active channel, stop;
 * 			- if other channels are active, continue;
 *
 * 		- if the GPIO must be toggled :
 *
 * 			- toggle the GPIO, update its delay and future state;
 * 			- re-insert the GPIO in the sorted list;
 *
 * 			- if the GPIO was the only active channel, stop;
 * 			- if not, keep traversing;
 *
 *
 * 	It is called by a timer interrupt;
 */

static void pwm_isr() {

	//Cache the active list;
	struct channel_data **active_list = &active_channels;

	//Cache the first pwm of the active list;
	struct channel_data *channel = *active_list;

	//If the list is empty :
	if (!channel) {
		stop();
	}

	//Cache the time to re-interruption;
	uint32_t delay = 0;

	//Cache the pwm tolerance;
	const uint32_t tolerance = (uint32_t) TOLERANCE;

	//We will toggle all channels until the delay becomes greater than the tolerance;
	do {

		//Cache the next channel;
		struct channel_data *next = channel->head.next;

		//Is the list a singleton ?
		bool singleton = channel == next;

		//Remove the channel from the active list;
		list_remove((struct list_head *) channel);

		//If the channel must be stopped :
		if (!channel->high_duration) {

			//Mark the channel de-activated;
			channel->inactive = true;

			//If the channel was the only member of the list :
			if (singleton) {

				//Stop the pwm manager;
				stop();

				//Complete here, nothing to do;
				return;

			}

			//If the struct was not a singleton, keep traversing;

		} else {

			//Toggle the channel; delay time until toggle is placed in channel->toogle_delay, next state is updated;
			toggle_channel(channel);

			//If the list was not a singleton :
			if (!singleton) {

				//Re-insert the channel, and update the first channel;
				next = channel_insert(next, channel);

			}

			/*
			 * If the list was a singleton, delay will take a greater value than the tolerance, and the traversal
			 * 	will stop here.
			 */

		}


		//Focus on the next channel;
		channel = next;

		//Increment the delay, to comprise the new channel's delay;
		delay += next->toogle_delay;

	} while (delay < tolerance);

	//Update the first active channel;
	*active_list = channel;

	//Cache the timer ref;
	const struct timer_if *iface = &pwm_timer;


	//Update the timer's period to interrupt;
	timer_set_int_period(iface, delay);

	//Clear the timer ovf flag;
	timer_flag_clr(iface);


}


//---------------------------------------------------- Channels mgt ----------------------------------------------------

/**
 * toggle_channel : toggles the provided channel, and updates its next state, and its next duration;
 *
 * @param channel : the channel to toggle;
 */

static void toggle_channel(struct channel_data *channel) {

	//Cache the GPIO interface;
	struct gpio_if *gpio = &channel->gpio;

	//Declare the next duration;
	uint32_t next_duration;

	//Cache the state to put the GPIO on;
	bool state = channel->next_state;

	//If the channel must be set high :
	if (state) {

		//Set the GPIO;
		gpio_set(gpio, gpio->pin_mask);

		//The next duration is the one corresponding to the high state;
		next_duration = channel->high_duration;


	} else {
		//If the channel must be set low :

		//The next duration is the one corresponding to the high state; This duration can be null;
		next_duration = channel->low_duration;

		//If the duration is null :
		if (!next_duration) {

			//No need to clear the GPIO;

			//Channel will be maintained set at next toggle;
			state = false;

			//The next duration is the one corresponding to the high state;
			next_duration = channel->high_duration;


		} else {

			//Clear the GPIO;
			gpio_clear(gpio, gpio->pin_mask);

		}

	}


	//Flip the state to invert line state at next toggle (except for pwm at 100%, handled previously);
	channel->next_state = !state;

	//update the channel's duration;
	channel->toogle_delay = next_duration;

}


/**
 * channel_insert : inserts a pwm channel in the pwm sorted list;
 *
 * 	As delays are stored relatively, the delay is decreases as the list is traversed;
 *
 * @param channels : the first channel of the list; Cannot be null;
 * @param new_channel : the new channel to insert;
 * @return the new first channel of the list;
 */

static struct channel_data *channel_insert(struct channel_data *first, struct channel_data *const new_channel) {

	//Cache the first element of the list;
	struct channel_data *current_channel = first;

	//Cache the channel's duration;
	uint32_t delay = new_channel->toogle_delay;

	//Initialise the current channel's delay;
	uint32_t c_delay = current_channel->toogle_delay;

	//If the channel must take the first position :
	if (delay < c_delay) {

		//Concatenate both channels;
		list_concat((struct list_head *) new_channel, (struct list_head *) first);

		//Update the delay time of the next channel;
		first->toogle_delay = c_delay - delay;

		//Stop here, insertion complete, new channel becomes the first element of the list;
		return new_channel;

	}

	//While we have not found a channel that must wait less than @new_channel, traverse the list;
	while (1) {

		//Decrement the delay;
		delay -= c_delay;

		//Cache the next element;
		struct channel_data *next = current_channel->head.next;

		//Update the next channel's delay;
		c_delay = current_channel->toogle_delay;

		//Is the traversal terminated ?
		bool end = (next == first);

		//If the delay is lesser than the next channel, or the traversal is complete :
		if ((delay < c_delay) || end) {

			//Concatenate both channels;
			list_concat((struct list_head *) current_channel, (struct list_head *) new_channel);

			//If the next element respects the order relation (not the case for the end -> first link) :
			if (!end) {

				//Update the delay time of the next channel;
				next->toogle_delay = c_delay - delay;

			}

			//Complete;
			return first;

		}

		//If not, keep traversing;
		current_channel = next;

	}

}


//---------------------------------------------------- Channel config ---------------------------------------------------

/**
 * channel_setup : updates a channel's durations according to the required high duration;
 *
 * @param channel : the channel that must be configured;
 * @param high_duration : the required high duration;
 *
 * @return true if the configuration de-activates the channel;
 */

static bool channel_setup(uint8_t channel_id, uint32_t high_duration) {

	//Cache the channel's period;
	const uint32_t period = specs[channel_id]->period;

	//A deactivation happens when the activity duration is lower than the tolerance;
	const bool deactivation = (high_duration < (uint32_t) TOLERANCE);

	//If the channel must be deactivated :
	if (deactivation) {

		//Reset the high duration;
		high_duration = 0;

	}

	//Correct the channel's high_duration, according to the period;
	if (high_duration > period) high_duration = period;

	//Determine the low time;
	uint32_t low_duration = period - high_duration;

	//If the low time is lower than the tolerance :
	if (low_duration < (uint32_t) TOLERANCE) {

		//Force full power;struct channel_data *channel,
		high_duration = period, low_duration = 0;

	}

	//Cache the channel ref :
	struct channel_data *const channel = channels_data + channel_id;

	//Update parameters;
	channel->high_duration = high_duration, channel->low_duration = low_duration;

	//Return the deactivation status;
	return deactivation;

}


/**
 * channel_set_high_duration : updates the channel's duration values, and eventually activates it;
 * 	Deactivation is done by the IRQ;
 *
 * @param channel : the channel whose high duration must be modified;
 * @param high_duration : the new high duration;
 */

void REFERENCE_SYMBOL(MODULE_NAME, update_channel_duration)(uint8_t channel_id, uint32_t high_duration) {

	//Channel durations can be accessed and modified with no restrictions. Setup the channel.
	bool deactivation = channel_setup(channel_id, high_duration);

	//Cache the channel ref :
	struct channel_data *const channel = channels_data + channel_id;

	/*pwm_exit
	 * Changing durations is sufficient in the following cases :
	 * - active and must be activated (setup);
	 * - active and must be deactivated, done by
	 * - inactive and must be deactivated;
	 *
	 * 	The problematic case is the activation of an inactive channel;
	 */

	//Cache the channel's activity state;
	bool inactive = channel->inactive;

	//If the channel must be deactivated :
	if (deactivation) {

		//If the channel is active :
		if (!inactive) {

			//If the channel is the only member of the list, we can stop the manager;
			// a modification of the list will happen.
			// Disable interrupts, to prevent the pwm irq to happen;
			ic_enter_critical_section();

			//If the list is a singleton :
			if (channel->head.next == channel) {

				//Mark the channel inactive;
				channel->inactive = true;

				//Cache the gpio iface;
				struct gpio_if *gpio = &channel->gpio;

				//Turn the GPIO down;
				gpio_clear(gpio, gpio->pin_mask);

				//Stop the ISR;
				stop();

			} else {
				//If other channels are present, simply clear the gpio, to keep timing consistent;

				//Cache the GPIO interface;
				struct gpio_if *gpio = &channel->gpio;

				//Clear the GPIO;
				gpio_clear(gpio, gpio->pin_mask);

			}

			//Leave the critical section;
			ic_leave_critical_section();

		}

	} else {
		//If the channel must be activated :

		//If @channel->inactive is read set, its value is not volatile; No critical section is required;
		//If the channel is inactive :
		if (inactive) {


			//If the channel must be activated, a modification of the list will happen.
			// Disable interrupts, to prevent the pwm irq to happen;
			ic_enter_critical_section();

			//Reset the channel's delay;
			channel->toogle_delay = 0;

			//Set the next state to be active;
			channel->next_state = true;

			//If there are no active channels (ISR stopped) :
			if (!active_channels) {

				//The channel becomes the first active channel;
				active_channels = channel;

				//Start the pwm manager;
				start();

			} else {

				//Insert the channel after the next channel to be toggled;
				list_concat((struct list_head *) active_channels, (struct list_head *) channel);

			}

			//Mark the channel active;
			channel->inactive = false;

			//Leave the critical section;
			ic_leave_critical_section();

		}

	}

}


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

//Inodes can be stored in an array, as their amount is predetermined, and never altered;
static struct channel_inode inodes[NB_CHANNELS];


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
	if ((channel_id >= NB_CHANNELS)) {

		//Log;
		kernel_log_("pwm : channel_interface : invalid index provided;");

		//Fail;
		return false;
	}

	//Allow interfacing. Size check is taken in charge.
	return command_if_interface(if_struct, &specs[channel_id]->iface, &channels_data[channel_id].iface_ref, size);

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
		kernel_log_("pwm : chanel_close : invalid index provided;")

		//Fail;
		return;

	}

	//Neutralise the interface, if interfaced;
	command_if_neutralise(&channels_data[channel_id].iface_ref);

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

static void channel_register(const size_t channel_index) {

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
	fs_create(specs[channel_index]->channel_name, (struct inode *) node);

}


//--------------------------------------------------- Channels setup ---------------------------------------------------


/**
 * init_channel : opens the required gpio, interfaces with it, and initialises the channel data;
 *
 * @return true if the initialisation was made properly. If not, the module should fail;
 */

static bool init_channel(size_t channel_id) {

	//Cache the specs struct;
	const struct channel_specs *channel_specs = specs[channel_id];

	//Cache the gpio file name;
	const char *gpio_name = channel_specs->gpio_name;

	//Open the gpio file;
	file_descriptor gpio_fd = fs_open(gpio_name);

	//If the file does not exist :
	if (!gpio_fd) {

		//Log;
		kernel_log("pwm : %s gpio file not found.", gpio_name);

		//Fail;
		return false;

	}

	//Cache the channel data struct;
	struct channel_data *channel = channels_data + channel_id;

	//Create the initializer for the channel;
	struct channel_data init = {

		//Default the ISR part;
		.head = {.next = channel, .prev = channel,},
		.toogle_delay = 0,
		.inactive = true,
		.next_state = true,

		//--------------- NON ISR PART ---------------

		//Initialised right after;
		.gpio = {},

		//Never active;
		.high_duration = 0,
		.low_duration = channel_specs->period,

		//Save the file descriptor;
		.gpio_fd = gpio_fd,

		//Not interfaced for instance;
		.iface_ref = 0,

	};

	//Interface with the gpio;
	bool success = inode_interface(gpio_fd, &init.gpio, sizeof(struct gpio_if));

	//If the interfacing failed :
	if (!success) {

		//Close the file;
		fs_close(gpio_fd);

		//Log;
		kernel_log("pwm : %s gpio interfacing failed.", gpio_name);

		//Fail;
		return false;

	}

	//Initialise the channel;
	memcpy(channel, &init, sizeof(channels_data));

	//If the interfacing succeeded, complete;
	return true;

}


//----------------------------------------------------- Init - Exit ----------------------------------------------------

static bool pwm_init() {

	/*
	 * Timer interfacing;
	 */

	//Open the timer file;
	file_descriptor fd = fs_open(STR(TIMER_NAME));

	//If the file does not exist :
	if (!fd) {

		//Log;
		kernel_log_("pwm : timer file not found.");

		//Fail;
		return false;

	}

	//Interface with the timer;
	bool success = inode_interface(fd, &pwm_timer, sizeof(struct timer_if));


	//If the interfacing failed :
	if (!success) {

		//Close the file;
		fs_close(fd);

		//Log;
		kernel_log_("pwm : timer interfacing failed.");

		//Fail;
		return false;

	}


	/*
	 * Globals init;
	 */

	//Mark the complement channel active;
	active_channels = 0;

	//Save the file descriptor;
	timer_fd = fd;

	//Mark ISR routine stopped ?;
	started = false;


	/*
	 * Standard interfaces init and registration;
	 */

	//For each regular channel :
	for (uint8_t channel_id = NB_CHANNELS; channel_id--;) {

		init_channel(channel_id);

		channel_register(channel_id);
	}

	//Complete;
	return true;

}

//Embed the module in the executable;
KERNEL_EMBED_SYSTEM_MODULE(MODULE_NAME, &pwm_init);
