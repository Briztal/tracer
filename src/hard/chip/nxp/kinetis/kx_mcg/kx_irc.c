//
// Created by root on 9/22/18.
//

#include "kx_mcg.h"


//There are 9 IRC channels;
#define NB_IRC_CHANNELS 9


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



/**
 * kx_irc_find_configuration : this function will evaluate all configurations of the IRC, and determine
 * 	if one is closer to the target frequency than the current mcg config;
 *
 * 	If so, the current config will be overwritten;
 *
 * @param target_frequency : the frequency that the mcg should have after config;
 *
 * @param config : the current configuration of the MCG.
 *
 *
 */

bool kx_irc_find_configuration(const uint32_t target_frequency, struct mcg_config *const config) {
	
	//Cache the current frequency;
	const uint32_t current_frequency = config->frequency;
	
	//Determine the current distance;
	uint32_t current_dist = U32_DIST(current_frequency, target_frequency);
	
	//A flag, set if a closer configuration has been found; If the current frequency is null, it is set;
	bool improvement = (current_frequency == 0);
	
	//If the flag is set, represents the channel closest to the target frequency;
	uint8_t best_channel = 0;
	
	//A flag, set if the target frequency is reached;
	bool target_reached = false;
	
	//For each channel :
	for (uint8_t channel_id = NB_IRC_CHANNELS; channel_id--;) {
	
		//Cache the channel's frequency;
		uint32_t channel_freq = irc_frequencies[channel_id];
		
		//Determine the frequency distance;
		uint32_t channel_dist = U32_DIST(channel_freq, target_frequency);
		
		//If the channel is closer than the current best configuration;
		if (channel_dist < current_dist) {
			
			//Update the improvement flag, and the best channel;
			improvement = true, best_channel = channel_id, current_dist = channel_dist;
			
			//If the channel hits the target frequency :
			if (channel_dist == 0) {
				
				//Update the related flag;
				target_reached = true;
				
				//Stop here;
				break;
				
			}
			
		}
	
	}
	
	//If a better configuration has been found :
	if (improvement) {
		
		//Create the config initializer;
		struct mcg_config new_config = {
			
			//Save the frequency;
			.frequency = irc_frequencies[best_channel],
			
			//Only the IRC will be used;
			.mode = BLPI,
			
			//Only the IRC will be used;
			.irc_used = true,
			
			//Save the channel to configure the IRC properly;
			.irc_channel = best_channel,
			
			//OSC not used,
			.osc_enabled = false,
			
			//FLL not used,
			.fll_used = false,
			
			//PLL not used,
			.pll_used = false,
			
		};
		
		//Update the mcg configuration;
		*config = new_config;
		
	}
	
	//Return true if the target frequency is reached;
	return target_reached;

}
