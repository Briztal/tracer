/*
  kinetis_PORT.h Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

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

#ifndef TRACER_KINETIS_GPIO_H
#define TRACER_KINETIS_GPIO_H

#include <stdint.h>
#include <stddef.h>

#include <kernel/drivers/gpio.h>


//------------------------------------------------- Creation - Deletion ------------------------------------------------

//Create a kinetis GPIO driver;
struct gpio_driver_t *kinetis_GPIO_create(struct kinetis_GPIO_memory_t *first_memory_map, uint8_t nb_peripherals,
											size_t memory_spacing);


#endif //TRACER_KINETIS_GPIO_H
