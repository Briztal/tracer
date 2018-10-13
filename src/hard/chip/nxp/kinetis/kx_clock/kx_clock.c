/*
  kx_clock.c Part of TRACER

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


#include <stddef.h>
#include <kernel/debug/log.h>
#include <hard/chip/nxp/kinetis/kx_sim.h>
#include <kernel/clock/clock.h>
#include <kernel/mod/auto_mod.h>
#include <kernel/debug/debug.h>

#include "kx_clock.h"


//TODO MAKE PARAMETERS : FLEXBUS CLOCK USED;

//----------------------------------------------- Hardware configuration -----------------------------------------------


static uint8_t sim_dividers_build_usage_mask(struct sim_dividers_config *config) {
	
	uint8_t mask = 0;
	
	if (config->core_system_divider_used) {
		mask |= (1 << 3);
	}
	
	if (config->bus_divider_used) {
		mask |= (1 << 2);
	}
	
	if (config->flexbus_divider_used) {
		mask |= (1 << 1);
	}
	
	if (config->flash_divider_used) {
		mask |= (1 << 0);
	}
	
	return mask;
	
};


/**
 * sim_dividers_build_DIV : builds the DIV value associated with the config.
 *
 * 	DIV is a 16 bits value formatted like a|b|c|d  with each letter being 4 bits long and :
 * 		- a is the core_system divider;
 * 		- b is the bus divider;
 * 		- c is the flexbus divider;
 * 		- d is the flash divider;
 *
 * @param config : the desires SIM dividers configuration;
 *
 * @return the DIV value as described behind;
 */

static uint16_t sim_dividers_build_div(struct sim_dividers_config *config) {
	
	uint16_t div = 0;
	
	div |= (config->core_system_divider & 0xf);
	
	div <<= 4;
	
	div |= (config->bus_divider & 0xf);
	
	div <<= 4;
	
	div |= (config->flexbus_divider & 0xf);
	
	div <<= 4;
	
	div |= (config->flash_divider & 0xf);
	
	return div;
}


/**
 * sim_dividers_early_config : this function will update all dividers that must be increased.
 *
 * 	Only those dividers must be updated before configuring the mcg to the new frequency, as decreasing a divider
 * 	increases the frequency;
 *
 * @param usage_mask : a bit mask that defines which divider can be configured;
 *
 * @param div : the final divider word; see format behind;
 */

static void sim_dividers_early_config(uint8_t usage_mask, uint16_t div) {
	
	//Cache the value of the SIM register;
	uint16_t SIM_DIV = *(((uint16_t *) SIM_CLKDIV1) + 1);
	
	//Define the mask;
	uint16_t mask = 0xf;
	
	//For each divider :
	for (uint8_t i = 0; i < 4; i++) {
		
		//If the divider must be used :
		if (usage_mask & 1) {
			
			//Mask all other dividers;
			uint16_t m_divider = div & mask;
			
			//If the masked value is superior to the one currently in the SIM register :
			if (m_divider > (SIM_DIV & mask)) {
				
				//Reset the divider in the SIM register;
				SIM_DIV &= ~mask;
				
				//Save the new divider in the SIM register;
				SIM_DIV |= m_divider;
				
			}
			
			//If the divider is inferior or equal to the current one, it will not be updated;
			
		}
		
		
		//Update the usage mask;
		usage_mask >>= 1;
		
	}
	
	//Write the new SIM value;
	*(((uint16_t *) SIM_CLKDIV1) + 1) = SIM_DIV;
	
}


/**
 * sim_dividers_late_config : this function will update all dividers that must be decreased.
 *
 * 	Those dividers must be updated after configuring the mcg to the new frequency, as decreasing a divider
 * 	increases the frequency;
 *
 * @param usage_mask : a bit mask that defines which divider can be configured;
 *
 * @param div : the final divider word; see format behind;
 */

static void sim_dividers_late_config(uint8_t usage_mask, uint16_t div) {
	
	//Cache the value of the SIM register;
	uint16_t SIM_DIV = *(((uint16_t *) SIM_CLKDIV1) + 1);
	
	//Define the mask;
	uint16_t mask = 0xf;
	
	//For each divider :
	for (uint8_t i = 0; i < 4; i++) {
		
		//If the divider must be used :
		if (usage_mask & 1) {
			
			//Mask all other dividers;
			uint16_t m_divider = div & mask;
			
			//If the masked value is superior to the one currently in the SIM register :
			if (m_divider < (SIM_DIV & mask)) {
				
				//Reset the divider in the SIM register;
				SIM_DIV &= ~mask;
				
				//Save the new divider in the SIM register;
				SIM_DIV |= m_divider;
				
			}
			
			//If the divider is inferior or equal to the current one, it will not be updated;
			
		}
		
		
		//Update the usage mask;
		usage_mask >>= 1;
		
	}
	
	//Write the new SIM value;
	*(((uint16_t *) SIM_CLKDIV1) + 1) = SIM_DIV;
	
}


