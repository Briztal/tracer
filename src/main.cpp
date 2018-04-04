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

#include "Kernel/Kernel.h"
#include "core_cm4.h"
#include "setjmp.h"


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

void *stack0 = malloc(200);

void *stack1 = malloc(200);


typedef struct {
    uint32_t pc;
};



/*
 * preempt : this function will switch the context
 */
void preempt() {

}




int main() {

    Serial.begin(115200);

    Serial.println("STACK0 : "+String((long)stack0));

    Serial.println("STACK1 : "+String((long)stack1));


    while(true) {

    }

    //Kernel::start();

}


bool task_1_selected;

void task_0() {

}

void f1() {

    task_1_selected = true;

    while(true) {

        uint8_t i = 2;

        while(i--) {

            Serial.println("F_1_TICK");
            delay(1000);

        }

        preempt();


    }

}

void task_2() {


    task_1_selected = false;

    while(true) {

        uint8_t i = 2;

        while(i--) {

            Serial.println("F_2_TICK");
            delay(1000);

        }

        preempt();


    }

}



