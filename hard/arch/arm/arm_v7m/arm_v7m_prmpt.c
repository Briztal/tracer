//
// Created by root on 2/17/19.
//


#include "arm_v7m.h"

//Set the priority of the preemption exception and enable it;
void __prmpt_configure(uint8_t priority) {
    armv7m_set_pendsv_priority(priority);
}

//Set the preemption exception pending;
void __prmpt_trigger() {
    armv7m_set_pendsv_pending();
}



/*
 * The context switcher :
 *
 *  - saves the current thread context;
 *  - calls the kernel context switcher;
 *  - loads the new thread context;
 *
 *  This function is written in pure assembly, and follows the ARM calling convention;
 */

__attribute__ ((naked)) static void arm_v7m_prmpt_handler() {

    __asm__ __volatile__ (\
    //Disable all interrupts;
    "cpsid 	i \n\t"

        //Cache psp in R0. R0 has already been stacked;
        "mrs 	r0, 	psp 		\n\t"

        //Stack {R4 - R11} range, and set R0 to the end of the software context;
        "stmdb 	r0!, 	{r4 - r11}	\n\t"

        //Save LR, as calling bl overwrites it; ARM calling conventions, R4 is not altered by call;
        "mov	r4, 	lr			\n\t"

        //Call __krnl_switch_context to get a new sp. Arm calling conventions, old and new are located in R0;
        "bl 	__krnl_switch_context\n\t"

        //Restore LR;
        "mov	lr, 	r4			\n\t"

        //Update the provided sp to hit the beginning of the software context
        "add 	r0, 	#32			\n\t"

        //Unstack {R4 - R11}, to restore the previous context; R0 remains unchanged, points to the end of the hardware context
        "ldmdb 	r0, 	{r4 - r11} \n\t"

        //Update psp, so that the new task's software context gets loaded;
        "msr 	psp, 	r0 			\n\t"

        //Enable interrupts again;
        "cpsie 	i					\n\t"

        //Complete;
        "bx lr						\n\t"

    );

}

