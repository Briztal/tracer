/*
  Part of TRACER

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

/*
#include "kernel/kernel.h"
#include "setjmp.h"

 */


#include <kernel/arch/arch.h>

#include <kernel/arch/peripherals/kinetis/kinetis_UART.h>
#include <kernel/kernel.h>


int main() {


	//Initialise drivers;
	teensy35_hardware_init();

    core_init();

    //Never reached;
    while (true);


}



