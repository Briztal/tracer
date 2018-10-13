/*
  preempt.c Part of TRACER

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

#include "preempt.h"


//Initialise the preemption; Will update the handler, set the priority, and enable;
void preemption_init(void (*handler)(void), uint8_t priority) {
	
	//Update the handler;
	__preemption_set_handler(handler);
	
	//Update the priority;
	__preemption_set_priority(priority);
	
	//Update the handler;
	__preemption_enable();
	
}


