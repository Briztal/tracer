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
#include <kernel/hard/except.h>
#include "../arm_v7m.h"


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
#include "../nvic_channel_list.h"

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
#include "../nvic_channel_list.h"

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

