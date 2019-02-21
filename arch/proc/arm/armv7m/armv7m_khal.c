/*
  arm_v7m.c Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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
 * This file contains the kernel hardware base for the arm v7m architecture.
 */

//------------------------------------------------ Make parameters -----------------------------------------------

#if !defined(RUN_MODE) || !defined(NB_EXCEPTIONS) || !defined(SUPPORT_NVIC_RELOC)

#error "The run mode or the number of exceptions, or the relocation support state were not provided. Check your makefile;"

#define SUPPORT_NVIC_RELOC 1

#define RUN_MODE 0

#define NB_EXCEPTIONS 256

#endif


//------------------------------------------------------ Includes ------------------------------------------------------

#include <stdint.h>


#include <kernel/hard/arch/lnk.h>
#include <kernel/printk.h>
#include <kernel/common.h>
#include <kernel/arch/lnk.h>
#include "../../../khooks.h"

#include "armv7m.h"



//Define an empty ISR handler
static void no_isr() {}

static void __arm_v7m_init() {

    //Enable all faults;

    armv7m_enable_bus_fault();
    armv7m_enable_usage_fault();
    armv7m_enable_mem_fault();

    armv7m_set_mem_fault_priority(0);
    armv7m_set_bus_fault_priority(0);
    armv7m_set_usage_fault_priority(0);

    //Call the run init function;
    __proc_init();

}


static void arm_v7m_hard_fault_handler() {
    kernel_fault_handler();
}


static void arm_v7m_bus_fault_handler() {
    kernel_fault_handler();
}


static void arm_v7m_mem_fault_handler() {
    kernel_fault_handler();
}


static void arm_v7m_usg_fault_handler() {
    kernel_fault_handler();
}


/*------------------------------------------------------ syscall -----------------------------------------------------*/

//Set the priority of the syscall exception and enable it;
void __syscl_configure(uint8_t priority) {
    armv7m_set_svcall_priority(priority);
}


/**
 * syscl_handler : this function handles the SVC exception. It performs the following operations :
 * 	- preparing syscall arguments, namely syscall_id, arg0, arg1 and arg2 for the kernel syscall handler;
 * 	- calling kernel_syscall_handler;
 * 	- Modifying the stacked version of R0 so that the kernel syscall handler's return value
 * 		arrives to __syscall_trigger;
 */

__attribute__ ((naked)) static void arm_v7m_syscl_handler() {
    __asm__ __volatile__ (""

        "push 	{lr} 	\n\r"

        //R0-R3 have not been altered. We can directly call the kernel syscall handler;
        "bl 	__krnl_syscall_handler\n\r"

        "pop 	{lr} 	\n\r"

        //Cache PSP in R5;
        "mrs	r1, 	psp	\n\r"

        //Save R0 in the psp; The psp points to the stacked version of R0;
        "str	r0, 	[r1]\n\r"

        //Exit from exception;
        "bx 	lr			\n\r"

    );
}


/*---------------------------------------------------- preemption ----------------------------------------------------*/


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





//--------------------------------------------------- dynamic vtable ---------------------------------------------------

/*
 * The dynamic vtable can replace the static vtable if nvic relocation is possible, and is queried by the static
 *  vtable for non-system exceptions ;
 *
 *  It is initialised at runtime;
 */

//Define the kernel vtable, and align it on 512 bytes.
static void (*dynamic_vtable[NB_EXCEPTIONS])() __attribute__ ((aligned (512))) = {};


//------------------------------------------------- boot - static vtable ------------------------------------------------

/*
 * If the kernel is standalone, it must handle the processor startup; This is made by embedding the vtable in the
 *  executable;
 */

#if (RUN_MODE == 0)

/**
 * isr_generic_handler : in order to support handler update, all functions of the in-flash vector table will
 * 	lead to this function, that executes the appropriate function;
 *
 * @param i : the interrupt channel. 0 to 240;
 */

static void isr_generic_flash_handler(uint8_t i) {

    //Execute the handler;
    (*dynamic_vtable[i])();

}


/*
 * Generate an ISR for each interrupt channel; Done using XMacro;
 */

//The handler link : a function that calls the handler link with a specific value;
#define channel(i) static void isr_##i() {isr_generic_flash_handler(i);}

//Define all isrs;
#include "static_vtable.h"

#undef channel

/*
 * In order to start the processor properly, we define here the vector table, that is hard-copied in the firmware
 * 	as it, at the link section .vector. This section can be found in the link script, and starts at address 0;
 */

