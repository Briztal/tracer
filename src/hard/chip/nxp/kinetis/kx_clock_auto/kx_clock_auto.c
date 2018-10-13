/*
  kx_clock_auto.c Part of TRACER

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


#include <kernel/panic.h>
#include <kernel/clock/clock.h>
#include <kernel/debug/debug.h>
#include <kernel/debug/log.h>

#include "kx_clock_auto.h"



/*
 * TODO LIST :
 *
 * 	TODO LIST PARAMETERS TO PROVIDE FOR BOTH CLOCK MODULES.
 * 	TODO MODULARISE THE OSC TO DEFINE MULTIPLE CHANNELS
 * 	TODO CHECK MAKE PARAMETERS
 */

/*
 * -------------------------------------------------------- IRC --------------------------------------------------------
 */

//TODO
#define NB_CONFIGURATION_FINDER 3

/*
 * This static array contains functions that can be used to search an adapted configuration;
 */

static bool (*const config_search[NB_CONFIGURATION_FINDER])(uint32_t target, struct kx_mcg_config *config) = {
	
	/*
	 * First, internal clocking configs will be evaluated;
	 *
	 * !!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!
	 *
	 * The IRC can always work and provide a proper configuration. It can be called with an
	 * empty config (config.frequency = 0).
	 * All others will trigger a kernel panic if they get called with an empty configuration;
	 */
	
	&kx_irc_find_configuration,
	
	//External clocking configs will be evaluated after;
	&kx_osc_find_configuration,
	
	//FLL clocking configs will then be evaluated;
	&kx_fll_find_configuration,
	
	/*
	//PLL clocking configs will finally be evaluated;
	&kx_pll_find_configuration,
	 */
	
};


void kx_clock_autotune(const uint32_t frequency_target) {
	
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	
	//Declare the target configuration;
	struct kx_mcg_config config;
	
	debug_delay_ms(1000);
	
	kernel_log_("Evaluating ...");
	
	//For each configuration finder :
	for (uint8_t finder_id = 0; finder_id < NB_CONFIGURATION_FINDER; finder_id++) {
		
		/*
		 * Search for a config that hits the target frequency;
		 * If a config closer than the current one is found, the config will be updated;
		 * If a config matching the target frequency is found, 1 will be returned, and 0 if not;
		 */
		
		bool exact = (*(config_search[finder_id]))(frequency_target, &config);
		
		//If a matching solution is found, stop here;
		if (exact)
			break;
		
	}
	
	
	kernel_log_("Done. Calling kernel clock config.");
	
	//Call the kernel clock tuning function;
	clock_tune(&config, sizeof(struct kx_mcg_config));
	
}




