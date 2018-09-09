//
// Created by root on 9/5/18.
//


#include <stdint.h>

#include <stdbool.h>

#include <kernel/timer/timer.h>

#include <kernel/fs/dfs.h>

#include <kernel/ic.h>

#include <kernel/mod/make_args.h>




/*
 * This file contains code for a single PIT channel. It must be compiled for each channel. 
 * 
 * 	This file takes macros parameters generated by the makefile, namely : 
 * 	- CHANNEL_ID : the identifier of the PIT, supposedly an integer between 0 and NB_CHANNELS-1
 * 	- __REGISTERS_START__ : the address of the first channel's register area;
 * 	- __REGISTERS_SPACING__ : the address space between two channels register areas;
 * 	
 * 	A PIT registers follows the map below :
 * 	
 * 	{
 * 		//The Load Value;
 * 		volatile uint32_t LDVAL;
 * 	
 * 		//Current Value;
 * 		volatile uint32_t CVAL;
 * 
 * 		//Timer Control Register
 * 		volatile uint32_t TCTRL;
 * 
 * 		//Timer Flag;
 * 		volatile uint32_t TFLG;
 * 
 *	 };
 * 
 */



#ifndef MAKE_ARGS
#define MAKE_ARGS 0,0,0,
#endif

#define MAKE_ARGS 0,0,0,



/*
 * Make attributes :
 * 	- 0 : Channel identifier;
 * 	- 1 : interrupt channel;
 * 	- 2 : start of the channel's registers area;
 */

//TODO MAKE ARGS:

#define CHANNEL_ID 	0
#define INT_CHANNEL MAKE_ARG(1)
#define REGISTERS ((volatile uint32_t *)(MAKE_ARG(2)))


#define LDVAL 	((volatile uint32_t *) (REGISTERS))
#define CVAL 	((volatile uint32_t *) (REGISTERS + 1))
#define TCTRL	((volatile uint32_t *) (REGISTERS + 2))
#define TFLG 	((volatile uint32_t *) (REGISTERS + 3))

//Enable the timer;
#define TCTRL_TEN	((uint32_t)(1<<0))

//Enable the interrupt;
#define TCTRL_TIE 	((uint32_t)(1<<1))

//Timer interrupt flag. W1C
#define TFLG_TIF (1<<0)




//Close the timer resource : will neutralise the interface;
static void close();

//Transmit the timer interface;
static bool interface(void *data, size_t data_size);


static struct dfs_file_operations file_operations = {
	.close = &close,
	.interface = &interface,
};

//-------------------------------------------------- Timer Operations --------------------------------------------------

/*
 * All variables and methods in this section are private. No one can access them outside this translation unit;
 */


//The ratio tics/period;
static float period_to_tics;

//The maximal period;
static float max_period;


//TODO BUS CLOCK;

static void set_base_frequency(uint32_t frequency) {

	//Determine the tics/period ratio;
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);
	//TODO:::::::: float _period_to_tics = ((float) iface->clock_frequency) / ((float) frequency);

	float _period_to_tics = ((float) 24000000) / ((float) frequency);

	//Determine the maximal period;
	float _max_period = ((float) (uint32_t) -1) / period_to_tics;

	//Save both constants;
	period_to_tics = _period_to_tics;
	max_period = _max_period;

}


//Enable the timer;
static void enable() {

	//Set TEN;
	(*TCTRL) |= TCTRL_TEN;
}


//Disable the timer;
static void disable() {

	//Clear TEN;
	*TCTRL &= ~TCTRL_TEN;
}


//Is the timer enabled ?
static bool enabled() {

	//Return the state of TCTRL_TEN;
	return (bool) (*TCTRL & TCTRL_TEN);

}


//Update the reload value;
static void set_count(float period_count) {

	if (period_count < max_period) {
		*CVAL = (uint32_t) (period_to_tics * period_count);
	}

}

//Get the current value;
static float get_count() {
	return (((float) *CVAL) / period_to_tics);
}


