/*
  pwm.c Part of TRACER

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



//-------------------------------------------------- Module array head -------------------------------------------------

#include <kernel/exec/module_array.h>


//------------------------------------------------------- Headers ------------------------------------------------------



#include <stdmem.h>

#include "pwm.h"

#include <fs/iinode.h>
#include <if/gpio.h>
#include <if/timer.h>
#include <kernel/exec/mod_hook>
#include <if/port.h>
#include <debug/printk.h>

#include "pwm_ch.h"


//------------------------------------------------- Channels const data ------------------------------------------------

MODULE_CREATE_SPECS_ARRAY(channels_specs)


//------------------------------------------------------- Structs ------------------------------------------------------

struct channel_data {
	
	//Status;
	enum dev_status status;
	

	//---------------- init ----------------

	//The timer file descriptor;
	file_descriptor timer_fd;

	//The gpio if; Non IRQ can access only in a critical section;
	struct timer_if timer;

	//The gpio file descriptor;
	file_descriptor gpio_fd;

	//The gpio if; Non IRQ can access only in a critical section;
	struct gpio_if gpio;

	//The ref of the struct interfaced with the channel;
	struct cmd_if *iface_ref;


	//---------------- IRQ data ----------------

	//The current state of the GPIO; Non IRQ can access only when channel is inactive. IRQ has full access;
	bool next_state;


	//The duration of a high state; Can be modified asynchronously to setup the pwm;
	//A high duration of 0 is equivalent to the ISR routine stopped;
	uint32_t high_duration;

	//The duration of a low state; Can be modified asynchronously to setup the pwm;
	uint32_t low_duration;
	
	//The pwm's period, in timer tics;
	uint32_t period;
	
	//The timer tolerance, in timer tics;
	uint32_t tolerance;


};


//The array of channel data;
static struct channel_data channels_data[NB_CHANNELS];


//------------------------------------------------------ PWM setup -----------------------------------------------------

/**
 * start : setups the isr environment, and starts the timer;
 */

static void start(const uint8_t channel_index) {

	//Cache the channel data ref;
	struct channel_data *channel = channels_data + channel_index;

	//If the IRQ routine is already started, fail;
	if (channel->high_duration) {
		return;
	}

	//Set next toggle to ascending;
	channel->next_state = true;

	//Initialise the timer : 1MHz base, ovf immediately;
	timer_init(&channel->timer, 1, channels_specs[channel_index]->isr_handler);

}


/**
 * start : stops the timer;
 */

static void stop(const uint8_t channel_index) {
	
	//Cache the channel data ref;
	struct channel_data *channel = channels_data + channel_index;

	//If the channel is already stopped, complete;
	if (!channel->high_duration) return;

	//Reset the timer;
	timer_reset(&channel->timer);

	//Reset the channel's data;
	channel->high_duration = 0;
	channel->low_duration = channel->period;
	channel->next_state = true;

	//Cache the channel's gpio;
	struct gpio_if *gpio = &channel->gpio;

	//Clear the channel's gpio;
	gpio_clear(gpio, gpio->pin_mask);


}


/**
 * pwm_isr : TODO
 */

void REFERENCE_SYMBOL(MODULE_NAME, isr_handler)(const uint8_t channel_index) {

	
	//Cache the channel data;
	struct channel_data *channel = channels_data + channel_index;

	//Cache the GPIO if;
	struct gpio_if *gpio = &channel->gpio;

	//Declare the next duration;
	uint32_t delay;

	//Cache the state to put the GPIO on;
	bool state = channel->next_state;

	//If the channel must be set high :
	if (state) {
		
		//Set the GPIO;
		gpio_set(gpio, gpio->pin_mask);

		//The next duration is the one corresponding to the high state;
		delay = channel->high_duration;


	} else {
		//If the channel must be set low :

		//The next duration is the one corresponding to the high state; This duration can be null;
		delay = channel->low_duration;

		//If the duration is null :
		if (!delay) {

			//No need to clear the GPIO;

			//Channel will be maintained set at next toggle;
			state = false;

			//The next duration is the one corresponding to the high state;
			delay = channel->high_duration;


		} else {
			
			//Clear the GPIO;
			gpio_clear(gpio, gpio->pin_mask);

		}

	}

	//Flip the state to invert line state at next isr (except for pwm at 100%, handled previously);
	channel->next_state = !state;

	//Cache the channel's timer if;
	struct timer_if *timer = &channel->timer;

	//Update the timer's period to interrupt;
	timer_set_int_period(timer, delay);

	//Clear the timer ovf flag;
	timer_flag_clr(timer);


}


void REFERENCE_SYMBOL(MODULE_NAME, disable_channel)(uint8_t channel_id) {

	stop(channel_id);

}


