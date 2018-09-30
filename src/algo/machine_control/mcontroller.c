/*
  mcontroller.c Part of TRACER

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

#include "mcontroller.h"

#include <kernel/log.h>


//--------------------------------------------------- Initialisation ---------------------------------------------------

//Init the model of an actuator;
void mcontroller_init_actuator_model(struct mcontroller *ctrl, uint8_t actuator_id, struct actuator_model *model) {
	
	//If the index is invalid :
	if (actuator_id >= ctrl->dimension) {
		
		//Log;
		kernel_log_("set_actuator_model : invalid index;");
		
		//Fail;
		return;
		
	}
	
	//Cache the current model;
	struct actuator_model *current = ctrl->actuators_models[actuator_id];
	
	//If the model is already initialised :
	if (current) {
		
		//Log;
		kernel_log_("set_actuator_model : already initialised;");
		
		//Fail;
		return;
		
	}
	
	//Initialise the actuator model;
	ctrl->actuators_models[actuator_id] = model;
	
	//Decrement the number of uninitialised actuators;
	ctrl->nb_uninitialised_actuators--;
	
}

//Set a builder computation function;
void mcontroller_set_builder_cmp(struct mcontroller *ctrl, uint8_t cmp_id, builder_cpt computation) {
	
	
	//If the index is invalid :
	if (cmp_id >= ctrl->nb_builder_cpts) {
		
		//Log;
		kernel_log_("set_builder_cmp : invalid index;");
		
		//Fail;
		return;
		
	}
	
	//Cache the current model;
	builder_cpt current = ctrl->builder_cpts[cmp_id];
	
	//If the model is already initialised :
	if (current) {
		
		//Log;
		kernel_log_("set_builder_cmp : already initialised;");
		
		//Fail;
		return;
		
	}
	
	//Initialise the computation;
	ctrl->builder_cpts[cmp_id] = computation;
	
	//Decrement the number of uninitialised actuators;
	ctrl->nb_uninitialised_builder_cpts--;
	
	
}

//Set a kinematic constraint function;
void mcontroller_set_kinematic_cnst(struct mcontroller *ctrl, uint8_t cnst_id, kinematic_cnst constraint) {
	
	//If the index is invalid :
	if (cnst_id >= ctrl->nb_kinematic_cnsts) {
		
		//Log;
		kernel_log_("set_kinematic_cnst : invalid index;");
		
		//Fail;
		return;
		
	}
	
	//Cache the current model;
	kinematic_cnst current = ctrl->kinematic_cnsts[cnst_id];
	
	//If the model is already initialised :
	if (current) {
		
		//Log;
		kernel_log_("set_kinematic_cnst : already initialised;");
		
		//Fail;
		return;
		
	}
	
	//Initialise the constraint;
	ctrl->kinematic_cnsts[cnst_id] = constraint;
	
	//Decrement the number of uninitialised actuators;
	ctrl->nb_uninitialised_kinematic_cnsts--;
	
}

//Set a state computation function;
void mcontroller_set_state_cmp(struct mcontroller *ctrl, uint8_t cmp_id, state_cpt computation) {
	
	//If the index is invalid :
	if (cmp_id >= ctrl->nb_state_cpts) {
		
		//Log;
		kernel_log_("set_state_cmp : invalid index;");
		
		//Fail;
		return;
		
	}
	
	//Cache the current model;
	state_cpt current = ctrl->state_cpts[cmp_id];
	
	//If the model is already initialised :
	if (current) {
		
		//Log;
		kernel_log_("set_state_cmp : already initialised;");
		
		//Fail;
		return;
		
	}
	
	//Initialise the computation;
	ctrl->state_cpts[cmp_id] = computation;
	
	//Decrement the number of uninitialised actuators;
	ctrl->nb_uninitialised_state_cpts--;}



