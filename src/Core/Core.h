/*
  Core.h - Part of TRACER

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

//TODO TESTER L'ENVOI DES PARAMETRES
//TODO MONITOR : AFFICHER LES VALEURS DANS LES WIDGETS
//TODO MONITOR : FAIRE L'AFFICHAGE DES PID ET DES ACTIONS

//TODO : ENVOI DES ACTIONS (METTRE UN OCTET POUR LE TYPE)

//TODO REINTEGRATE GCODEINTERFACE
//TODO ENABLE-DISABLE GCODEINTERFACE
//TODO PIN_INTERRUPT
//TODO DIGITALWRITE DIGITALREAD ANALOGWRITE ANALOGREAD IN HARDWARE_LANGUAGE_ABSTRACTION



/*
 * Core Structure :
 *
 * Main thread: Task manager. Runs basic tasks, like
 * - Communication Interface
 * - Basic interaction
 * - StepperControl Plannification for TRACER
 *
 * A task can be added, in the queue. Priorities may be defined in a future commit.
 *
 *
 * Level 0 Thread : Running permanent tasks, on non prioritary interrupts
 * - Control loops;
 * - Step process function
 *
 */


/*
 * The Machine Core.
 *
 * The Core is in charge of :
 * - setting up all used modules, in begin function;
 * - processing tasks, provided internally, or coming from any interface;
 *
 * Tasks are stored using 3 Queues :
 *
 *
 * (lower priority)
 *
 * priority 0 :
 *      - Permanent task queue : a list of background routines. Defined in the configuration file, unchangeable
 *              after compilation; these tasks have the lowest priority status.
 *
 * priority 1 :
 *      - External commands queue : a queue containing external commands, received by any interface;
 *
 * priority 2 :
 *      - Internal tasks queue : a queue containing tasks enqueued by an internal procedure.
 *              These tasks are processed with the highest priority.
 *
 * (higher priority)
 */

#ifndef PROJECT_SYSTEM_H
#define PROJECT_SYSTEM_H

#include <sanity_check.h>

#include "../DataStructures/Queue.h"

class Core {

public:

    static void run();

    static void begin();

    static bool add_external_task(void (*f)(char *, uint8_t), char *args, uint8_t size);

    static bool add_internal_task(void (*f)());

private:

    static bool process_external_task();

    static Queue<void(*)(char *, uint8_t)> external_tasks;

    static char *external_args;

    static Queue<void(*)()> internal_tasks;

    static bool process_internal_task();

};



#endif //PROJECT_SYSTEM_H
