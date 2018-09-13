//
// Created by root on 9/12/18.
//

#include <kernel/debug/debug.h>
#include <kernel/log.h>
#include <kernel/mod/auto_mod.h>


#include <kernel/ic.h>

#include <kernel/interface/timer.h>
#include <kernel/interface/gpio.h>



//------------------------------------------------------- Structs ------------------------------------------------------

struct pwm_channel {

	//Channels are stored and sorted; Non IRQ can access only in a critical section;
	struct list_head head;

	//The duration between this pins's toggle and the previous one;
	// Non IRQ can access only when channel is inactive. IRQ has full access;
	uint32_t toogle_delay;

	//A flag set if the channel must be de-activated; Set by IRQ, cleared by non-IRQ;
	bool inactive;

	//The direction of the toogle; Non IRQ can access only when channel is inactive. IRQ has full access;
	bool next_state;

	//The period of the channel; Can't be modified when the pwm is active;
	uint32_t period;

	//The duration of a high state; Can be modified asynchronously to setup the pwm;
	uint32_t high_duration;

	//The duration of a low state; Can be modified asynchronously to setup the pwm;
	uint32_t low_duration;

	//The GPIO pin we interface with; Non IRQ can access only in a critical section;
	struct gpio_interface gpio;

};


struct pwm_manager {

	//The list containing active channels; Non IRQ can access only in a critical section;
	struct pwm_channel *active_list;

	//The list containing inactive channels; Non IRQ can access only in a critical section;
	struct pwm_channel *inactive_list;

	//The duration tolerance, in microseconds. Toggles that are distanced with less than this duration will be merged;
	// Modification is only allowed when the module is stopped;
	uint32_t tolerance;

	//The timer we use;
	// Modification is only allowed when the module is stopped;
	struct timer_interface timer;

};


//------------------------------------------------------- Headers ------------------------------------------------------

//Start the pwm routine;
void pwm_start();

//Stop the pwm routine; Timer will be reset, all channels will be inactive;
void pwm_stop();


//------------------------------------------------------- Globals ------------------------------------------------------

//The pwm manager;
struct pwm_manager manager;

//TODO PREDEFINE CHANNELS;

//---------------------------------------------------- Channel config ---------------------------------------------------

/**
 * channel_setup : updates a channel's durations according to the required high duration;
 *
 * @param channel : the channel that must be configured;
 * @param high_duration : the required high duration;
 *
 * @return true if the configuration de-activates the channel;
 */

static bool channel_setup(struct pwm_channel *channel, uint32_t high_duration) {

	//Cache the manager's tolerance;
	uint32_t tolerance = manager.tolerance;

	//If the high_duration is lower than the tolerance, force deactivation;
	bool deactivation = (high_duration <= tolerance);

	//If the channel must be deactivated, reset the high duration;
	if (deactivation) {
		high_duration = 0;
	}

	//Cache the channel's period;
	uint32_t period = channel->period;

	//Correct the channel's high_duration, according to the period;
	if (high_duration > period) high_duration = period;

	//Determine the low time;
	uint32_t low_duration = period - high_duration;

	//If the low time is lower than the tolerance, force full power;
	if (low_duration < tolerance) {
		high_duration = period, low_duration = 0;
	}

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
static void channel_set_high_duration(struct pwm_channel *channel, uint32_t high_duration) {

	//Channel durations can be accessed and modified with no restrictions. Setup the channel.
	bool deactivation = channel_setup(channel, high_duration);


	/*
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

			//Cache the GPIO interface;
			struct gpio_interface *gpio = &channel->gpio;

			//Clear the GPIO;
			gpio_clear(gpio, gpio->pin_mask);

		}

	} else {
		//If the channel must be activated :

		//If @channel->inactive is read set, its value is not volatile; No critical section is required;
		//If the channel is inactive :
		if (inactive) {

			//If the channel must be activated, a modification of the list will happen.
			// Disable interrupts, to prevent the pwm irq to happen;
			ic_enter_critical_section();

			//Remove the list from the inactive list;
			list_remove_ref_next((struct list_head *) channel, (struct list_head **) manager.inactive_list);

			//Reset the channel's delay;
			channel->toogle_delay = 0;

			//Set the next state to be active;
			channel->next_state = true;

			//Insert the channel after the next channel to be toggled;
			list_concat((struct list_head *) manager.active_list, (struct list_head *) channel);

			//Mark the channel active;
			channel->inactive = false;

			//Leave the critical section;
			ic_leave_critical_section();

		}

	}

}


/**
 * channel_set_period : updates the channel's period;
 *
 * 	Can only be executed when the channel's high duration is os 0 (current or future deactivation);
 *
 * @param channel : the channel whose period must be modified;
 * @param period : the new period;
 */

static void channel_set_period(struct pwm_channel *channel, uint32_t period) {

	//If the channel is active :
	if (channel->high_duration) {

		//Log;
		kernel_log_("Attempted to change an active pwm channel's period. Aborted.");

		//Fail;
		return;

	}

	//Modify the period;
	channel->period = period;

}



//------------------------------------------------------- PWM irq ------------------------------------------------------

/**
 * toggle_channel : toggles the provided channel, and updates its next state, and its next duration;
 *
 * @param channel : the channel to toggle;
 */

static void toggle_channel(struct pwm_channel *channel) {

	//Cache the GPIO interface;
	struct gpio_interface *gpio = &channel->gpio;

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

			//The next duration is the one corresponding to the high state;
			next_duration = channel->high_duration;

		}

	}

	//Flip the state to invert line state at next toggle (except for pwm at 100%, handled previously);
	state = !state;

	//Update the channel's next state;
	channel->next_state = state;

	//update the channel's duration;
	channel->toogle_delay = next_duration;

}