//Update the reload value;
static void set_ovf(float period_count) {

	if (period_count < max_period) {
		*LDVAL = (uint32_t) (period_to_tics * period_count);
	}
}

//Get the reload value;
static float get_ovf() {
	return (((float) *LDVAL) / period_to_tics);
}


//Enable the interrupt;
static void enable_int() {
	//Set bit 1 of TCTRL;
	*TCTRL |= TCTRL_TIE;

}

//Disable the interrupt;
static void disable_int() {
	//Clear bit 1 of TCTRL;
	*TCTRL &= ~TCTRL_TIE;
}

//Is the interrupt enabled ?
static bool int_enabled() {

	//Is bit 1 of TCTRL set ?
	return (bool) (*TCTRL & TCTRL_TIE);

}


//Update the handler;
static void update_handler(void (*handler)()) {

	//If the handler is not null :
	if (handler) {

		//Update the handler;
		ic_set_interrupt_handler((uint16_t)INT_CHANNEL, handler);

		//Enable the interrupt;
		ic_enable_interrupt((uint16_t)INT_CHANNEL);

	} else {

		//If the handler is null :

		//Disable the interrupt;
		ic_disable_interrupt((uint16_t)INT_CHANNEL);

		//Reset the default handler;
		ic_reset_interrupt_handler((uint16_t)INT_CHANNEL);

	}

}


//Is the flag set ?
static bool flag_set() {
	//Is bit 0 of TFLG set ?
	return (bool) (*TFLG);
}


//Clear the interrupt flag;
static void clear_flag() {
	//Set bit 0 of TFLG;
	*TFLG = 1;
}



//---------------------------------------------------- Hidden code -----------------------------------------------------

/*
 * All variables and methods in this section are hidden. No one can access them outside the PIT library;
 */

/*
 * This file is compiled once for each instance of the timer channel. Its non-static symbols can't have the same name;
 *
 * 	When referring one of its public symbols, the macro NM will be used. It takes the generic name, and concatenates
 * 	the instance index, so that all channels have different methods;
 */



#define CCAT(a, b, ...) a##b##__VA_ARGS__

#define NM(name) CCAT(name,_,CHANNEL_ID)


/*
 * The timer interface;
 */

struct timer_interface NM(kx_pit_channel) __attribute__((visibility("hidden"))) = {

	.set_base_frequency = &set_base_frequency,

	.start = &enable,
	.stop = &disable,
	.started = &enabled,

	.set_count = &set_count,
	.get_count = &get_count,

	.set_ovf_value = &set_ovf,
	.get_ovf_value = &get_ovf,

	.enable_int = &enable_int,
	.disable_int = &disable_int,
	.int_enabled = &int_enabled,
	.update_handler = &update_handler,

	.int_flag_set = &flag_set,
	.int_flag_clr = &clear_flag,

};


//Two macros to determine the string name;
#define STR(i) #i
#define SNM(name) STR(NM(name))

//Create the name;
const char *const pit_name = SNM(pit_);

//No more use for these macros;
#undef STR
#undef SNM


/**
 * 	kx_pit_channel_init_i : initialises the channel; Called by the pit module's init function
 */
void NM(kx_pit_channel_init)() {

	//Register a file with no content leading to our operations;
	dfs_create(pit_name, DFS_INTERFACE, &file_operations, 0);

	//Initialise the timer;
	timer_init(&NM(kx_pit_channel), 1000);

	//TODO IC SETUP (PRIO, HANDLER);
}




/**
 * 	kx_pit_channel_exit_i : initialises the channel; Called by the pit module's init function
 */
bool NM(kx_pit_channel_exit)() {gi

	//Attempt to remove the file;
	if (dfs_remove(pit_name)) {

		//If success, reset the timer;
		timer_reset(&NM(kx_pit_channel));

		//Complete;
		return true;

		//TODO IC CLEANUP (PRIO, HANDLER);

	}

	//If removal failed, fail;
	return false;

}

#undef NM

//-------------------------------------------------- File Operations --------------------------------------------------


/*
 * All variables and methods in this section are private. No one can access them outside this translation unit;
 */

