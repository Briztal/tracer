/*
  mutex.h Part of TRACER

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

#ifndef TRACER_MUTEX_H
#define TRACER_MUTEX_H


/*
 * A mutex is composed of some data, and three function pointers :
 * 	- a lock function;
 * 	- an unlock function;
 * 	- a deleter function;
 *
 */

typedef struct mutex_t {

	//The mutex instance;
	void *mutex_instance;

	//The mutex's lock function;
	void (*const mutex_lock)(void *mutex_instance);

	//The mutex's unlock function;
	void (*const mutex_unlock)(void *mutex_instance);

	//The mutex's duplication function;
	struct mutex_t *(*const mutex_duplication)(const void *mutex_instance);

	//The mutex's deleter;
	void (*const mutex_destructor)(void *mutex_instance);

} mutex_t;


//Shortcut for lock;
inline void mutex_lock(mutex_t *mutex) {

	//Call the lock function passing instance;
	(*(mutex->mutex_lock))(mutex->mutex_instance);

}


//Shortcut for unlock;
inline void mutex_unlock(mutex_t *mutex) {

	//Call the unlock function passing instance;
	(*(mutex->mutex_unlock))(mutex->mutex_instance);

}


//Shortcut for duplicator;
inline mutex_t *mutex_duplicate(const mutex_t *const mutex) {

	//Call the duplication passing instance;
	return (*(mutex->mutex_duplication))(mutex->mutex_instance);

}


//Shortcut for deleter;
inline void mutex_delete(mutex_t *mutex) {

	//Call the deleter function passing instance;
	(*(mutex->mutex_destructor))(mutex->mutex_instance);

}


#endif //TRACER_MUTEX_H
