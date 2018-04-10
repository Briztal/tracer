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

#include <Kernel/Scheduler/NewKernel/ThreadManager.h>


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


void task_0(void *) {

    Serial.println("Task_0");
    delay(1000);

    TaskData td;
    td.function = task_0;
    td.termination = end_function;
    Serial.println("Adding");

    ThreadManager::addTask(td);
    Serial.println("Added");


}


void task_1(void *) {

    Serial.println("Task_1");
    delay(1000);

    TaskData td;
    td.function = task_1;
    td.termination = end_function;
    ThreadManager::addTask(td);
    Serial.println("Added");

}



int main() {

    Serial.begin(115200);

    delay(2000);
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

    //Add some tasks;
    ThreadManager::addTask(td0);


    Serial.println("Added");


    TaskData td1;
    td1.function = task_1;
    td1.termination = end_function;

    //Add some tasks;
    ThreadManager::addTask(td1);

    Serial.println("Ready");

    //Run threads;
    ThreadManager::start();

    //Never reached;
    while (true);

    //Kernel::start();

}

