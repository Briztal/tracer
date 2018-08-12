//
// Created by root on 8/12/18.
//

#ifndef TRACER_ARM_V7_H
#define TRACER_ARM_V7_H

#include <stdbool.h>

#include <stdint.h>

/*
 * This file summarises registers location described in the ARMV7 Reference Manual, and implements system functions
 * 	over these;
 */


/*
 * -------------------------------------------- System Control And ID Blocks -------------------------------------------
 */

//----------------- ICTR - RO: Interrupt Controller Type Register -----------------

#define ARMV7_ICTR ((volatile uint32_t *) 0xE000E004)

//TODO PAGE B3-32



//----------------- ACTLR - RW : Auxiliary Control Register -----------------

#define ARMV7_ACTLR ((volatile uint32_t *) 0xE000E008)

//The content of this register is IMPLEMENTATION_DEFINED



//----------------- CPUID - RO : CPUID Base Register -----------------

#define ARMV7_CPUID ((volatile uint32_t *) 0xE000ED00)

//TODO APPENDIX A;


//----------------- ICSR - RW : Interrupt Control State Register, page 496 -----------------

#define ARMV7_ICSR ((volatile uint32_t *) 0xE000ED04)

//Trigger an NMI; RW
#define ARMV7_ICSR_NMIPENDSET ((uint32_t) (1 << 31))


//Set the PendSV interrupt pending; Reads 1 if pending. RW;
#define ARMV7_ICSR_PENDSVSET ((uint32_t) (1 << 28))

//TODO FUNCTIONS TO ENABLE AND DISABLE;
//Write 1 to clear the PendSV interrupt. WO;
#define ARMV7_ICSR_PENDSVCLR ((uint32_t) (1 << 27))


//Set the systick interrupt pending; Reads 1 if pending. RW;
#define ARMV7_ICSR_PENDSTSET ((uint32_t) (1 << 26))
//TODO FUNCTIONS TO ENABLE AND DISABLE;
//Write 1 to clear the systick interrupt. WO;
#define ARMV7_ICSR_PENDSTCLR ((uint32_t) (1 << 25))


//If set, a pending exception will be serviced on exit from the debug halt state;
#define ARMV7_ICSR_ISRPREEMPT ((uint32_t) (1 << 23))

//Indicates if an external configurable (NVIC generated) interrupt is pending;
#define ARMV7_ICSR_ISRPENDING ((uint32_t) (1 << 22))

//Mask to get the exception number, RO; TODO FUNCTION ?
#define ARMV7_ICSR_VECTPENDING ((uint32_t) (((uint32_t) (0x1FF)) << 12))

//RETTOBASE TODO WHAT IS THE USE FOR IT ?
#define ARMV7_ICSR_RETTOBASE ((uint32_t) (1<<11));

//Get the number of the current exception;
#define ARMV7_ICSR_VECTACTIVE ((uint32_t) 0x1FF)


/**
 * armv7_trigger_nmi : sets NMI_PENDSET of ICSR to trigger an NMI
 */

static inline void armv7_trigger_nmi() {
	*ARMV7_ICSR |= ARMV7_ICSR_NMIPENDSET;
}


/**
 * armv7_set_pendsv_pending : sets PendSV IRQ pending;
 */

static inline void armv7_set_pendsv_pending() {
	*ARMV7_ICSR |= ARMV7_ICSR_PENDSVSET;
}


/**
 * armv7_set_pendsv_pending : returns true if PendSV IRQ is pending;
 */

static inline bool armv7_is_pendsv_pending() {
	return (bool)(*ARMV7_ICSR & ARMV7_ICSR_PENDSVSET);
}


/**
 * armv7_clr_pendsv_pending : sets PendSV IRQ not pending;
 */

static inline void armv7_clr_pendsv_pending() {
	*ARMV7_ICSR |= ARMV7_ICSR_PENDSVCLR;
}


/**
 * armv7_set_pendsv_pending : sets systick IRQ pending;
 */

static inline void armv7_set_systick_pending() {
	*ARMV7_ICSR |= ARMV7_ICSR_PENDSTSET;
}


/**
 * armv7_set_pendsv_pending : returns true if systick IRQ is pending;
 */

