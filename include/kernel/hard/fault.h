//
// Created by root on 10/3/18.
//

#ifndef TRACER_FAULT_H
#define TRACER_FAULT_H

#include <stdbool.h>

#include <stdint.h>


/*
 * The kernel can handle faults. A fault will place the kernel in a safe state, where it can analyse the fault,
 * 	and decide what to do.
 *
 * 	kernel_fault_handler shall be called whenever the fault is executed.
 *
 * 	The kernel have three hardware related functions that implement fault management :
 *
 * 	__flt_update_env caches fault related registers; can't fail;
 *
 * 	__flt_repair analyses the fault and triggers kernel actions, to repair the fault; can fail;
 *
 * 	__flt_log displays a log message describing the fault that has happened;
 *
 *
 * 	When a fault occurs, the kernel calls __flt_update_env and __flt_repair;
 *
 * 	If the repairing succeeds, it stops and let the execution continue; if not, it calls __flt_log and stalls
 * 	indefinately;
 */

void kernel_fault_handler();


#endif //TRACER_FAULT_H