/**
 * channel_setup : updates a channel's durations according to the required high duration;
 *
 * @param channel : the channel that must be configured;
 * @param high_duration : the required high duration;
 *
 * @return true if the configuration de-activates the channel;
 */

void REFERENCE_SYMBOL(MODULE_NAME, update_channel_duration)(uint8_t channel_id, uint32_t high_duration) {


	//Cache the channel's data;
	struct channel_data *const channel = channels_data + channel_id;

	//Cache the channel's period and tolerance;
	const uint32_t period = channel->period;
	const uint32_t tolerance = channel->tolerance;

	//If the channel must be stopped :
	if (high_duration < tolerance) {

		//Stop it;
		stop(channel_id);

	} else {

		//Correct the channel's high_duration, according to the period;
		if (high_duration > period) high_duration = period;

		//Determine the low time;
		uint32_t low_duration = period - high_duration;

		//If the low time is lower than the tolerance :
		if (low_duration < tolerance) {

			//Force full power;struct channel_data *channel,
			high_duration = period, low_duration = 0;

		}

		//If the channel is stopped :
		if (!channel->high_duration) {

			//Start it;
			start(channel_id);

		}

		//Update parameters;
		channel->high_duration = high_duration, channel->low_duration = low_duration;


	}

}


//----------------------------------------------- File system interaction ----------------------------------------------

/*
 * The channel iinode is only used for interfacing. Only Interfacing data will be stored inside;
 */

struct channel_inode {

	//The iinode base;
	struct iinode node;

	//The channel index;
	uint8_t channel_index;

};

//Inodes can be stored in an array, as their amount is predetermined, and never altered;
static struct channel_inode inodes[NB_CHANNELS];


/**
 * init_channel : opens the required gpio, interfaces with it, and initialises the channel data;
 *
 * @return true if the initialisation was made properly. If not, the module should fail;
 */

static bool channel_init(const struct channel_inode *const node, struct pwm_config *cfg, size_t cfg_size) {
	
	//Size check;
	if (cfg_size != sizeof(struct pwm_config))
		return false;
	
	//Cache the channel's id;
	uint8_t channel_id = node->channel_index;
	
	
	//If the channel id is invalid :
	if ((channel_id >= NB_CHANNELS)) {
		
		//Fail;
		return false;
		
	}
	
	//Cache the channel data struct ref;
	struct channel_data *data = channels_data + channel_id;
	
	
	//If the channel is not reset :
	if (data->status != DEV_RESET) {
		
		//Fail;
		return false;
		
	}
	
	
	
	//Cache the specs struct;
	const struct channel_specs *channel_specs = channels_specs[channel_id];
	
	//Cache gpio and timer file name;
	const char *gpio_name = cfg->gpio_name;
	const char *timer_name = cfg->timer_name;
	
	//Open both files;
	file_descriptor gpio_fd = fs_open(gpio_name);
	file_descriptor timer_fd = fs_open(timer_name);
	
	//If the file does not exist :
	if (!gpio_fd) {
		
		//Log;
		printkf("pwm : %s gpio file not found.", gpio_name);
		
		//Fail;
		return false;
		
	}
	
	//If the file does not exist :
	if (!timer_fd) {
		
		//Log;
		printkf("pwm : %s timer file not found.", timer_name);
		
		//Fail;
		return false;
		
	}
	
	//Define the status variable;
	uint8_t status = 0;
	
	
	//configure;
	status |= !iop_init(gpio_fd, cfg->gpio_config, cfg->gpio_config_size);
	status <<= 1;
	
	status |= !iop_init(timer_fd, cfg->timer_config, cfg->timer_config_size);
	status <<= 1;
	
	//Create the initializer for the channel;
	struct channel_data init = {
		
		//Status;
		.status = DEV_INITIALISED,
		
		
		//--------- Init data ---------
		
		//Save the file descriptor;
		.gpio_fd = gpio_fd,
		
		//Initialised right after;
		.gpio = {0},
		
		//Save the file descriptor;
		.timer_fd = gpio_fd,
		
		//Initialised right after;
		.timer = {0},
		
		//Not interfaced for instance;
		.iface_ref = 0,
		
		
		//--------- IRQ data ---------
		
		.next_state = true,
		
		//Not active;
		.high_duration = 0,
		
		//Initialised right after;
		.low_duration = 0,
		.period = 0,
		.tolerance = 0,
		
	};
	
	
	//Interface with the gpio and the timer;
	status |= !iop_interface(gpio_fd, &init.gpio, sizeof(struct gpio_if));
	status <<= 1;
	
	status |= !iop_interface(timer_fd, &init.timer, sizeof(struct timer_if));
	
	//printkf("status : %d", status);
	
	//If the interfacing failed :
	if (status) {
		
		//Close files;
		fs_close(gpio_fd);
		fs_close(timer_fd);
		
		if (status & 0x01) {
			
			printk("pwm : timer interfacing failed.");
			
		}
		
		if (status & 0x02) {
			
			//Log;
			printk("pwm : gpio interfacing failed.");
			
		}
		
		if (status & 0x04) {
			
			//Log;
			printk("pwm : timer init failed.");
			
		}
		
		
		if (status & 0x08) {
			
			//Log;
			printk("pwm : gpio init failed.");
			
		}
		
		//Fail;
		return false;
		
	}
	
	//Update the frequency of the timer;
	timer_update_frequency(&init.timer);
	
	//Compute the maximal timer period;
	init.period = init.low_duration = timer_convert_to_tics(&init.timer, 1000000, channel_specs->period_us);
	init.tolerance = timer_convert_to_tics(&init.timer, 1000000, channel_specs->tolerance_us);
	
	//Initialise the channel;
	memcpy(data, &init, sizeof(struct channel_data));
	
	//If the initialisation succeeded, complete;
	return true;
	
}


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
	struct cmd_if *const if_struct,
	const size_t size

) {
	
	//Size check;
	if (size != sizeof(struct cmd_if))
		return false;

	//The channel iinode contains the identifier of the channel to if;
	const uint8_t channel_id = node->channel_index;

	//If the channel id is invalid :
	if ((channel_id >= NB_CHANNELS)) {
		
		//Fail;
		return false;
	}
	
	//Cache the channel's data ref;
	struct channel_data *dyn = &channels_data[channel_id];
	
	//If the channel is not initialised :
	if (dyn->status != DEV_INITIALISED) {
		
		//Fail;
		return false;
		
	}

	//Allow interfacing. Size check is taken in charge.
	bool success = iface_connect(if_struct, &channels_specs[channel_id]->iface, &dyn->iface_ref);
	
	//If the connection went well :
	if (success) {
		
		//Mark the channel interfaced;
		dyn->status = DEV_INTERFACED;
		
	}
	
	//Return the connection status;
	return success;

}


