//
// Created by root on 9/26/18.
//


#include <hard/chip/nxp/kinetis/periph/kx_clock/kx_mcg.h>

#define U32_DIST(a, b) (((a) < (b)) ? ((b) - (a)) : ((a) - (b)))

/**
 * kx_irc_find_configuration : this function will evaluate all configurations of the IRC, and determine
 * 	if one is closer to the target frequency than the current mcg config;
 *
 * 	If so, the current config will be overwritten;
 *
 * @param target_frequency : the frequency that the mcg should have after config;
 *
 * @param config : the current configuration of the MCG.
 */

bool kx_irc_find_configuration(const uint32_t target_frequency, struct kx_mcg_config *const config) {
	
	//Cache the current frequency;
	const uint32_t current_frequency = config->mcgout_freq;
	
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
		struct kx_mcg_config new_config = {
			
			//Save the frequency;
			.mcgout_freq = irc_frequencies[best_channel],
			
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