static inline bool armv7_is_systick_pending() {
	return (bool)(*ARMV7_ICSR & ARMV7_ICSR_PENDSTSET);
}


/**
 * armv7_clr_pendsv_pending : sets systick IRQ not pending;
 */

static inline void armv7_clr_systick_pending() {
	*ARMV7_ICSR |= ARMV7_ICSR_PENDSTCLR;
}


/**
 * armv7_get_exception_number :
 * @return the number of the currently happening exception;
 */

static inline uint16_t armv7_get_exception_number() {
	return (uint16_t) (*ARMV7_ICSR & ARMV7_ICSR_VECTACTIVE);
}





//----------------- VTOR - RW : Vetcor Table Offset Register, page 497 -----------------

#define ARMV7_VTOR ((volatile uint32_t *) 0xE000ED08)

//Relocate the vector table; Asserts if relocation succeeded;
bool arm_v7_relocate_vector_table(bool in_ram, uint32_t offset);


//----------------- AIRCR - RW : Application Interrupt / Reset Control Register, page 498 -----------------

#define ARMV7_AIRCR ((volatile uint32_t *) 0xE000ED0C)


//----------------- SCR - RW : System Control Register, page 499-----------------

#define ARMV7_SCR ((volatile uint32_t *) 0xE000ED10)


//If set, interrupt transitions from inactive to pending are included in the list of wake up events;
#define ARMV7_SCR_SEVONPEND ((uint32_t) (1<<4))

//Qualifying hint indicating that waking from sleep might take longer; Can be used to identify a low power sleep rate;
#define ARMV7_SCR_SLEEPDEEP ((uint32_t) (1<<2))

//Enter in sleep when rettobase;
#define ARMV7_SCR_SLEEPONEXIT ((uint32_t) (1<<1))


//----------------- CCR - RW : Configuration Control Register, page 500 -----------------

#define ARMV7_CCR ((volatile uint32_t *) 0xE000ED14)

//If set, 8 byte alignment is guaranteed when entering an exception. If not, 4 byte alignment is guaranteed;
#define ARMV7_CCR_STCKALIGN ((uint32_t) (1 << 9))

//TODO
#define ARMV7_CCR_BFHFNMIGN ((uint32_t) (1 << 8))

//Enable bit for trap on divide by 0;
#define ARMV7_CCR_DIV_0_TRP ((uint32_t) (1 << 4))

//Enable bit for trap on unaligned word / halfword access;
#define ARMV7_CCR_UNALIGN_TRP ((uint32_t) (1 << 3))


//Enable bit for authorising unprivileged code to write the STIR register;
#define ARMV7_CCR_USERSETMPEND ((uint32_t) (1 << 1))


//Enable to authorise entering thread state at any level;
#define ARMV7_CCR_NONBASETHRDENA ((uint32_t) (1 << 0))




//----------------- SHPR1 - RW : System Handlers 4 - 7 priority registers, page 501 -----------------

#define ARMV7_SHPR1 ((volatile uint32_t *) 0xE000ED18)


