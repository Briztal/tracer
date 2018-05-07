//
// Created by root on 4/29/18.
//

#ifndef TRACER_SEMAPHORE_H
#define TRACER_SEMAPHORE_H


#include <DataStructures/Containers/Container.h>
#include "stdint.h"



/*
 * The Semaphore library.
 *
 *  It is used by the kernel to synchronise processes.
 *
 *  ----------------------- WARNING -----------------------
 *
 *  Semaphores synchronise processes. If they are not called from a process_t (in the ThreadManager's meaning), the
 *  The semaphore call will have no effect, as semaphores lock and unlock processes.
 *
 *  Semaphore can't (lol) lock interrupts;
 *
 *  A semaphore call from an interrupt will be detected, and ignored.
 *
 *  As a consequence, interrupts have full control on drivers, and can use them without any restrictions, but it
 *  can also cause damage, as it can shortcut potentially ordered procedures.
 *
 *  You must take that in consideration when writing your ISRs. Use it carefully;
 */


typedef struct {

    uint16_t allocation_counter;

} semaphore;


//The P function : requires the execution;
void sem_wait(semaphore *semaphore);

//The V function : lets an execution happen;
void sem_post(semaphore semaphore);


//------------------------------------------ Fields ------------------------------------------

//The allocation counter : if superior, allocation is allowed. If not, initiator thread is put to sleep;
uint16_t allocation_counter;

//The Thread vector : memorises threads indices that are put to sleep;
Container<uint8_t> locked_threads;




#endif //TRACER_SEMAPHORE_H
