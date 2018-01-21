/*
  EventTaskList.h - Part of TRACER

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


#ifndef TRACER_EVENTTASKLIST_H
#define TRACER_EVENTTASKLIST_H


#include <avr/pgmspace.h>

class EventTaskList {

    //-------------------------------------- Initialisation --------------------------------------

public:

    //Constructor;
    explicit EventTaskList(uint8_t max_size);

    //Destructor;
    ~EventTaskList();


    //-------------------------------------- Builders --------------------------------------

public:

    //Add a new task to the list;
    bool addTask(void (*new_task)());

    //Remove a task from the list if it is present;
    void removeTask(void (*old_task)());

    //Remove all tasks;
    void clear();


private:

    //Verify that a task is present in the array and eventually save its index;
    bool isTaskPresent(void (*old_task)(), uint8_t *index);



    //-------------------------------------- Getters --------------------------------------

public:

    //Get the number of tasks in the list;
    uint8_t getSize();

    //Get a particular task;
    void *get_task(uint8_t index);


    //-------------------------------------- Fields --------------------------------------

private:

    //The number of tasks in the list;
    uint8_t size;

    //The maximum number of tasks in the list;
    uint8_t max_size;

    //Tasks to execute;
    void (**tasks_array)();


};


#endif //TRACER_EVENTTASKLIST_H
