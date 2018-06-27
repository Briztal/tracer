//
// Created by root on 6/25/18.
//

#ifndef TRACER_KINEMATICS_H
#define TRACER_KINEMATICS_H


/*
 * The kinematics struct contains function pointers to the
 */

typedef struct {

	//The pointer to the kinematics instance;
	void *data;

	//Initialise the kinematics manager;
	void (*init)(void *data);

	//Reset the kinematics manager; //TODO REQUIRED ?
	void (*reset)(void *data);

	//Update kinematics data after a trajectory has been inserted in the queue;
	void (*accept_movement)(void *data);



	//Update kinematics movement after a movement has been determined



} kinematics_t;

#endif //TRACER_KINEMATICS_H
