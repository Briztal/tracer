/*
  arm_v7m_base.c Part of TRACER

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
 * This source file contains the base code make the kernel work properly in loop mode.
 * 	Code related to preemption, syscalls (advanced mode) can be found in arm_v7m_adv.c
 * 	All other components (FPU, MPU, etc...) included in modules;
 */


//------------------------------------------------ Make parameters -----------------------------------------------

/*
 * Makefile must provide :
 * 	- NB_EXCEPTIONS : the number of interrupts supported by the chip.
 * 	- NVIC_RELOCATION : 1 or 0, depending on if the nvic vector table will be relocated or not;
 * 	- EXEC_LEVEL : the level of execution of the kernel;
 */


#if !defined(NB_EXCEPTIONS) || !defined(NVIC_RELOC) || !defined(EXEC_LEVEL)

#error "The number of interrupts was not provided. Check your makefile;"

#define NB_EXCEPTIONS 256

#define NVIC_RELOC 1

#define EXEC_LEVEL 1

#endif


//------------------------------------------------ Includes -----------------------------------------------

#include <stdint.h>
#include <kernel/async/interrupt.h>
#include <kernel/clock/clock.h>
#include <kernel/panic.h>
#include "arm_v7m.h"


//---------------------------------------------------- ARM_V7M Init ----------------------------------------------------

static void __arm_v7m_init() {
	
	//Enable all faults;
	
	armv7m_enable_bus_fault();
	armv7m_enable_usage_fault();
	armv7m_enable_mem_fault();
	
	armv7m_set_mem_fault_priority(0);
	armv7m_set_bus_fault_priority(0);
	armv7m_set_usage_fault_priority(0);
	
	//Call the sched init function;
	__proc_init();
	
}


//------------------------------------------------------- Faults -------------------------------------------------------

static void arm_v7m_hard_fault_handler() {
	__krnl_handle_fault(0);
}


static void arm_v7m_bus_fault_handler() {
	__krnl_handle_fault(1);
}


static void arm_v7m_mem_fault_handler() {
	__krnl_handle_fault(2);
}


static void arm_v7m_usg_fault_handler() {
	__krnl_handle_fault(2);
}


//----------------------------------------------------- Preemption -----------------------------------------------------

//Set the priority of the preemption exception and enable it;
void __prmpt_configure(uint8_t priority) {
	armv7m_set_pendsv_priority(priority);
}

//Set the preemption exception pending;
void __prmpt_set_pending() {
	armv7m_set_pendsv_pending();
}

//Set the preemption exception not pending;
void __prmpt_clear_pending() {
	armv7m_clr_pendsv_pending();
}



//----------------------------------------------------- Syscall -----------------------------------------------------

//Set the handler of the syscall exception;
void __syscall_set_handler(void (*handler)(void)) {
}

//Set the priority of the syscall exception;
void __syscall_set_priority(uint8_t priority) {
	armv7m_set_svcall_priority(priority);
}

//Enable the syscall exception;
void __syscall_enable() {
	//Always enabled;
}

/**
 * __syscall_trigger : triggers the syscall exception;
 *
 * 	This function is pure assembly, and follows the ARM Calling Convention;
 *
 * @param syscall_id : the identifier of the system call to execute. Will be located on R0 at handler entry;
 */

__attribute__ ((naked)) uint32_t __syscall_trigger(uint32_t syscall_id, uint32_t arg0, uint32_t arg1, uint32_t arg2) {
	__asm__ __volatile (\

	//Trigger the supervisor call; Arguments are already placed in R0, R1, R2 and R3;
	"SVC #0\n\t"
		
		//The supervisor call handler will have placed the return value in R0. We can return now;
		"bx lr\n\t"
	);
}


/**
 * __arm_v7m_syscall_handler : this function handles the SVC exception. It performs the following operations :
 * 	- preparing syscall arguments, namely syscall_id, arg0, arg1 and arg2 for the kernel syscall handler;
 * 	- calling kernel_syscall_handler;
 * 	- Modifying the stacked version of R0 so that the kernel syscall handler's return value
 * 		arrives to __syscall_trigger;
 */

__attribute__ ((naked)) static void __arm_v7m_syscall_handler() {
	__asm__ __volatile__ (""
		
		//Save LR in R4, as BL overwrites it;
		"mov 	r4,		lr 	\n\r"
		
		//R0-R3 have not been altered. We can directly call the kernel syscall handler;
		"bl 	__krnl_syscall_handler\n\r"
		
		//Restore LR;
		"mov 	lr, 	r4	\n\r"
		
		//Cache PSP in R5;
		"mrs	r5, 	psp	\n\r"
		
		//Save R0 in the psp; The psp points to the stacked version of R0;
		"str	r0, 	[r5]\n\r"
		
		//Exit from exception;
		"bx 	lr			\n\r"
	
	);
}


