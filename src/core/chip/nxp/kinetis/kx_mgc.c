//
// Created by root on 8/23/18.
//

#include "kx_mgc.h"

#include <stdint.h>

#define MCG_C1 			((uint8_t *)0x40064000)
#define C1_CLKS		 	((uint8_t)0xC0)
#define C1_CLKS_FPLL 	((uint8_t)0xC0)
#define C1_CLKS_INT 	((uint8_t)0xD0)
#define C1_CLKS_EXT 	((uint8_t)0xE0)


#define MCG_C2 		((uint8_t *)0x40064001)
#define C2_IRCS		((uint8_t) (1<<0))


#define MCG_C3 ((uint8_t *)0x40064002)
#define MCG_C4 ((uint8_t *)0x40064003)
#define MCG_C5 ((uint8_t *)0x40064004)


#define MCG_C6 ((uint8_t *)0x40064005)
#define C6_PLLS ((uint8_t) (1<<6))

//Read only;
#define MCG_S ((uint8_t *)0x40064006)

#define MCG_SC 		((uint8_t *)0x40064008)
#define SC_FCRDIV	((uint8_t) 0x0E)

#define MCG_ATCVH ((uint8_t *)0x4006400A)
#define MCG_ATCVL ((uint8_t *)0x4006400B)

#define MCG_C7 ((uint8_t *)0x4006400C)
#define MCG_C8 ((uint8_t *)0x4006400D)



/*
 * -------------------------------------------------------- IRC --------------------------------------------------------
 */

//The current frequency of the IRC clock. At init, the slow clock is selected;
static uint32_t irc_frequency = IRC_SLOW_FREQ;


/**
 * mcg_configure_irc : configures the IRC;
 *
 * 	The slow or fast clock can be selected, and a divider can be set for the fast clock;
 *
 * 	Because the fast clock can't be set up when it is selected, the slow clock is selected when the function begins;
 *
 * 	The irc clock frequency global is updated according to the config.
 *
 * @param config : the required configuration;
 */

void mcg_configure_irc(const struct mcg_irc_config *config) {

	/*
	 * Select the slow internal clock;
	 *
	 * This operation is made in both cases, as the alteration of the fast clock divider is forbidden when
	 * 	the fast clock is selected;
	 */

	//Select the slow internal clock;
	*MCG_C2 &= ~C2_IRCS;

	//Set IRC freq to the slow frequency; Leaves times for IRCS sync and maintains integrity;
	irc_frequency = IRC_SLOW_FREQ;

	//If the fast IRC must be selected :
	if (config->fast_IRC_selected) {

		//Cache the index of the divider (log2(divider));
		uint8_t divisor = config->fast_irc_divider;

		//Zero unused divisor bits for safety;
		divisor &= (uint8_t) 0x7;

		//Get SC value;
		uint8_t SC = *MCG_SC;

		//Reset FCRDIV value;
		SC &= ~SC_FCRDIV;

		//Update the FRDIV value;
		SC |= divisor << 1;

		//Save SC;
		*MCG_SC = SC;

		//Determine the value of the IRC clock. The divisor is a power of 2 that we know the log2 of;
		irc_frequency = IRC_FAST_FREQ >> divisor;

		//Select the fast internal clock;
		*MCG_C2 |= C2_IRCS;

	}

}