/*
  EventTaskList.cpp - Part of TRACER

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

#include <cstdlib>
#include <Interaction/Interaction.h>
#include "EventTaskList.h"


//--------------------------------------------------- Initialisation ---------------------------------------------------

/*
 * Constructor : initialises the tasks array;
 */

EventTaskList::EventTaskList(uint8_t max_size) : max_size(max_size), size(0), tasks_array(nullptr) {}


/*
 * Destructor : frees the dynamic tasks array;
 */
EventTaskList::~EventTaskList() {

    //Simply free the tasks array;
    free(tasks_array);

}
//------------------------------------------------------ Builders ------------------------------------------------------


/*
 * addTask : add a task to the task list.
 *
 * This function starts to realloc the task array, and if it succeeds, saves the new task;
 */

bool EventTaskList::addTask(void (*new_task)()) {

    //First, we must check that the task is not already in the queue;
    uint8_t index = 0;

    //If the task is already present :
    if (isTaskPresent(new_task, &index)) {

        //Log;
        std_out("Error in EventTaskList::addTask : task already present in the list;");

        //Fail;
        return false;

    }

    //Increment the size;
    uint8_t new_size = size + (uint8_t) 1;

    //If the maximum number of tasks is reached :
    if (new_size == max_size) {

        //Log;
        std_out("Error in EventTaskList::addTask : the maximum number of tasks is reached;");

        //Fail;
        return false;

    }

    //Then, realloc the task array, to contain one more element;
    void *new_array = realloc(tasks_array, size);

    if (new_array) {
        //If the reallocation completed :

        //Update the tasks array;
        tasks_array = (void (**)()) new_array;

        //Save the last task;
        tasks_array[size] = new_task;

        //Update the size;
        size = new_size;

        //Complete;
        return true;

    } else {

        //If the reallocation failed :

        //Log;
        std_out("Error in EventTaskList::addTask : the reallocation failed;");

        //Fail;
        return false;

    }

}


/*
 * removeTask : this function will search if the provided function exists in the array, and if so, it will
 *  remove it.
 *
 *  After removing the task, it will reallocate the task array to save the space;
 */

void EventTaskList::removeTask(void (*old_task)()) {

    //First, initialise an index variable;
    uint8_t index = 0;

    //Then, check if the task is present;
    if (!isTaskPresent(old_task, &index)) {

        //If the task is not present :

        //Complete;
        return;

    }

    /*
     * Now index is initialised and contains the index of the task to remove;
     *
     * Shift every task of 1 position, from index+1 to the end;
     */

    const uint8_t size = this->size;

    //For every task after index;
    for (uint8_t shift_index = index + (uint8_t) 1; shift_index < size; index++, shift_index++) {

        //Shift the task of one position;
        tasks_array[index] = tasks_array[shift_index];

    }


    uint8_t new_size = size - (uint8_t) 1;

    //Then, realloc the task array, to contain one less element;
    void *new_array = realloc(tasks_array, size);

    if (new_array) {
        //If the reallocation completed :

        //Update the tasks array;
        tasks_array = (void (**)()) new_array;

        //Update the size;
        this->size = new_size;

    } else {

        //If the reallocation failed :

        //Clear the last task;
        tasks_array[new_size] = nullptr;

        //Log;
        std_out("Error in EventTaskList::addTask : the reallocation failed;");

    }


}


/*
 * clear : this function clears the task array;
 */

void EventTaskList::clear() {

    //First, realloc tasks_array to zero-size;
    void *new_ptr = realloc(tasks_array, 0);

    //If the reallocation completed :
    if (new_ptr) {

        //Update the task pointer;
        tasks_array = (void (**)()) new_ptr;

        //Update the size;
        size = 0;

    }
}


/*
 * isTaskPresent : this function return true if the given task is already present in the queue;
 *
 *  If it finds the task, it saves its index in the given variable;
 */

bool EventTaskList::isTaskPresent(void (*old_task)(), uint8_t *index) {

    //Search for the task in the array;
    for (uint8_t task_index = 0; task_index < size; task_index++) {

        //If a matching task is found
        if (tasks_array[task_index] == old_task) {

            //Update the index;
            *index = task_index;

            //Complete;
            return true;

        }

    }

    //Fail;
    return false;

}


//------------------------------------------------------ Getters -------------------------------------------------------


/*
 * getSize : this function returns the size of the list;
 */

uint8_t EventTaskList::getSize() {

    //Return the size lol;
    return size;

}


/*
 * get_task : this function returns the task at the given index, if it is valid;
 */

void *EventTaskList::get_task(uint8_t index) {

    //If the index is invalid
    if (index >= size)
        return nullptr;

    //If the index is valid, return the appropriate task
    return tasks_array[index];

}