/**
 * channel_reset : stops the channel, closes all files, and neutralises the interface;
 * @param node
 */

static void channel_reset(const struct channel_inode *const node) {
	
	//Cache the channel's index;
	uint8_t channel_id = node->channel_index;
	
	//If the channel id is invalid :
	if (channel_id >= NB_CHANNELS) {
		
		//Log;
		printk("pwm : chanel_close : invalid index provided;")
		
		//Fail;
		return;
		
	}
	
	//Cache the channel's data ref;
	struct channel_data *data = &channels_data[channel_id];
	
	//If the channel is reset, nothing to do;
	if (data->status == DEV_RESET) {
		
		//Complete;
		return;
		
	}
	
	//Stop the channel;
	stop(channel_id);
	
	//Neutralise the if, if interfaced;
	iface_disconnect(&channels_data[channel_id].iface_ref, &neutral_cmd_if);
	
	//Close files;
	fs_close(data->gpio_fd);
	fs_close(data->timer_fd);
	
}


/*
 * Servo channel inodes accept following calls. Cast is required in order to downcast inodes into channel_inodes. Safe;
 */

static const struct inode_ops channel_ops = {
	.init = (bool (*)(struct iinode *, const void *, size_t)) &channel_init,
	.interface = (bool (*)(struct iinode *, void *, size_t)) &channel_interface,
	.reset = (void (*)(struct iinode *)) &channel_reset,

};


//----------------------------------------------------- Init - Exit ----------------------------------------------------

/**
 * channel_register : registers the channel to the file system. Inode must be initialised;
 *
 * @param channel_name
 * @param channel_index
 */

static void channel_register(const uint8_t channel_index) {

	//Create the iinode initializer;
	struct channel_inode init = {

		//Initialise the node base, providing channel operations;
		.node = INODE(&channel_ops),

		//Save the node index;
		.channel_index = channel_index,
		
	};
	
	//Cache the iinode ref;
	struct channel_inode *node = inodes + channel_index;

	//Initialise the channel;
	memcpy(node, &init, sizeof(struct channel_inode));

	//Register the iinode to the file system; Safe up-cast;
	fs_create(channels_specs[channel_index]->channel_name, (struct iinode *) node);

}


static bool pwm_init() {
	
	//For each channel :
	for (uint8_t channel_id = NB_CHANNELS; channel_id--;) {
		
		//Mark the channel uninitialised;
		channels_data[channel_id].status = DEV_RESET;

		//Register the channel in the file system;
		channel_register(channel_id);
		
	}

	//Complete;
	return true;

}


//Embed the module in the executable;
KERNEL_HOOK_MODULE(KERNEL_MODULE, MODULE_NAME, &pwm_init)

