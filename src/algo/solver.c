/*
  solving.c Part of TRACER

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


#include "solver.h"


//Actualise the state of a solving unit;
void solver_iterate(struct solver *s) {
	
	//The next state will be used multiple times. A cache will be used;
	void *next = s->s_next;
	
	//Cache the operations struct ref;
	struct solver_ops *ops = s->s_ops;
	
	//Cache the state updater;
	void (*update_state)(void *) = ops->update_state;
	
	//If the loop is closed : 
	if (update_state) {
		
		//Feedback;
		(*update_state)(next);
		
	} else {
		
		//If open loop, just switch target and next;
		void *tmp = s->s_target;
		s->s_target = next;
		s->s_next = next = tmp;
		
	}
	
	//Cache the state completion function;
	void (*complete_state)(void *, const void *) = ops->complete_state;
	
	//If the state must be completed :
	if (complete_state) {
		
		//Complete the next state providing the current state;
		(*complete_state)(next, s->s_current);
		
	}
	
	//Determine the next target state;
	(*(ops->compute_target))(s->s_ctrl, next, s->s_target);
	
}
