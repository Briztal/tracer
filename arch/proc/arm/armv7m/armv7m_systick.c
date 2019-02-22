/*
  arm_v7m_systick.c Part of TRACER

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

#include "armv7m.h"

#include <kernel/res/clock.h>

#include <kernel/common.h>

/*
 * This file contains code to run the arm_v7m system timer;
 */

//Configure the frequency and the priority of the system clock interrupt
extern void __sclk_configure(uint32_t tick_frequency, uint8_t int_prio) {

    //Get the core clock value;
    uint32_t core_freq = clock_get("core");

    //If the core frequency is null :
    if (!core_freq) {

        //Panic. The core clock is not registered;
        __kernel_panic("systick_set_frequency : core clock not registered;");

    }

    //Compute the reload;
    uint32_t reload = core_freq / tick_frequency;

    //Update the reload;
    armv7m_systick_set_reload(reload);

    //Update the priority;
    armv7m_set_systick_priority(int_prio);

}

//Start the system clock;
void __sclk_start() {
    armv7m_systick_select_core_clock();
    armv7m_systick_enable();
    armv7m_systick_int_enable();
}


//Stop the system clock;
extern void __sclk_stop() {
    armv7m_systick_disable();
    armv7m_systick_int_disable();
}

