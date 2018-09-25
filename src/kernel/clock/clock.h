/*
  clock.h Part of TRACER

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


#ifndef TRACER_CLOCK_H
#define TRACER_CLOCK_H


#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


//---------------------------------------------- Clock environment tuning ----------------------------------------------

/*
 * Update the clock tuning environment;
 * 	tuner : the clock tuner, receives a config struct and its size;
 * 	auto_tuner : the clock auto_tuner tuner, receives a frequency and finds a config;
 */

void clock_register_tuner(void (*tuner)(void *, size_t));
void clock_register_auto_tuner( void (*auto_tuner)(uint32_t));


/*
 * Release the clock tuning environment; Both tuners will be reset to 0;
 */

void clock_release_tuner();
void clock_release_auto_tuner();


//---------------------------------------------------- Clock Tuning ----------------------------------------------------

//Tune the clock, providing a config struct and its size;
void clock_tune(void *config, size_t );

//Find automatically the closest configuration for the clock;
void clock_auto_tune(uint32_t);


//-------------------------------------------------- Clock Reference ---------------------------------------------------

//Register a clock in the clock reference;
bool clock_register(const char *name, uint32_t value);

//Update the value of a clock in the clock reference
void clock_set(const char *name, uint32_t value);

//Remove a clock in the clock reference;
void clock_remove(const char *name);



#endif //TRACER_CLOCK_H
