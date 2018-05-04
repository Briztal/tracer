//
// Created by root on 4/29/18.
//

#ifndef TRACER_SEMAPHORE_H
#define TRACER_SEMAPHORE_H


#include <DataStructures/Containers/Container.h>
#include "stdint.h"

/*
 * The Semaphore class.
 *
 *  It is used by the kernel to synchronise threads.
 *
 *  ----------------------- WARNING -----------------------
 *
 *  Semaphores synchronise threads. If they are not called from a thread (in the ThreadManager's meaning), the
 *  The semaphore call will have no effect, as semaphores lock and unlock threads.
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

class Semaphore {

    //------------------------------------------ Object Oriented Methods ------------------------------------------

public:

    //Constructor;
    explicit Semaphore(uint16_t initial_counter);

    //Destructor :
    ~Semaphore();

    //---------- Disabled methods : These objects mustn't be copied, moved or assigned ----------

private:

    //Copy constructor;
    Semaphore(const Semaphore &) : allocation_counter(0), locked_threads(0) {};

    //Move constructor;
    Semaphore(Semaphore &&) noexcept : allocation_counter(0), locked_threads(0) {};

    //Copy constructor;
    Semaphore &operator=(const Semaphore &) {return *this;};

    //Move constructor;
    Semaphore &operator=(Semaphore &&) noexcept {return *this;};


    //------------------------------------------ Semaphore Methods ------------------------------------------

public:

    //The P function : requires the execution;
    void P();

    //The V function : lets an execution happen;
    void V();


    //------------------------------------------ Fields ------------------------------------------

private:

    //The allocation counter : if superior, allocation is allowed. If not, initiator thread is put to sleep;
    uint16_t allocation_counter;

    //The Thread vector : memorises threads indices that are put to sleep;
    Container<uint8_t> locked_threads;


};


#endif //TRACER_SEMAPHORE_H
