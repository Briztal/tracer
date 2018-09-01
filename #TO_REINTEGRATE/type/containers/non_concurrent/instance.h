//
// Created by root on 7/12/18.
//

#ifndef TRACER_INSTANCE_H
#define TRACER_INSTANCE_H

/*
 * A lot of structures contain dynamic data, that must never be owned without its deleter;
 */

typedef struct {

	//The instance's data;
	void *const data;

	//The instance's deleter;
	void (*const destructor)(void *data);

} instance_t;


//Shortcut to delete an instance;
inline void instance_delete(instance_t *const instance) {

	//Call the deleter on data;
	(*(instance->destructor))(instance->data);

}


#endif //TRACER_INSTANCE_H