/**
 * armv7_set_memmanage_priority : Updates SHPR1 set the priority of the memmanage system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7_set_memmanage_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR1) = priority;
}


/**
 * armv7_set_bus_fault_priority : Updates SHPR1 set the priority of the bus fault system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7_set_bus_fault_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR1 + 1) = priority;
}


/**
 * armv7_set_usage_fault_priority : Updates SHPR1 set the priority of the usage fault system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7_set_usage_fault_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR1 + 2) = priority;
}


//----------------- SHPR2 - RW : System Handlers 8 - 11 priority registers, page 501 -----------------

#define ARMV7_SHPR2 ((volatile uint32_t *) 0xE000ED1C)


/**
 * armv7_set_svcall_priority : Updates SHPR2 set the priority of the supervisor call system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7_set_svcall_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR2 + 3) = priority;
}


//----------------- SHPR3 - RW : System Handlers 12 - 15 priority registers, page 501 -----------------

#define ARMV7_SHPR3 ((volatile uint32_t *) 0xE000ED20)

/**
 * armv7_set_debug_monitor_priority : Updates SHPR2 set the priority of the debug monitor system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7_set_debug_monitor_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR2) = priority;
}


/**
 * armv7_set_pendsv_priority : Updates SHPR2 set the priority of the PendSV system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7_set_pendsv_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR2 + 2) = priority;
}



/**
 * armv7_set_systick_priority : Updates SHPR2 set the priority of the systick system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7_set_systick_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR2 + 3) = priority;
}


//----------------- SHCSR - RW : System Handler Control and State Register -----------------

#define ARMV7_SHCSR ((volatile uint32_t *) 0xE000ED24)

#define ARMV7_SHCSR_USGFAULTENA ((uint32_t) (1 << 18))
#define ARMV7_SHCSR_BUSFAULTENA ((uint32_t) (1 << 17))
#define ARMV7_SHCSR_MEMFAULTENA ((uint32_t) (1 << 16))
#define ARMV7_SHCSR_SVCALLPENDED ((uint32_t) (1 << 15))
#define ARMV7_SHCSR_BUSFAULTPENDED ((uint32_t) (1 << 14))
#define ARMV7_SHCSR_MEMFAULTPENDED ((uint32_t) (1 << 13))
#define ARMV7_SHCSR_USGFAULTPENDED ((uint32_t) (1 << 12))
#define ARMV7_SHCSR_SYSTICKACT ((uint32_t) (1 << 11))
#define ARMV7_SHCSR_PENDSVACT ((uint32_t) (1 << 10))
#define ARMV7_SHCSR_MONITORACT ((uint32_t) (1 << 8))
#define ARMV7_SHCSR_SVCALLACT ((uint32_t) (1 << 7))
#define ARMV7_SHCSR_USGFAULTACT ((uint32_t) (1 << 3))
#define ARMV7_SHCSR_BUSFAULTACT ((uint32_t) (1 << 1))
#define ARMV7_SHCSR_MEMFAULTACT ((uint32_t) (1 << 0))



/**
 * armv7_enable_usage_fault : enables the usage fault exception;
 */

static inline void armv7_enable_usage_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_USGFAULTENA;
}


/**
 * armv7_usage_fault_enabled : Asserts if the usage fault exception is enabled;
 */

static inline bool armv7_usage_fault_enabled() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_USGFAULTENA);
}


/**
 * armv7_disable_usage_fault : disables the usage fault exception;
 */

static inline void armv7_disable_usage_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_USGFAULTENA;
}


/**
 * armv7_enable_bus_fault : enables the usage fault exception;
 */

static inline void armv7_enable_bus_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_BUSFAULTENA;
}


/**
 * armv7_bus_fault_enabled : Asserts if the usage fault exception is enabled;
 */

static inline bool armv7_bus_fault_enabled() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_BUSFAULTENA);
}


/**
 * armv7_disable_bus_fault : disables the usage fault exception;
 */

static inline void armv7_disable_bus_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_BUSFAULTENA;
}



/**
 * armv7_enable_mem_fault : enables the usage fault exception;
 */

static inline void armv7_enable_mem_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_MEMFAULTENA;
}


/**
 * armv7_mem_fault_enabled : Asserts if the usage fault exception is enabled;
 */

static inline bool armv7_mem_fault_enabled() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_MEMFAULTENA);
}


/**
 * armv7_disable_mem_fault : disables the usage fault exception;
 */

static inline void armv7_disable_mem_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_MEMFAULTENA;
}


/**
 * armv7_svc_pending :
 * @return true if the svc exception is pending;
 */

static inline bool armv7_svc_pending() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_SVCALLPENDED);
}


/**
 * armv7_bus_fault_pending :
 * @return true if the bus fault exception is pending;
 */

static inline bool armv7_bus_fault_pending() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_BUSFAULTPENDED);
}


/**
 * armv7_mem_fault_pending :
 * @return true if the mem fault exception is pending;
 */

static inline bool armv7_mem_fault_pending() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_MEMFAULTPENDED);
}


/**
 * armv7_usage_fault_pending :
 * @return true if the usage fault exception is pending;
 */

static inline bool armv7_usage_fault_pending() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_USGFAULTPENDED);
}


/**
 * armv7_systick_active :
 * @return true if the systick exception is active;
 */

static inline bool armv7_systick_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_SYSTICKACT);
}