/**
 * kx_clock_configure : configures the kx clocking module;
 *
 *
 * 	Clock configuration procedure for kx :
 * 		- (autotune)
 * 		- providing config for mcg and sim dividers;
 * 		- modification of dividers that must be increased;
 * 		- mcg configuration, and mode changing;
 *  	- modification of dividers that must be decreased;
 */

static void kx_clock_configure(struct kx_clock_config *config, size_t config_size) {
	
	//First, verify that the size is correct;
	if (config_size != sizeof(struct kx_clock_config)) {
		
		kernel_log_("kx_clock_configure : error, bad size");
		
	}
	
	//Compute div and usage_mask;
	uint8_t usage_mask = sim_dividers_build_usage_mask(&config->sim_dividers_configuration);
	uint16_t div = sim_dividers_build_div(&config->sim_dividers_configuration);
	
	//Early config;
	sim_dividers_early_config(usage_mask, div);
	
	//MCG config;
	mcg_configure(&config->mcg_configuration);
	
	//Late config;
	sim_dividers_late_config(usage_mask, div);
	
	/*
	 * kernel clocks configuration;
	 */
	
	//Cache the mcgout frequency;
	uint32_t mcgout = config->mcg_configuration.mcgout_freq;
	
	clock_set("core", mcgout / (config->sim_dividers_configuration.core_system_divider + 1));
	clock_set("bus", mcgout / (config->sim_dividers_configuration.bus_divider + 1));
	//TODO NO REGISTRATION WITH MACRO PARAMETER.
	clock_set("flexbus", mcgout / (config->sim_dividers_configuration.flexbus_divider + 1));
	clock_set("flash", mcgout / (config->sim_dividers_configuration.flash_divider + 1));
	
	
}


//------------------------------------------------- Kernel interaction -------------------------------------------------

static bool kx_clock_init() {
	
	//Register the clock tuner; A cast is required to interpret void * as kx_clock_config * automatically;
	// A size check helps bad usage;
	clock_register_tuner((void (*)(void *, size_t)) &kx_clock_configure);
	
	//Register all four clocks;
	clock_register("core", 0);
	clock_register("bus", 0);
	
	//TODO NO REGISTRATION WITH MACRO PARAMETER.
	clock_register("flexbus", 0);
	
	clock_register("flash", 0);
	
	//At init, will be configured to FEI;
	struct kx_clock_config initial_config = {
		
		//All dividers used and set to 0, no division;
		.sim_dividers_configuration = {
			.core_system_divider_used = true,
			.core_system_divider = 0,
			.bus_divider_used = true,
			.bus_divider = 0,
			.flexbus_divider_used = true,
			.flexbus_divider = 0,
			.flash_divider_used = true,
			.flash_divider = 0,
		},
		
		//FEI config;
		.mcg_configuration = {
			
			//Fll Engaged Internal with slow reference;
			.mode = FEI,
			
			//FLL range 0, fll factor of 640;
			.mcgout_freq = 32000 * 640,
			
			
			//IRC not used, fll clocked directly;
			.irc_used = false,
			
			//OSC not used;
			.osc_enabled = false,
			
			//FLL used;
			.fll_used = true,
			
			//FLL configured for Internal clocking in range 0.;
			.fll_configuration = {
				
				//Internal clocking;
				.external_ref = false,
				
				//No divider required, Internal clocking;
				.eref_divider_id = 0,
				
				//Lowest frequency range;
				.f_range = 0,
				
				//IRC slow = 32K;
				.exact_32768_ref = false,
				
			},
			
			//Pll not used;
			.pll_used = false,
			
		},
		
	};
	
	//Configure the clock, and update kernel clocks;
	clock_tune(&initial_config, sizeof(struct kx_clock_config));
	
	//Complete;
	return true;
	
}


KERNEL_EMBED_MODULE(PROC_MODULE, kx_clock, kx_clock_init);


//TODO KERNEL MODULE INTEGRATION