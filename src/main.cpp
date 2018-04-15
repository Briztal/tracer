/*
  Part of TRACER

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

/*
#include "Kernel/Kernel.h"
#include "setjmp.h"

 */

#include "Arduino.h"

#include <Kernel/Scheduler/ThreadManager/ThreadManager.h>
#include <Kernel/Scheduler/TaskStorage.h>
#include <Kernel/Scheduler/Systick.h>
#include <usb_dev.h>


/*
 *  Initialisation :
 *
 *  - Declaration of Threads stack spaces;
 *  - Initialisation of threads program counters;
 *  - Initialisation of threads's stacks;
 *
 *
 *  Preemption, by PenSV call, in normal task mode (lower priority, in process context);
 *
 *      ASM :
 *      - (AUTOMATIC) Stacking of hardware context; //TODO VERIFY WHICH CONTEXT IS STACKED
 *      - Stacking of the software context (GPRegisters + FPU REGISTERS IF REQUIRED) //TODO VERIFY WHICH CONTEXT IS STACKED
 *
 *      C / C++;
 *      - Call to the threadManager to get a valid thread to execute;
 *
 *      ASM :
 *      - Modification of the PSP;
 *      - Unstacking of the software context (GPRegisters + FPU REGISTERS IF REQUIRED) //TODO VERIFY WHICH CONTEXT IS UNSTACKED
 *      - Return -> (AUTOMATIC) Unstacking of the hardware context //TODO VERIFY WHICH CONTEXT IS UNSTACKED
 *
 *
 *  Task scheduling : C / C++;
 *
 *      - All tasks are stored in heap;
 *      - The scheduler selects a task to execute, and returns a pointer to it;
 *      - If no task is found, a nop task is selected;
 *
 *
 *  Thread function : C / C++;
 *
 *      - loop :
 *      - Call to scheduler, to get a new task to execute;
 *      - Execution of the task (multiple preemption possible);
 *      - endloop:
 *
 *
 *  Thread Manager : C / C++;
 *
 *      - Save of the current Thread's PSP;
 *      - Selection of the next thread to run;
 *      - Determination of the thread's PSP;
 */



void end_function() {
    Serial.println("END FUNCTION REACHED");
}


task_state_t task_0(void *) {

    delay(500);

    while (true) {
        //Serial.println("Task_0");
        digitalWrite(13, !digitalRead(13));
        Systick::delay(1000);
    }


}

task_state_t task_1(void *) {

    while (true) {
        Serial.println("Task_1");
        Systick::delay(3000);
    }

}

task_state_t task_2(void *) {

    while (true) {
        Serial.println("Task_2");
        Systick::delay(5000);
    }

}


task_state_t task_3(void *) {

    Systick::delay(1000);

    while (true) {
        Serial.println("Task_3");
        Systick::delay(14000);
    }


    return complete;

}


int main() {

    delay(1000);

    pinMode(13, OUTPUT);

    digitalWrite(13, HIGH);

    Serial.begin(115200);

    Serial.println("INITIALISED");


    Serial.println("SUUUUUS");

    //Initialise all threads;
    ThreadManager::createThread(1000);
    ThreadManager::createThread(1000);
    ThreadManager::createThread(1000);
    ThreadManager::createThread(1000);
    ThreadManager::createThread(1000);


    Serial.println("Initialised");

    TaskData td0;
    td0.function = task_0;
    td0.termination = end_function;

    TaskStorage::addTask(td0);

    //Add some tasks;
    ThreadManager::setTask(0, TaskStorage::addTask(td0));


    Serial.println("Added");


    TaskData td1;
    td1.function = task_1;
    td1.termination = end_function;

    //Add some tasks;
    //ThreadManager::setTask(1, TaskStorage::addTask(td1));


    Serial.println("Added");


    TaskData td2;
    td1.function = task_2;
    td1.termination = end_function;

    //Add some tasks;
    //ThreadManager::setTask(2, TaskStorage::addTask(td2));


    Serial.println("Added");


    TaskData td3;
    td1.function = task_3;
    td1.termination = end_function;

    //Add some tasks;
    //ThreadManager::setTask(3, TaskStorage::addTask(td3));

    Serial.println("Ready");

    //Run threads;
    ThreadManager::start();

    //Never reached;
    while (true);

    //Kernel::start();

}

