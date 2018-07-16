//
// Created by root on 6/30/18.
//

#ifndef TRACER_CONTROL_GROUPS_H
#define TRACER_CONTROL_GROUPS_H

#include <stdint.h>


typedef struct {

	//The group's first index;
	const uint8_t index_0;

	//The group's second index;
	const uint8_t index_1;

	//The group's third index;
	const uint8_t index_2;

} cgroup_t;


typedef struct {

	//The number of control groups;
	uint8_t nb_groups;

	//The priority order;
	uint8_t *const priority_order;

} cgroups_priotiry_order;


typedef struct {

	//The number of control groups;
	const uint8_t nb_groups;

	//The target speeds array;
	float *const target_speeds;

} cgroups_target_speeds;


typedef struct {

	//The number of control groups;
	uint8_t nb_groups;

	//The control group array;
	const cgroup_t *const groups;

	//The target speeds array;
	float *const target_speeds;

	//The distances array;
	float *const distances;

	//The speeds array;
	float *const speeds;

} control_groups_t;


//Select the prioritary speed group;
void cgroups_set_prioritary_group(uint8_t group);

//Set the priorities array;
void cgroups_set_priorities(uint8_t



#endif //TRACER_CONTROL_GROUPS_H
