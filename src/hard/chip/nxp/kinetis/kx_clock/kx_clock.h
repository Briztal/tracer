/*
  kx_clock.h Part of TRACER

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

#ifndef TRACER_KX_CLOCK_H
#define TRACER_KX_CLOCK_H

#include "kx_mcg.h"



struct sim_dividers_config {
	
	//Must the core_system divider be modified ?
	bool core_system_divider_used;
	
	//The core system divider.
	uint16_t core_system_divider;
	
	
	//Must the bus divider be modified ?
	bool bus_divider_used;
	
	//The core system divider.
	uint16_t bus_divider;
	
	
	//Must the flexbus divider be modified ?
	bool flexbus_divider_used;
	
	//The core system divider.
	uint16_t flexbus_divider;
	
	
	//Must the flash divider be modified ?
	bool flash_divider_used;
	
	//The core system divider.
	uint16_t flash_divider;
	
};


struct kx_clock_config {
	
	//The MCG configuration structure;
	struct kx_mcg_config mcg_configuration;
	
	//The sim dividers configuration structure;
	struct sim_dividers_config sim_dividers_configuration;
	
};


#endif //TRACER_KX_CLOCK_H
