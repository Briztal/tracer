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
#include "kernel/kernel.h"
#include "setjmp.h"

 */


/*
 *  Initialisation :
 *
 *  - Declaration of Threads stack spaces;
 *  - Initialisation of threads program counters;
 *  - Initialisation of threads's stacks;
 *
 *
 *  Preemption, by PenSV call, in normal task mode (lower priority, in process_t context);
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
    //USBuart::write("END FUNCTION REACHED");
}

/*
Semaphore S(1);

//digitalWrite(13, !digitalRead(13));

task_state_t task_0(void *) {

    while (true) {

        USBuart::write("Task_0 REQUEST");

        S.P();

        for (uint8_t i = 10; i--;) {

            USBuart::write("Task_0");

            systick_sleep(1);
        }

        S.V();

        systick_sleep(10);
    }


}

task_state_t task_1(void *) {

    while (true) {

        USBuart::write("Task_1 REQUEST");

        S.P();

        for (uint8_t i = 10; i--;) {

            USBuart::write("Task_1");


        }

        S.V();

        systick_sleep(1);


    }

}

task_state_t task_2(void *) {

    while (true) {

        USBuart::write("Task_2 REQUEST");

        S.P();

        for (uint8_t i = 2; i--;) {
            USBuart::write("Task_2");

        }

        S.V();

        systick_sleep(2);
    }

}


task_state_t task_3(void *) {

    systick_sleep(1000);

    while (true) {
        USBuart::write("Task_3");
        systick_sleep(14000);
    }


    return complete;

}

 */

#include <kernel/drivers/PORT.h>
#include <kernel/arch/peripherals/kinetis/kinetis_PORT.h>
#include <kernel/kernel.h>
#include "Arduino.h"

//#include <Kernel/drivers/PORT.h>

void blink_high() {
    while(true) {


        pinMode(13, OUTPUT);
        digitalWriteFast(13, !digitalReadFast(13));

        delay(30);

    };

}


void blink_low() {
    while(true) {


        pinMode(13, OUTPUT);
        digitalWriteFast(13, !digitalReadFast(13));

        delay(500);

    };

}

void finish() {
    pinMode(13, OUTPUT);
    digitalWriteFast(13, HIGH);

}

void test(bool b) {
    if (b) blink_high();
    else blink_low();
}

int main() {

    /*

    PORT_pin_config_t config;


    PORT_get_pin_config(&PORT_C, 5, &config);

    config.mux_channel = 1;
    config.direction = PORT_OUTPUT;
    config.output_mode = PORT_HIGH_DRIVE;

    PORT_set_pin_configuration(&PORT_A, 5, &config);
    PORT_set_pin_configuration(&PORT_B, 5, &config);
    PORT_set_pin_configuration(&PORT_C, 5, &config);
    PORT_set_pin_configuration(&PORT_D, 5, &config);
    PORT_set_pin_configuration(&PORT_E, 5, &config);

    //test(config.direction == PORT_OUTPUT);
    GPIO_output_registers_t Aregisters;
    GPIO_output_registers_t Bregisters;
    GPIO_output_registers_t Cregisters;
    GPIO_output_registers_t Dregisters;
    GPIO_output_registers_t Eregisters;

    PORT_get_GPIO_registers(&PORT_C, &Aregisters);
    PORT_get_GPIO_registers(&PORT_C, &Bregisters);
    PORT_get_GPIO_registers(&PORT_C, &Cregisters);
    PORT_get_GPIO_registers(&PORT_C, &Dregisters);
    PORT_get_GPIO_registers(&PORT_C, &Eregisters);


    //pinMode(13, OUTPUT);

    PORT_C.GPIO_data->PSOR = (uint32_t) 1<<5;

    //GPIO_data(Cregisters.data_register, (uint32_t) 1<<5);
    //test(*(volatile uint32_t *)Aregisters.data_register == (uint32_t)1<<5);

    //finish();

    while(true);

     */

    kernel_start();


    /*

    pinMode(13, OUTPUT);

    digitalWrite(13, HIGH);

    delay(2000);

     */
    /*
    USBuart::init(115200);

    //Initialise all threads;
    ThreadManager::createThread(1000);
    ThreadManager::createThread(1000);
    ThreadManager::createThread(1000);
    ThreadManager::createThread(1000);
    ThreadManager::createThread(1000);


    TaskData td0;
    td0.function = task_0;
    td0.termination = end_function;

    TaskStorage::addTask(td0);

    //Add some tasks;
    ThreadManager::setTask(0, TaskStorage::addTask(td0));


    TaskData td1;
    td1.function = task_1;
    td1.termination = end_function;

    //Add some tasks;
    ThreadManager::setTask(1, TaskStorage::addTask(td1));


    TaskData td2;
    td2.function = task_2;
    td2.termination = end_function;

    //Add some tasks;
    ThreadManager::setTask(2, TaskStorage::addTask(td2));


    TaskData td3;
    td3.function = task_3;
    td3.termination = end_function;

    //Add some tasks;
    //ThreadManager::setTask(3, TaskStorage::addTask(td3));


    //Run threads;
    ThreadManager::start();
     */

    //Never reached;
    while (true);

    //Kernel::start();

}