void (*static_vtable[NB_EXCEPTIONS])()  __attribute__ ((section(".vectors"))) = {

    //0 : not assigned;
    (void (*)()) &__ram_max,

    //1 : reset; Effectively used when NVIC is relocated;
    &__arm_v7m_init,

    //2 : NMI. Not supported for instance;
    &no_isr,

    //3 : HardFault.
    &arm_v7m_hard_fault_handler,

    //4 : MemManage fault;
    &arm_v7m_mem_fault_handler,

    //5 : BusFault.
    &arm_v7m_bus_fault_handler,

    //6 : UsageFault;
    &arm_v7m_usg_fault_handler,

    //7 : Reserved;
    &no_isr,

    //8 : Reserved;
    &no_isr,

    //9 : Reserved;
    &no_isr,

    //10 : Reserved;
    &no_isr,

    //11 : SVCall, syscall handler, hooking the kernel;
    &arm_v7m_syscl_handler,

    //12 : Reserved;
    &no_isr,

    //13 : Reserved;
    &no_isr,

    //14 : PendSV, context switcher, hooking the kernel;
    &arm_v7m_prmpt_handler,

    //15 : SysTick, kernel hook;
    &__krnl_tick,

    //In order to avoid writing 254 times the function name, we will use macros that will write it for us;
#define channel(i) &isr_##i,

//Redirect all isrs to the empty one;
#include "static_vtable.h"

#undef channel

    //Adding another "&empty_isr" will cause a compiler warning "excess array initializer. Try it, it is funny !

};


/*
 * If the kernel is called by an external program, the kernel entry point must be embedded at the beginning of the
 * executable;
 */

#else //(RUN_MODE == 0)