/**
 * channel_insert : inserts a pwm channel in the pwm sorted list;
 *
 * 	As delays are stored relatively, the delay is decreases as the list is traversed;
 *
 * @param channels : the channels list; can be null if the list is empty;
 * @param n_channel : the new channel to insert;
 */

static void channel_insert(struct pwm_channel **channels, struct pwm_channel *const n_channel) {

	//Cache the first element of the list;
	struct pwm_channel *const first = *channels, *channel = first;

	//If no more channels are present :
	if (!first) {

		//@n_channel becomes the only channel in the list;
		*channels = n_channel;

		//Stop here, insertion complete;
		return;
	}

	//Cache the channel's duration;
	uint32_t delay = n_channel->toogle_delay;

	//Initialise the current channel's delay;
	uint32_t c_delay = channel->toogle_delay;

	//If the channel must take the first position :
	if (delay < c_delay) {

		//Concatenate both channels;
		list_concat((struct list_head *) n_channel, (struct list_head *) first);

		//Update the delay time of the next channel;
		first->toogle_delay = c_delay - delay;

		//@n_channel becomes the first channel of the list;
		*channels = n_channel;

		//Stop here, insertion complete;
		return;

	}

	//While we have not found a channel that must wait less than @n_channel, traverse the list;
	while(1) {

		//Decrement the delay;
		delay -= c_delay;

		//Cache the next element;
		struct pwm_channel *next = channel->head.next;

		//Update the next channel's delay;
		c_delay = channel->toogle_delay;

		//Is the traversal terminated ?
		bool end = (next == first);

		//If the delay is lesser than the next channel, or the traversal is complete :
		if ((delay < c_delay) || end) {

			//Concatenate both channels;
			list_concat((struct list_head *) channel, (struct list_head *) n_channel);

			//If the next element respects the order relation (not the case for the end -> first link) :
			if (!end) {

				//Update the delay time of the next channel;
				next->toogle_delay = c_delay - delay;

			}

			//Complete;
			return;

		}

		//If not, keep traversing;
		channel = next;

	}

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

static void pwm_irq() {

	//Cache the active list;
	struct pwm_channel **active_list = &manager.active_list;

	//Cache the first pwm of the active list;
	struct pwm_channel *channel = manager.active_list;

	//If the list is empty :
	if (!channel) {
		pwm_stop();
	}

	//Cache the time to re-interruption;
	uint32_t delay = 0;


	//Cache the pwm tolerance;
	const uint32_t tolerance = manager.tolerance;

	//We will toggle all channels until the delay becomes greater than the tolerance;
	do {

		//Cache the next channel;
		struct pwm_channel *next = channel->head.next;

		//Is the list a singleton ?
		bool singleton = channel == next;

		//Remove the channel from the active list;
		list_remove((struct list_head *) channel);

		//If the channel must be stopped :
		if (!channel->high_duration) {

			//Insert the channel in the inactive list;
			list_concat_ref((struct list_head *) channel, (struct list_head **) &manager.inactive_list);
			
			//Mark the channel de-activated;
			channel->inactive = true;

			//If the channel was the only member of the list :
			if (singleton) {

				//Stop the pwm manager;
				pwm_stop();

				//Complete here, nothing to do;
				return;

			}

			//If the struct was not a singleton, keep traversing;

		} else {

			//Toggle the channel; delay time until toggle is placed in channel->toogle_delay, next state is updated;
			toggle_channel(channel);

			//If the list was not a singleton :
			if (!singleton) {

				//Re-insert the channel;
				channel_insert(&next, channel);

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
	const struct timer_interface *iface = &manager.timer;

	//Update the timer's period to interrupt;
	timer_set_ovf_value(iface, delay);

	//Clear the timer ovf flag;
	timer_flag_clr(iface);

}


//----------------------------------------------------- Init - Exit ----------------------------------------------------

static void pwm_init() {

	struct tlist *e = list_merge(&b, &a);

	tlist_print(e);

	struct tlist *f = list_merge(&c, &d);

	tlist_print(f);

	struct tlist *g = list_merge(e, f);

	tlist_print(g);

	while (1);

}


static bool pwm_exit() {

	return true;

}

KERNEL_EMBED_USER_MODULE(pwm, &pwm_init, &pwm_exit);