/**
 * armv7_pendsv_active :
 * @return true if the PendSV exception is active;
 */

static inline bool armv7_pendsv_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_PENDSVACT);
}


/**
 * armv7_monitor_active :
 * @return true if the monitor is active;
 */

static inline bool armv7_monitor_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_MONITORACT);
}


/**
 * armv7_svc_active :
 * @return true if the svc exception is active;
 */

static inline bool armv7_svc_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_SVCALLACT);
}


/**
 * armv7_usage_fault_active :
 * @return true if the usage fault exception is active;
 */

static inline bool armv7_usage_fault_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_USGFAULTACT);
}


/**
 * armv7_bus_fault_active :
 * @return true if the bus fault exception is active;
 */

static inline bool armv7_bus_fault_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_BUSFAULTACT);
}


/**
 * armv7_mem_fault_active :
 * @return true if the mem fault exception is active;
 */

static inline bool armv7_mem_fault_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_MEMFAULTACT);
}




//----------------- CFSR - RW : Configurable Fault Status Register -----------------

#define ARMV7_CFSR (volatile uint32_t *) 0xE000ED28

//TODO LIB


//----------------- HFSR - RW : Hard Fault Status Register -----------------

#define ARMV7_HFSR (volatile uint32_t *) 0xE000ED2C)


//----------------- DFSR - RW : Debug Fault Status Register -----------------

#define ARMV7_DFSR ((volatile uint32_t *) 0xE000ED30)


//----------------- MMFAR - RW : MemManage Fault Address Register -----------------

#define ARMV7_MMFAR ((volatile uint32_t *) 0xE000ED34)


//----------------- BFAR - RW : Bus Fault Address Register -----------------

#define ARMV7_BFAR ((volatile uint32_t *) 0xE000ED38)


//----------------- AFSR - RW : Auxiliary Fault Status Register -----------------

#define ARMV7_AFSR ((volatile uint32_t *) 0xE000ED3C)



/*
 * 0xE000ED40 -> 7F RESERVED FOR CPUID TABLE
 */


//----------------- CPACR - RW : Coprocessor Access Control Register -----------------

#define ARMV7_CPACR ((volatile uint32_t *) 0xE000ED88)



//----------------- STIR - WO : Software Trigger Interrupt Register -----------------

#define ARMV7_STIR ((volatile uint32_t *) 0xE000EF00)

#define ARMV7_STIR_INTID ((uint32_t)0x1FF);


/**
 * armv7_software_trigger_interrupt : writes STIR to trigger the required interrupt;
 *
 * 	The value provided is an interrupt number, not an exception number. System exceptions cannot be triggered;
 *
 * @param interrupt_number : the interrupt number;
 */

static inline void armv7_software_trigger_interrupt(uint16_t interrupt_number) {

	*ARMV7_STIR = (uint32_t)interrupt_number & ARMV7_STIR_INTID;

}



//----------------- PIDx - RO : Peripheral Identification Register -----------------

#define ARMV7_PID4 ((volatile uint32_t *) 0xE000EFD0)
#define ARMV7_PID5 ((volatile uint32_t *) 0xE000EFD4)
#define ARMV7_PID6 ((volatile uint32_t *) 0xE000EFD8)
#define ARMV7_PID7 ((volatile uint32_t *) 0xE000EFDC)

#define ARMV7_PID0 ((volatile uint32_t *) 0xE000EFE0)
#define ARMV7_PID1 ((volatile uint32_t *) 0xE000EFE4)
#define ARMV7_PID2 ((volatile uint32_t *) 0xE000EFE8)
#define ARMV7_PID3 ((volatile uint32_t *) 0xE000EFEC)



//----------------- CIDx - RO : Component Identification Register -----------------

#define ARMV7_CID0 ((volatile uint32_t *) 0xE000EFF0)
#define ARMV7_CID1 ((volatile uint32_t *) 0xE000EFF4)
#define ARMV7_CID2 ((volatile uint32_t *) 0xE000EFF8)
#define ARMV7_CID3 ((volatile uint32_t *) 0xE000EFFC)





/*
 * ------------------------------------------------------ Systick ------------------------------------------------------
 */

//TODO;

#endif //TRACER_ARM_V7_H
