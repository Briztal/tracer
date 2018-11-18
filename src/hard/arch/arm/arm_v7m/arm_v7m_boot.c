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

//------------------------------------------------ Make parameters -----------------------------------------------

/*
 * mk64fx512.mk must provide :
 * 	- NB_EXCEPTIONS : the number of interrupts supported by the chip.
 * 	- NVIC_RELOCATION : 1 or 0, depending on if the nvic vector table will be relocated or not;
 * 	- EXEC_LEVEL : the level of execution of the kernel;
 */


#if !defined(NB_EXCEPTIONS) || !defined(NVIC_RELOC)

#error "The number of exceptions, or the nvic relocation flag was not provided. Check your makefile;"

#define NB_EXCEPTIONS 256

#define NVIC_RELOC 1

#endif


//------------------------------------------------------ Includes ------------------------------------------------------

#include <stdint.h>
#include <async/except.h>
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
	
	//Call the run init function;
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



//----------------------------------------------------- Syscall -----------------------------------------------------

//Set the priority of the syscall exception and enable it;
void __syscl_configure(uint8_t priority) {
	armv7m_set_svcall_priority(priority);
}


/**
 * __syscall_trigger : triggers the syscall exception;
 *
 * 	This function is pure assembly, and follows the ARM Calling Convention;
 *
 * @param syscall_id : the identifier of the system call to execute. Will be located on R0 at handler entry;
 */

__attribute__ ((naked)) size_t __syscl_trigger(size_t arg0, size_t arg1, size_t arg2, size_t syscall_id) {
	__asm__ __volatile (\

	//Trigger the supervisor call; Arguments are already placed in R0, R1, R2 and R3;
	"SVC #0\n\t"
		
		//The supervisor call handler will have placed the return value in R0. We can return now;
		"bx lr\n\t"
	);
}


/**
 * syscl_handler : this function handles the SVC exception. It performs the following operations :
 * 	- preparing syscall arguments, namely syscall_id, arg0, arg1 and arg2 for the kernel syscall handler;
 * 	- calling kernel_syscall_handler;
 * 	- Modifying the stacked version of R0 so that the kernel syscall handler's return value
 * 		arrives to __syscall_trigger;
 */

__attribute__ ((naked)) static void syscl_handler() {
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




//----------------------------------------------------- Preemption -----------------------------------------------------

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


//------------------------------------------------- kernel vector table ------------------------------------------------

/*
 * Some data is required to start the processor properly. Namely :
 * 	- The initial stack_data pointer, provided by the linker;
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
	
	//11 : SVCall, syscall handler, hooking the kernel;
	&syscl_handler,
	
	//12 : Reserved;
	&no_isr,
	
	//13 : Reserved;
	&no_isr,
	
	//14 : PendSV, context switcher, hooking the kernel;
	&arm_v7m_prmpt_handler,
	
	//15 : SysTick, kernel hook;
	&__krnl_tick,
	
	
	//In order to avoid writing 254 times the function name, we will use macros that will write it for us;
#define channel(i) &no_isr,

//Redirect all isrs to the empty one;
#include "nvic_channel_list.h"

#undef channel
	
};



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
	
	//11 : SVCall, syscall handler, hooking the kernel;
	&syscl_handler,
	
	//12 : Reserved;
	&no_isr,
	
	//13 : Reserved;
	&no_isr,
	
	//14 : PendSV, context switcher, hooking the kernel;
	&arm_v7m_prmpt_handler,
	
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
	
	//11 : SVCall, syscall handler, hooking the kernel;
	&syscl_handler,
	
	//12 : Reserved;
	&no_isr,
	
	//13 : Reserved;
	&no_isr,
	
	//14 : PendSV, context switcher, hooking the kernel;
	&__arm_v7m_prmpt_handler,
	
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

