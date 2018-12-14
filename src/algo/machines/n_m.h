//
// Created by root on 12/6/18.
//

#ifndef TRACER_CNC_H
#define TRACER_CNC_H

#include <stdint.h>



struct cnc_state {
	
	//Actuation positions. Actuators commanded on an integer base;
	float act_val[];
	
	//Actuation positions. Actuators commanded on an integer base;
	float act_d1[];
	
	
	//Control positions. Expressed in float;
	float env_val[];
	
	//Control positions. Expressed in float;
	float env_d1[];
	
};



#endif //TRACER_CNC_H
