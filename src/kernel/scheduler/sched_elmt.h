/*
  sched_elmt.h Part of TRACER

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

#ifndef TRACER_SCHED_ELEMENT_H
#define TRACER_SCHED_ELEMENT_H


struct sched_elmt {
	
	//The status head. Used by the active list, or by shared fifos;
	struct list_head status_head;
	
	//The main head. Used to reference all processes in their add order; Access is critical;
	struct list_head main_head;
	
	//The process itself. Its form is not specified;
	struct prc *process;
	
	//TODO ENABLE ONLY FOR DEBUG;
	//The activity state; Set if the element is active;
	bool active;
	
};


#endif //TRACER_SCHED_ELEMENT_H