//Create a function pointer referencing the entry point and embed it in the vector section;
void (*entry_point()  __attribute__ ((section(".vectors"))) =  &__arm_v7m_init;

#endif //(RUN_MODE == 0)


//----------------------------------------------------- vtable init ----------------------------------------------------

/**
 * dynamic_vtable_init : initializes the dynamic vector table;
 */

void __xcpt_init() {

    uint16_t exception_id;

    //If vector table relocation is enabled :
    #if (SUPPORT_NVIC_RELOC == 1)

    //Check that the kernel vtable is properly aligned; If not :
    if ((uint32_t) dynamic_vtable & 511) {

        //Log;
        printk("nvic_relocation : the kernel vtable is not properly aligned. Aborting.");

        //Do not initialise system exception handlers;

    } else {

        //Relocate the vector table;
        *ARMV7_VTOR = (uint32_t) dynamic_vtable;

        /*
         * Initialise system exception handlers;
         */

        //1 : reset;
        dynamic_vtable[1] = &__arm_v7m_init;

        //2 : NMI. Not supported for instance;
        dynamic_vtable[2] = &no_isr;

        //3 : HardFault.
        dynamic_vtable[3] = &arm_v7m_hard_fault_handler;

        //4 : MemManage fault;
        dynamic_vtable[4] = &arm_v7m_mem_fault_handler;

        //5 : BusFault.
        dynamic_vtable[5] = &arm_v7m_bus_fault_handler;

        //6 : UsageFault;
        dynamic_vtable[6] = &arm_v7m_usg_fault_handler;

        //7 : Reserved;
        dynamic_vtable[7] = &no_isr;

        //8 : Reserved;
        dynamic_vtable[8] = &no_isr;

        //9 : Reserved;
        dynamic_vtable[9] = &no_isr;

        //10 : Reserved;
        dynamic_vtable[10] = &no_isr;

        //11 : SVCall; syscall handler; hooking the kernel;
        dynamic_vtable[11] = &arm_v7m_syscl_handler;

        //12 : Reserved;
        dynamic_vtable[12] = &no_isr;

        //13 : Reserved;
        dynamic_vtable[13] = &no_isr;

        //14 : PendSV; context switcher; hooking the kernel;
        dynamic_vtable[14] = &arm_v7m_prmpt_handler;

        //15 : SysTick; kernel hook;
        dynamic_vtable[15] = &__krnl_tick;

    }

    #endif


    /*
     * Initialise non-system exception handlers;
     */

    for (exception_id = 16; exception_id < NB_EXCEPTIONS; exception_id++) {
        dynamic_vtable[exception_id] = &no_isr;
    }


}


//------------------------------------------------ Interrupt priorities ------------------------------------------------

/*
 * The ARMV7 NVIC support 8 bit priorities. The number of bits evaluated depends on the implementation, but there
 * 	are at least 3 bits evaluated, which allows the library to implement all core priority levels;
 */


//The lowest priority level;
const uint8_t __xcpt_priority_0 = 0xE0;
const uint8_t __xcpt_priority_1 = 0xC0;
const uint8_t __xcpt_priority_2 = 0xA0;
const uint8_t __xcpt_priority_3 = 0x80;
const uint8_t __xcpt_priority_4 = 0x60;
const uint8_t __xcpt_priority_5 = 0x40;
const uint8_t __xcpt_priority_6 = 0x20;
const uint8_t __xcpt_priority_7 = 0x00;


//----------------------------------------------- IC standard interrupts -----------------------------------------------

/**
 * ic_enable_interrupts : enables the interrupt control;
 */

void __xcpt_enable() {
    __asm__ volatile("cpsie i":: :"memory");
}


/**
 * ic_disable_interrupts : enables the interrupt control;
 */

void __xcpt_disable() {
    __asm__ volatile("cpsid i":: :"memory");
}


/**
 * irq_enable : enables the required irq channel;
 * @param channel : the channel to enable;
 */

void __irq_enable(uint16_t channel) {
    armv7m_nvic_enable_interrupt(channel);
}

/**
 * irq_disable : disables the required irq channel;
 * @param channel : the channel to disable;
 */

void __irq_disable(uint16_t channel) {
    armv7m_nvic_disable_interrupt(channel);
}


/**
 * irq_set_pending : sets the required irq in the pending state;
 * @param channel : the channel to set in the pending state;
 */

void __irq_set_pending(uint16_t channel) {
    armv7m_nvic_set_interrupt_pending(channel);
}


/**
 * irq_clear_pending : sets the required irq in the not-pending state;
 * @param channel : the channel to set in the not-pending state;
 */

void __irq_clear_pending(uint16_t channel) {
    armv7m_nvic_clear_interrupt_pending(channel);
}

/**
 * irq_is_pending : sets the required irq in the not-pending state;
 * @param channel : the channel to set in the not-pending state;
 */

void __irq_is_pending(uint16_t channel) {
    armv7m_nvic_clear_interrupt_pending(channel);
}


/**
 * irq_set_priority : applies the provided priority to the required non-system interupt channel;
 *
 * @param channel : the channel to apply the priority;
 * @param priority : the priority to apply
 */

void __irq_set_priority(uint16_t channel, uint8_t priority) {
    armv7m_nvic_set_priority(channel, priority);
}

/**
 * irq_get_priority : applies the provided priority to the required non-system interupt channel;
 *
 * @param channel : the channel to apply the priority;
 * @param priority : the priority to apply
 */

uint8_t __irq_get_priority(uint16_t channel, uint8_t priority) {
    return armv7m_nvic_get_priority(channel);
}



/**
 * irq_in_handler_mode :
 *
 * @return true is a handler is curently in execution;
 */
bool __irq_in_handler_mode() {
    return (bool) armv7m_get_exception_number();
}


/**
 * irq_set_handler : updates the required irq handler;
 *
 * @param non_system_channel : the channel to update. must be inferior to 240;
 *
 * @param handler : the handler. Can be null if interrupt must be disabled;
 */

void __irq_set_handler(uint16_t irq_channel, void ( *handler)()) {

    //Translate to exception channel;
    irq_channel += 16;

    //If the channel is invalid :
    if (irq_channel >= NB_EXCEPTIONS) {

        //Panic;
        kernel_panic("arm_v7m.c : __irq_set_handler : invalid irq channel;");

    }

    //If the handler is null, save the empty handler; If not, save the handler;
    dynamic_vtable[irq_channel] = (handler) ? handler : no_isr;

}


/**
 * irq_reset_interrupt_handler : resets the handler of the required channel to 0, isr will return immediately;
 * @param channel : the interrupt channel to update;
 */

void __irq_reset_interrupt_handler(uint16_t channel) {
    __irq_set_handler(channel, &no_isr);
}




/*-------------------------------------------------------- CPU -------------------------------------------------------*/

/*
 * Notice :
 *
 *
 *  arm v7m minimal stack frame :
 *
 *      	----------- <- pre irq / stack frame header end;
 *      XPSR
 *      PC
 *      LR
 *      R12
 *      R3
 *      R2
 *      R1
 *      R0	--------- <- hw context save PSP EXCEPTION
 *      R4
 *      R5
 *      R6
 *      R7
 *      R8
 *      R9
 *      R10
 *      R11	--------- <- sw context save
 */


/*
 * __cpu_create_stack_context : this function initialises the unstacking environment, so that the given function will
 *  be executed at context switch time;
 *
 *  It starts by caching the process_t stack pointer, and stacks the process functions pointers, and the PSR.
 *
 *  Then, it saves the process index in R12 (next word);
 *
 *  Finally, it leaves space for empty stack frame and saves the stack pointer;
 */

void *__cpu_create_stack_context(void *sp_reset, void (*function)(), void (*exit_loop)(), void *arg) {

    //Cache the current stack pointer;
    uint32_t *sp4 = sp_reset;

    //Store the PSR. Contains the execution mode; //TODO DOC
    *(sp4 - 1) = 0x01000000;

    //Store the function in PC cache
    *(sp4 - 2) = (uint32_t) (function);

    //Store the return function in LR;
    *(sp4 - 3) = (uint32_t) (exit_loop);

    //Store the arg in R12 cache;
    *(sp4 - 4) = (uint32_t) (arg);

    //Update the stack pointer; Hits the future R4 reload address;
    sp4 -= 16;

    //Return the stack pointer;
    return sp4;

}


//Determine the closest inferior address, that would respect alignment requirements;
void *__cpu_stack_align(void *stack_reset) {

    //@core_stack_alignment is a power of 2, decrement and we obtain the mask of bits to reset;
    return (void *) ((((size_t) stack_reset) & ~((size_t) 7)));

}


/**
 * __cpu_enter_thread_mode :
 * 	- changes the current stack from msp to psp;
 * 	- updates control to use psp;
 * 	- updates msp with the provided exception stack;
 * 	- triggers preemption;
 * 	- enables interrupts so that preemption happen;
 *
 * 	This function is pure assembly, and uses ARM calling convention. @exception_sp will be located in R0;
 *
 * @param exception_sp : the stack where exceptions should happen;
 */

__attribute__ ((naked)) void __cpu_enter_thread_mode(void *exception_sp) {

    __asm__ __volatile__ (\

    //Disable interrupts;
    "cpsid 	i \n\t"

        //Save msp in R1;
        "mrs 	r1,  	msp		\n\t"

        //Write psp with cached value of msp;
        "msr 	psp,  	r1		\n\t"

        //Prepare new control value. psp used, privileged;
        "mov 	r2, 	#2 		\n\t"

        //Update the value of control, to use psp;
        "msr 	control, r2		\n\t"

        //Update the main stack pointer, so that exceptions use the exception stack;
        "msr 	msp,  r0		\n\t"

        //Save LR in R4, as bl will overwrite it;
        "mov 	r4,		lr		\n\t"

        //Set preemption pending;
        "bl 	__prmpt_trigger\n\t"

        //Enable all faults;
        "cpsie 	f				\n\t"

        //Enable all interrupts; Preemption should happen;
        "cpsie 	i				\n\t"

        //Return. That point should never be reached, if the preemption occurred correctly;
        "bx 	r4				\n\t"

    );


}


/*------------------------------------------------------ faults ------------------------------------------------------*/

/**
 * arm_v7m_fault_env : struct representation for all ARMV7M fault registers;
 */

struct arm_v7m_fault_env {

    //Memory management Fault Status Register;
    uint8_t MMFSR;

    //Bus Fault Status Register;
    uint8_t BFSR;

    //Usage Fault Status Register;
    uint16_t UFSR;

    //Hard fault Status Register;
    uint32_t HFSR;

    //Debug fault Status Register;
    uint32_t DFSR;

    //Memory Management Fault Address Register;
    uint32_t MMFAR;

    //Bus Fault Address Register;
    uint32_t BFAR;

};


//The fault environment;
static struct arm_v7m_fault_env fault_env;


/**
 * __flt_update_env : updates the fault environment structure with content of fault registers, and resets them;
 */

void __flt_update_env() {

    //Cache the ref of the fault env;
    struct arm_v7m_fault_env *env = &fault_env;

    //Cache Status Registers;
    env->MMFSR = *ARMV7_MMFSR;
    env->BFSR = *ARMV7_BFSR;
    env->UFSR = *ARMV7_UFSR;
    env->HFSR = *ARMV7_HFSR;
    env->DFSR = *ARMV7_DFSR;
    env->MMFAR = *ARMV7_MMFAR;
    env->BFAR = *ARMV7_BFAR;

    //Clear Status Registers;
    *ARMV7_MMFSR = (uint8_t) -1;
    *ARMV7_BFSR = (uint8_t) -1;
    *ARMV7_UFSR = (uint16_t) -1;
    *ARMV7_HFSR = (uint32_t) -1;
    *ARMV7_DFSR = (uint32_t) -1;

}


/**
 * __flt_repair : attempts to repair a fault, described by the up-to-date fault environment;
 * @return true if the repairs are successful, false if the fault is not recoverable;
 */

bool __flt_repair() {

    //TODO IMPLEMENT;
    return false;

}


/**
 * LOG_FAULT : if bits set in @mask are set in @var, then print @msg;
 */

#define LOG_FAULT(var, mask, msg) {if ((var) & (mask)) {printk(msg);}}


/**
 * log_MMFSR : detailed log for MMFSR;
 */

static void log_MMFSR(const uint8_t MMFSR, const uint32_t MMFAR) {

    //instruction access violation;
    LOG_FAULT(MMFSR, MMFSR_IACCVIOL, "Instruction access violation");

    //Data access violation;
    if (MMFSR & MMFSR_DACCVIOL) {

        //the MMFAR has valid content :
        if (MMFSR & MMFSR_MMARKVALID) {

            //Log;
            printkf("Data access violation : %h", MMFAR);

        } else {

            //MMFAR has invalid content, log;
            printk("Data access violation, invalid memory indicator");

        }

    }

    //mem manage error on exception return;
    LOG_FAULT(MMFSR, MMFSR_MUNSTKERR, "Derived MemManage fault on exception return");

    //mem manage error on exception entry;
    LOG_FAULT(MMFSR, MMFSR_MSTKERR, "Derived MemManage fault on exception entry");

}


/**
 * log_BFSR : detailed log for BFSR;
 */

static void log_BFSR(const uint8_t BFSR, const uint32_t BFAR) {

    //bus fault on instruction prefetch
    LOG_FAULT(BFSR, BFSR_IBUSERR,"Bus fault on instruction prefetch");

    //precise data access error :
    if (BFSR & BFSR_PRECISERR) {

        //the BFAR has valid content :
        if (BFSR & MMFSR_MMARKVALID) {

            //Log;
            printkf("Bus precise data access error : %h", BFAR);


        } else {

            //MMFAR has invalid content, log;
            printk("Bus precise data error, invalid memory indicator");

        }

    }

    //imprecise data access error :
    LOG_FAULT(BFSR, BFSR_IMPRECISERR,"Bus imprecise data access error");

    //derived bus fault on exception return :
    LOG_FAULT(BFSR, BFSR_UNSTKERR, "Derived bus fault on exception return");

    //derived bus fault on exception entry:
    LOG_FAULT(BFSR, BFSR_STKERR, "Derived bus fault on exception entry");

}



/**
 * log_UFSR : detailed log for UFSR;
 */

static void log_UFSR(const uint16_t UFSR) {

    //an undefined instruction is found :
    LOG_FAULT(UFSR, UFSR_UNDEFINSTR, "Undefined instruction");

    //invalid EPSR.T bit or illegal EPSR.IT bits for executing instruction :
    LOG_FAULT(UFSR, UFSR_INVSTATE, "invalid EPSR.T bit or illegal EPSR.IT bits");

    //integrity check error on EXEC_RETURN :
    LOG_FAULT(UFSR, UFSR_INVPC, "integrity check error on EXEC_RETURN");

    //coprocessor access error :
    LOG_FAULT(UFSR, UFSR_NOCP, "coprocessor access error");

    //unaligned access error :
    LOG_FAULT(UFSR, UFSR_UNALIGNED, "unaligned access error");

    //Division by zero :
    LOG_FAULT(UFSR, UFSR_DIVBYZERO, "Division by zero");

}


/**
 * log_HFSR : detailed log for HFSR;
 */

static void log_HFSR(const uint32_t HFSR) {

    //vector table read error;
    LOG_FAULT(HFSR, HFSR_VECTTBL, "error reading the vector table");

    //vector table read error;
    LOG_FAULT(HFSR, HFSR_FORCED, "Configurable fault disabled, escalated in hardfault");

    //vector table read error;
    LOG_FAULT(HFSR, HFSR_DEBUGEVT, "Debug event");

}


/**
 * __flt_log : displays a detailed log for all fault registers;
 */

void __flt_log() {

    uint32_t reg32;
    uint16_t reg16;
    uint8_t reg8;
    struct arm_v7m_fault_env *env;

    //Cache the ref of the fault env;
    env = &fault_env;

    //If Memory Management fault, detailed log;
    if ((reg8 =  env->MMFSR)) log_MMFSR(reg8, env->MMFAR);

    //If Bus fault, detailed log;
    if ((reg8 = env->BFSR)) log_BFSR(reg8, env->BFAR);

    //If Usage fault, detailed log;
    if ((reg16 = env->UFSR))log_UFSR(reg16);

    //If Hard fault, detailed log;
    if ((reg32 = env->HFSR)) log_HFSR(reg32);

}