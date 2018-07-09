//
// Created by root on 7/9/18.
//

#ifndef TRACER_MUTEX_H
#define TRACER_MUTEX_H


/*
 * A mutex is composed of some data, and three function pointers :
 * 	- a lock function;
 * 	- an unlock function;
 * 	- a destructor function;
 *
 */

typedef struct {

	//The mutex instance;
	void *mutex_instance;

	//The mutex's lock function;
	void (*const mutex_lock)(void *mutex_instance);

	//The mutex's unlock function;
	void (*const mutex_unlock)(void *mutex_instance);

	//The mutex's destructor;
	void (*const mutex_destructor)(void *mutex_instance);

} mutex_t;


//Shortcut for lock;
inline void mutex_lock(mutex_t *mutex) {

	//Call the lock function passing instance;
	(*(mutex->mutex_lock))(mutex->mutex_instance);

}


//Shortcut for unlock;
inline void mutex_unlock(mutex_t *mutex) {

	//Call the lock function passing instance;
	(*(mutex->mutex_unlock))(mutex->mutex_instance);

}


//Shortcut for destructor;
inline void mutex_delete(mutex_t *mutex) {

	//Call the lock function passing instance;
	(*(mutex->mutex_destructor))(mutex->mutex_instance);

}


#endif //TRACER_MUTEX_H
