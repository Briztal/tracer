/*
  kx_irc.c Part of TRACER

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


#include "kx_mcg.h"



const uint32_t irc_frequencies[NB_IRC_CHANNELS] = {
	
	IRC_SLOW_FREQ,
	
	IRC_FAST_FREQ,
	
	IRC_FAST_FREQ / 2,
	
	IRC_FAST_FREQ / 4,
	
	IRC_FAST_FREQ / 8,
	
	IRC_FAST_FREQ / 16,
	
	IRC_FAST_FREQ / 32,
	
	IRC_FAST_FREQ / 64,
	
	IRC_FAST_FREQ / 128,
	
};


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

void kx_irc_configure(uint8_t channel_id) {
	
	/*
	 * Select the slow internal clock;
	 *
	 * This operation is made in both cases, as the alteration of the fast clock divider is forbidden when
	 * 	the fast clock is selected;
	 */
	
	//Select the slow internal clock;
	*MCG_C2 &= ~C2_IRCS;
	
	//If channel 0 had to be selected, or if the channel id is invalid :
	if ((!channel_id) || (channel_id >= NB_IRC_CHANNELS))
		return;
	
	//Decrease the channel ID to get the divisor;
	channel_id--;
	
	//Get SC value;
	uint8_t SC = *MCG_SC;
	
	//Reset FCRDIV value;
	SC &= ~SC_FCRDIV;
	
	//Update the FRDIV value;
	SC |= channel_id << 1;
	
	//Save SC;
	*MCG_SC = SC;
	
	//Select the fast internal clock;
	*MCG_C2 |= C2_IRCS;
	
}