//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;
//TODO KERNEL SYSCALL HANDLER;




//-------------------------------------------------- Stack management  -------------------------------------------------

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
 * proc_init_stack : this function initialises the unstacking environment, so that the given function will
 *  be executed at context switch time;
 *
 *  It starts by caching the process_t stack pointer, and stacks the process functions pointers, and the PSR.
 *
 *  Then, it saves the process index in R12 (next word);
 *
 *  Finally, it leaves space for empty stack frame and saves the stack pointer;
 */

void __proc_create_stack_context(struct proc_stack *stack, void (*function)(), void (*exit_loop)(), void *arg) {
	
	//Cache the current stack pointer;
	uint32_t *sp4 = stack->sp;
	
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
	stack->sp = sp4;
	
}


/*
 * proc_get_init_arg : this function will return the value of r12. If called by the process init function,
 *  it will be equal to the previous function's arg;
 */

__attribute__ ((naked)) void *__proc_get_init_arg() {
	__asm__ __volatile__(\
        "mov 	r0, 	r12 	\n\t"
		"bx	 	lr 				\n\t"
	);
}


//Determine the closest inferior address, that would respect alignment requirements;
void *__proc_stack_align(void *stack_reset) {
	
	//@core_stack_alignment is a power of 2, decrement and we obtain the mask of bits to reset;
	return (void *) ((((size_t) stack_reset) & ~((size_t) 7)));
	
}


//---------------------------------------------------- Threads setup ---------------------------------------------------

/**
 * __proc_enter_thread_mode :
 * 	- changes the current stack from msp to psp;
 * 	- updates control to use psp;
 * 	- updates msp with the provided exception stack;
 * 	- triggers preemption;
 * 	- enables interrupts so that preemption happen;
 *
 * 	This function is pure assembly, and uses ARM calling convention. @exception_stack will be located in R0;
 *
 * @param exception_stack : the stack where exceptions should happen;
 */

__attribute__ ((naked)) void __proc_enter_thread_mode(struct proc_stack *exception_stack) {
	
	__asm__ __volatile__ (\

	//Disable interrupts;
	"cpsid 	i \n\t"
		
		//Load the value of the stack pointer (exception_stack located in R0, arm calling convention);
		"ldr 	r0, 	[r0]	\n\t"
		
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
		"bl 	__prmpt_set_pending\n\t"
		
		//Enable all faults;
		"cpsie 	f				\n\t"
		
		//Enable all interrupts; Preemption should happen;
		"cpsie 	i				\n\t"
		
		//Return. That point should never be reached, if the preemption occurred correctly;
		"bx 	r4				\n\t"
	
	);
	
	
}


//----------------------------------------------------- Preemption -----------------------------------------------------

/*
 * The context switcher :
 *
 *  - saves the current thread context;
 *  - calls the kernel context switcher;
 *  - loads the new thread context;
 *
 *  This function is written in pure assembly, and follows the ARM calling convention;
 */

__attribute__ ((naked)) static void __arm_v7m_context_switcher() {
	
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


//------------------------------------------------- kernel vector table ------------------------------------------------

/*
 * Some data is required to start the processor properly. Namely :
 * 	- The initial stack pointer, provided by the linker;
 * 	- The first function to execute, defined in another piece of code;
 */


//Define an empty ISR handler
static void no_isr() {}

//Define the kernel vtable, and align it on 512 bytes.
void (*__kernel_vtable[NB_EXCEPTIONS])(void) __attribute__ ((aligned (512))) = {
	
	//0 : not assigned;
	&no_isr,
	
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
	
	//11 : SVCall, kernel hook;TODO
	&no_isr,
	
	//12 : Reserved;
	&no_isr,
	
	//13 : Reserved;
	&no_isr,
	
	//14 : PendSV, context switcher, hooking the kernel;
	&__arm_v7m_context_switcher,
	
	//15 : SysTick, kernel hook;
	&__krnl_tick,
	
	
	//In order to avoid writing 254 times the function name, we will use macros that will write it for us;
#define channel(i) &no_isr,

//Redirect all isrs to the empty one;
#include "nvic_channel_list.h"

#undef channel
	
};


//-------------------------------------------------- Interrupt priorities --------------------------------------------------

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
 * irq_reset_interrupt_handler : resets the handler of the required channel to 0, isr will return immediately;
 * @param channel : the interrupt channel to update;
 */

void __irq_reset_interrupt_handler(uint16_t channel) {
	__irq_set_handler(channel, &no_isr);
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
		
		//Do nothing, errors can't be triggered from here; TODO USAGE FAULT;
		return;
		
	}
	
	//If the handler is null, save the empty handler; If not, save the handler;
	__kernel_vtable[irq_channel] = (handler) ? handler : no_isr;
	
}



