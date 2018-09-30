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

#ifndef TRACER_KX_CLOCK_AUTO_H
#define TRACER_KX_CLOCK_AUTO_H

#include <stdbool.h>

#include <stdint.h>


//------------------------------------------------ Configuration finding -----------------------------------------------

//Evaluate all configurations, and update the mcg config, if a better config that the current is found;
bool kx_irc_find_configuration(uint32_t target_frequency, struct kx_mcg_config *config);

//Evaluate all configurations, and update the mcg config, if a better config that the current is found;
bool kx_osc_find_configuration(uint32_t target_frequency, struct kx_mcg_config *config);

//Evaluate all configurations, and update the mcg config, if a better config that the current is found;
bool kx_fll_find_configuration(uint32_t target_frequency, struct kx_mcg_config *config);


//------------------------------------------------ Frequency tuning -----------------------------------------------

void mcg_autotune(uint32_t frequency_target);


#endif //TRACER_KX_CLOCK_AUTO_H
