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


#include <stdint.h>

#include "kx_sim.h"


#include <kernel/timer/timer.h>
#include <kernel/mod/auto_mod.h>

/*
//Prerequisites

#define __MCR__ (void *)0x40037000
#define __NB_CHANNELS__ 4
*/

#define    MCR            ((volatile uint32_t *) __MCR__)
#define    NB_CHANNELS        ((uint8_t)__NB_CHANNELS__)


#define MULTI_MACRO_0(x)
#define MULTI_MACRO_1(x) x(0)
#define MULTI_MACRO_2(x) x(0) x(1)
#define MULTI_MACRO_3(x) x(0) x(1) x(2)
#define MULTI_MACRO_4(x) x(0) x(1) x(2) x(3)
#define MULTI_MACRO_5(x) x(0) x(1) x(2) x(3) x(4)
#define MULTI_MACRO_6(x) x(0) x(1) x(2) x(3) x(4) x(5)
#define MULTI_MACRO_7(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6)
#define MULTI_MACRO_8(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7)
#define MULTI_MACRO_9(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7) x(8)
#define MULTI_MACRO_10(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7) x(8) x(9)


#define CAT(a, b) a##b

#define MULTI_MACRO(i, x) CAT(MULTI_MACRO_,i)(x)


/*
 * The PIT module interacts with a PIT peripheral.
 *
 * 	The PIT peripheral provides an array of independently configurable general-purpose timers;
 *
 * 	The PIT driver offers a timer interface for each one of there channels. It will register these structs to the
 * 	kernel at init, and un-register them at exit;
 */

//CHANNEL_DECLARE will declare the channel struct and the channel init function;
#define CHANNEL_DECLARE(i) \
	struct timer_interface kx_pit_channel_##i;\
	void kx_pit_channel_init_##i();\
	bool kx_pit_channel_exit_##i();

//Declare each channel;
MULTI_MACRO(__NB_CHANNELS__, CHANNEL_DECLARE);

//Macro not used anymore;
#undef CHANNEL_DECLARE



//INIT_ARRAY will print the reference of the i-th timer channel;
#define INIT_ARRAY(i) &kx_pit_channel_##i,

//Initialize the interfaces array;
struct timer_interface *interfaces[NB_CHANNELS] = {MULTI_MACRO(__NB_CHANNELS__, INIT_ARRAY)};

//Macro not used anymore;
#undef INIT_ARRAY


static void init() {

	//Enable PIT clock gating;
	sim_enable_PIT_clock_gating();

	//Enable clocks for all PITs;
	*MCR = 0x00;

	//TIMER_INIT calls a channel's initialisation function;
	#define TIMER_INIT(i) kx_pit_channel_init_##i();

	//Initialise each channel;
	MULTI_MACRO(__NB_CHANNELS__, TIMER_INIT);

	//Macro not used anymore;
	#undef TIMER_INIT
}


/**
 * kx_pit_delete : calls exit, and delete the driver struct;
 *
 * @param driver : the driver to delete;
 */

static bool exit() {

	//TIMER_INIT calls a channel's exit function;
	#define TIMER_EXIT(i)\
		 {if (!kx_pit_channel_exit_##i()) \
				return false;}\

	//Initialise each channel;
	MULTI_MACRO(__NB_CHANNELS__, TIMER_EXIT);

	//Macro not used anymore;
	#undef TIMER_EXIT


	//If all channels have been cleaned up properly :

	//Disable clocks for all PITs;
	*MCR = 0x02;

	//Disable clock gating;
	sim_disable_PIT_clock_gating();

	//Complete;
	return true;

}


//Embed the PIT module in the executable;
KERNEL_EMBED_MODULE("pit", &init, &exit);