//-------------------------------------------------- System clock -------------------------------------------------

//Configure the frequency and the priority of the system clock interrupt
extern void __sclk_configure(uint32_t tick_frequency, uint8_t int_prio) {
	
	//Get the core clock value;
	uint32_t core_freq = clock_get("core");
	
	//If the core frequency is null :
	if (!core_freq) {
		
		//Panic. The core clock is not registered;
		kernel_panic("systick_set_frequency : core clock not registered;");
		
	}
	
	//Compute the reload;
	uint32_t reload = core_freq / tick_frequency;
	
	//Update the reload;
	armv7m_systick_set_reload(reload);
	
	//Update the priority;
	armv7m_set_systick_priority(int_prio);
	
}

//Start the system clock;
void __sclk_start() {
	armv7m_systick_select_core_clock();
	armv7m_systick_enable();
	armv7m_systick_int_enable();
	
}

//Stop the system clock;
extern void __sclk_stop() {
	armv7m_systick_disable();
	armv7m_systick_int_disable();
}



//-------------------------------------------------- NVIC static base --------------------------------------------------

//If the vtable will be relocated, it must not be generated. This gives a smaller executable:
#if (NVIC_RELOC > 0)

/*
 * In order to start the processor properly, we define here the vector table, that is hard-copied in the firmware
 * 	as it, at the link section .vector. This section can be found in the link script, and starts at address 0;
 */

void (*vtable[NB_EXCEPTIONS])(void) __attribute__ ((section(".vectors"))) = {
	
	//0 : not assigned;
	(void (*)(void)) &__ram_max,
	
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
	
	//11 : SVCall, kernel hook;TODO
	&no_isr,
	
	//12 : Reserved;
	&no_isr,
	
	//13 : Reserved;
	&no_isr,
	
	//14 : PendSV, context switcher, hooking the kernel;
	&__arm_v7m_context_switcher,
	
	//15 : SysTick, kernel hook;
	&__krnl_tick,
	
	//In order to avoid writing 254 times the function name, we will use macros that will write it for us;
#define channel(i) &no_isr,

//Redirect all isrs to the empty one;
#include "nvic_channel_list.h"

#undef channel
	
	//Adding another "&no_isr" will cause a compiler warning "excess array initializer. Try it, it is funny !
	
};


//If the vtable must be generated in flash :
#else //(NVIC_RELOC > 0)


/**
 * isr_generic_handler : in order to support handler update, all functions of the in-flash vector table will
 * 	lead to this function, that executes the appropriate function;
 *
 * @param i : the interrupt channel. 0 to 240;
 */

static void isr_generic_flash_handler(uint8_t i) {
	
	//Execute the handler;
	(*__kernel_vtable[i])();
	
}


/*
 * Generate an ISR for each interrupt channel; Done using XMacro;
 */

//The handler link : a function that calls the handler link with a specific value;
#define channel(i) static void isr_##i() {isr_generic_flash_handler(i);}

//Define all isrs;
#include "nvic_channel_list.h"

#undef channel

/*
 * In order to start the processor properly, we define here the vector table, that is hard-copied in the firmware
 * 	as it, at the link section .vector. This section can be found in the link script, and starts at address 0;
 */

void (*vtable[NB_EXCEPTIONS])(void)  __attribute__ ((section(".vectors"))) = {
	
	//0 : not assigned;
	&_ram_highest,
	
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
	
	//11 : SVCall, kernel hook;TODO
	&no_isr,
	
	//12 : Reserved;
	&no_isr,
	
	//13 : Reserved;
	&no_isr,
	
	//14 : PendSV, context switcher, hooking the kernel;
	&__arm_v7m_context_switcher,
	
	//15 : SysTick, kernel hook;
	&__krnl_tick,
	
	//In order to avoid writing 254 times the function name, we will use macros that will write it for us;
#define channel(i) &isr_##i,

//Redirect all isrs to the empty one;
#include "nvic_channel_list.h"

#undef channel
	
	//Adding another "&empty_isr" will cause a compiler warning "excess array initializer. Try it, it is funny !
	
};


#endif //(NVIC_RELOC > 0)

