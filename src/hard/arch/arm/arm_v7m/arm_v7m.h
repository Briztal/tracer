//
// Created by root on 8/12/18.
//

#ifndef TRACER_ARM_V7_H
#define TRACER_ARM_V7_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

/*
 * This file summarises registers location described in the ARMV7 Reference Manual, and implements system functions
 * 	over these;
 */


/*
 * -------------------------------------------- System Control And ID Blocks -------------------------------------------
 */

//----------------- ICTR - RO: Interrupt Controller Type Register -----------------

#define ARMV7_ICTR ((volatile uint32_t *) 0xE000E004)

#define ARMV7_ICTR_INTLINESNUM_MASK ((uint32_t) 0x0F)


/**
 * armv7m_get_nb_interrupts : manipulates ICTR to determine the number of interrupts supported by the microcontroller;
 *
 * 	@return the maximal number of interrupt channels of the microcontroller;
 */

static inline uint16_t armv7m_get_nb_interrupts() {

	//Cache the 4 LSB of ICTR and multiply by 32, by shifting left 5 times;
	return (uint16_t) ((uint16_t)(*ARMV7_ICTR) & (uint16_t) ARMV7_ICTR_INTLINESNUM_MASK) << 5;

}


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


//If set, a pending exception will be serviced on prempt from the debug halt state;
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
 * armv7m_trigger_nmi : sets NMI_PENDSET of ICSR to trigger an NMI
 */

static inline void armv7m_trigger_nmi() {
	*ARMV7_ICSR |= ARMV7_ICSR_NMIPENDSET;
}


/**
 * armv7m_set_pendsv_pending : sets PendSV IRQ pending;
 */

static inline void armv7m_set_pendsv_pending() {
	*ARMV7_ICSR |= ARMV7_ICSR_PENDSVSET;
}


/**
 * armv7m_set_pendsv_pending : returns true if PendSV IRQ is pending;
 */

static inline bool armv7m_is_pendsv_pending() {
	return (bool)(*ARMV7_ICSR & ARMV7_ICSR_PENDSVSET);
}


/**
 * armv7m_clr_pendsv_pending : sets PendSV IRQ not pending;
 */

static inline void armv7m_clr_pendsv_pending() {
	*ARMV7_ICSR |= ARMV7_ICSR_PENDSVCLR;
}


/**
 * armv7m_set_systick_pending : sets systick IRQ pending;
 */

static inline void armv7m_set_systick_pending() {
	*ARMV7_ICSR |= ARMV7_ICSR_PENDSTSET;
}


/**
 * armv7m_is_systick_pending : returns true if systick IRQ is pending;
 */

static inline bool armv7m_is_systick_pending() {
	return (bool)(*ARMV7_ICSR & ARMV7_ICSR_PENDSTSET);
}


/**
 * armv7m_clr_systick_pending : sets systick IRQ not pending;
 */

static inline void armv7m_clr_systick_pending() {
	*ARMV7_ICSR |= ARMV7_ICSR_PENDSTCLR;
}


/**
 * armv7m_get_exception_number :
 * @return the number of the currently happening exception;
 */

static inline uint16_t armv7m_get_exception_number() {
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
 * armv7m_set_memmanage_priority : Updates SHPR1 set the priority of the memmanage system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7m_set_mem_fault_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR1) = priority;
}


/**
 * armv7m_set_bus_fault_priority : Updates SHPR1 set the priority of the bus fault system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7m_set_bus_fault_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR1 + 1) = priority;
}


/**
 * armv7m_set_usage_fault_priority : Updates SHPR1 set the priority of the usage fault system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7m_set_usage_fault_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR1 + 2) = priority;
}


//----------------- SHPR2 - RW : System Handlers 8 - 11 priority registers, page 501 -----------------

#define ARMV7_SHPR2 ((volatile uint32_t *) 0xE000ED1C)


/**
 * armv7m_set_svcall_priority : Updates SHPR2 set the priority of the supervisor call system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7m_set_svcall_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR2 + 3) = priority;
}


//----------------- SHPR3 - RW : System Handlers 12 - 15 priority registers, page 501 -----------------

#define ARMV7_SHPR3 ((volatile uint32_t *) 0xE000ED20)

/**
 * armv7m_set_debug_monitor_priority : Updates SHPR2 set the priority of the debug monitor system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7m_set_debug_monitor_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR2) = priority;
}


/**
 * armv7m_set_pendsv_priority : Updates SHPR2 set the priority of the PendSV system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7m_set_pendsv_priority(uint8_t priority) {
	*((uint8_t *) ARMV7_SHPR2 + 2) = priority;
}



/**
 * armv7m_set_systick_priority : Updates SHPR2 set the priority of the systick system exception;
 *
 * @param priority : the priority to give;
 */

static inline void armv7m_set_systick_priority(uint8_t priority) {
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
 * armv7m_enable_usage_fault : enables the usage fault exception;
 */

static inline void armv7m_enable_usage_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_USGFAULTENA;
}


/**
 * armv7m_usage_fault_enabled : Asserts if the usage fault exception is enabled;
 */

static inline bool armv7m_is_usage_fault_enabled() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_USGFAULTENA);
}


/**
 * armv7m_disable_usage_fault : disables the usage fault exception;
 */

static inline void armv7m_disable_usage_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_USGFAULTENA;
}


/**
 * armv7m_enable_bus_fault : enables the usage fault exception;
 */

static inline void armv7m_enable_bus_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_BUSFAULTENA;
}


/**
 * armv7m_bus_fault_enabled : Asserts if the usage fault exception is enabled;
 */

static inline bool armv7m_is_bus_fault_enabled() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_BUSFAULTENA);
}


/**
 * armv7m_disable_bus_fault : disables the usage fault exception;
 */

static inline void armv7m_disable_bus_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_BUSFAULTENA;
}



/**
 * armv7m_enable_mem_fault : enables the usage fault exception;
 */

static inline void armv7m_enable_mem_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_MEMFAULTENA;
}


/**
 * armv7m_mem_fault_enabled : Asserts if the usage fault exception is enabled;
 */

static inline bool armv7m_is_mem_fault_enabled() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_MEMFAULTENA);
}


/**
 * armv7m_disable_mem_fault : disables the usage fault exception;
 */

static inline void armv7m_disable_mem_fault() {
	*ARMV7_SHCSR |= ARMV7_SHCSR_MEMFAULTENA;
}


/**
 * armv7m_svc_pending :
 * @return true if the svc exception is pending;
 */

static inline bool armv7m_is_svc_pending() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_SVCALLPENDED);
}


/**
 * armv7m_bus_fault_pending :
 * @return true if the bus fault exception is pending;
 */

static inline bool armv7m_is_bus_fault_pending() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_BUSFAULTPENDED);
}


/**
 * armv7m_mem_fault_pending :
 * @return true if the mem fault exception is pending;
 */

static inline bool armv7m_is_mem_fault_pending() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_MEMFAULTPENDED);
}


/**
 * armv7m_usage_fault_pending :
 * @return true if the usage fault exception is pending;
 */

static inline bool armv7m_is_usage_fault_pending() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_USGFAULTPENDED);
}


/**
 * armv7m_systick_active :
 * @return true if the systick exception is active;
 */

static inline bool armv7m_is_systick_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_SYSTICKACT);
}


/**
 * armv7m_pendsv_active :
 * @return true if the PendSV exception is active;
 */

static inline bool armv7m_is_pendsv_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_PENDSVACT);
}


/**
 * armv7m_monitor_active :
 * @return true if the monitor is active;
 */

static inline bool armv7m_is_monitor_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_MONITORACT);
}


/**
 * armv7m_svc_active :
 * @return true if the svc exception is active;
 */

static inline bool armv7m_is_svc_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_SVCALLACT);
}


/**
 * armv7m_usage_fault_active :
 * @return true if the usage fault exception is active;
 */

static inline bool armv7m_is_usage_fault_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_USGFAULTACT);
}


/**
 * armv7m_bus_fault_active :
 * @return true if the bus fault exception is active;
 */

static inline bool armv7m_is_bus_fault_active() {
	return (bool)(*ARMV7_SHCSR & ARMV7_SHCSR_BUSFAULTACT);
}


/**
 * armv7m_mem_fault_active :
 * @return true if the mem fault exception is active;
 */

static inline bool armv7m_is_mem_fault_active() {
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

#define ARMV7_STIR_INTID ((uint32_t)0x1FF)


/**
 * armv7m_software_trigger_interrupt : writes STIR to trigger the required interrupt;
 *
 * 	The value provided is an interrupt number, not an exception number. System exceptions cannot be triggered;
 *
 * @param interrupt_number : the interrupt number;
 */

static inline void armv7m_software_trigger_interrupt(uint16_t interrupt_number) {

	//If the required interrutp number is greater than the id mask, do nothing;
	if (interrupt_number > ARMV7_STIR_INTID) {
		return;
	}

	//Trigger the required software interrupt;
	*ARMV7_STIR = (uint32_t)interrupt_number;

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


//----------------- CSR - RW : Systick Control and Status Register -----------------

#define ARMV7_SYST_CSR ((volatile uint32_t *) 0xE000E010)

#define ARMV7_SYST_CSR_COUNTFLAG ((uint32_t) (1 << 16))
#define ARMV7_SYST_CSR_CLKSOURCE ((uint32_t) (1 << 2))
#define ARMV7_SYST_CSR_TICKINT ((uint32_t) (1 << 1))
#define ARMV7_SYST_CSR_ENABLE ((uint32_t) (1 << 0))




static inline void armv7m_systick_enable() {
	*ARMV7_SYST_CSR |= ARMV7_SYST_CSR_ENABLE;

}

static inline void armv7m_systick_disable() {
	*ARMV7_SYST_CSR &= ~ARMV7_SYST_CSR_ENABLE;
}


static inline void armv7m_systick_int_enable() {
	*ARMV7_SYST_CSR |= ARMV7_SYST_CSR_TICKINT;
}

static inline void armv7m_systick_int_disable() {
	*ARMV7_SYST_CSR &= ~ARMV7_SYST_CSR_TICKINT;
}

static inline void armv7m_systick_select_core_clock() {
	*ARMV7_SYST_CSR |= ARMV7_SYST_CSR_CLKSOURCE;
}

static inline void armv7m_systick_select_external_clock() {
	*ARMV7_SYST_CSR &= ~ARMV7_SYST_CSR_CLKSOURCE;
}






//----------------- RVR - RW : Systick Reload Value Register -----------------

#define ARMV7_SYST_RVR ((volatile uint32_t *) 0xE000E014)

#define ARMV7_SYST_RVR_MAX ((uint32_t ) 0x00FFFFFF)


static inline void armv7m_systick_set_reload(uint32_t reload24b) {
	*ARMV7_SYST_RVR = reload24b & ARMV7_SYST_RVR_MAX;
}


//----------------- CVR - RW : Systick Current Value Register -----------------

#define ARMV7_SYST_CVR 	((volatile uint32_t *) 0xE000E018)


static inline void armv7m_systick_clear_countflag() {
	*ARMV7_SYST_CVR;
}

static inline uint32_t armv7m_systick_get_count() {
	return *ARMV7_SYST_CVR;
}



//----------------- CVR - RO : Systick Calibration Register -----------------

#define ARMV7_SYST_CALIB ((volatile uint32_t *) 0xE000E01C)


#define ARMV7_SYST_CALIB_NOREF ((uint32_t) (1 << 31))
#define ARMV7_SYST_CALIB_SKEW  ((uint32_t) (1 << 30))
#define ARMV7_SYST_CALIB_TENMS_MASK ((uint32_t) 0x00FFFFFF)



static inline uint32_t armv7m_systick_get_10ms_reload() {
	return (*ARMV7_SYST_CALIB & ARMV7_SYST_CALIB_TENMS_MASK);
}

static inline bool armv7m_systick_10_ms_accurate() {
	return (bool) (*ARMV7_SYST_CALIB & ARMV7_SYST_CALIB_SKEW);
}

static inline bool armv7m_systick_noref() {
	return (bool) (*ARMV7_SYST_CALIB & ARMV7_SYST_CALIB_NOREF);
}

//TODO;


/*
 * ------------------------------------------------------ NVIC ------------------------------------------------------
 */

//----------------- ISER - RW : Interrupt Set Enable Registers. 16 4-byte long registers -----------------

#define ARMV7_NVIC_ISER ((volatile uint32_t *) 0xE000E100)


/*
 * armv7m_nvic_enable_interrupt : enables the required interrupt channel;
 */

static inline void armv7m_nvic_enable_interrupt(uint16_t interrupt_channel) {

	//Cache the address of the related ISER register by dividing the interrupt channel by 32;
	volatile uint32_t *ISER_register = ARMV7_NVIC_ISER + (interrupt_channel >> 5);

	//Determine the bit mask using the remain of the interrupt channel by 32
	*ISER_register = (uint32_t) 1 << (interrupt_channel & (uint16_t) 0x1F);

}


//----------------- ICER - RW : Interrupt Clear Enable Registers. 16 4-byte long registers -----------------

#define ARMV7_NVIC_ICER ((volatile uint32_t *) 0xE000E180)


/*
 * armv7m_nvic_disable_interrupt : disables the required interrupt channel;
 */

static inline void armv7m_nvic_disable_interrupt(uint16_t interrupt_channel) {

	//Cache the address of the related ICER register by dividing the interrupt channel by 32;
	volatile uint32_t *ICER_register = ARMV7_NVIC_ICER + (interrupt_channel >> 5);

	//Determine the bit mask using the remain of the interrupt channel by 32
	*ICER_register = (uint32_t) 1 << (interrupt_channel & (uint16_t) 0x1F);

}


//----------------- ISPR - RW : Interrupt Set Pending Registers. 16 4-byte long registers -----------------

#define ARMV7_NVIC_ISPR ((volatile uint32_t *) 0xE000E200)


/*
 * armv7m_nvic_set_interrupt_pending : disables the required interrupt channel;
 */

static inline void armv7m_nvic_set_interrupt_pending(uint16_t interrupt_channel) {

	//Cache the address of the related ISPR register by dividing the interrupt channel by 32;
	volatile uint32_t *ISPR_register = ARMV7_NVIC_ISPR + (interrupt_channel >> 5);

	//Determine the bit mask using the remain of the interrupt channel by 32
	*ISPR_register = (uint32_t) 1 << (interrupt_channel & (uint16_t) 0x1F);

}


//----------------- ISPR - RW : Interrupt Clear Pending Registers. 16 4-byte long registers -----------------

#define ARMV7_NVIC_ICPR ((volatile uint32_t *) 0xE000E280)


/*
 * armv7m_nvic_clear_interrupt_pending : disables the required interrupt channel;
 */

static inline void armv7m_nvic_clear_interrupt_pending(uint16_t interrupt_channel) {

	//Cache the address of the related ICPR register by dividing the interrupt channel by 32;
	volatile uint32_t *ICPR_register = ARMV7_NVIC_ICPR + (interrupt_channel >> 5);

	//Determine the bit mask using the remain of the interrupt channel by 32
	*ICPR_register = (uint32_t) 1 << (interrupt_channel & (uint16_t) 0x1F);

}

//----------------- ISPR - RW : Interrupt Active Bit Registers. 16 4-byte long registers -----------------

#define ARMV7_NVIC_IABR ((volatile uint32_t *) 0xE000E300)


/*
 * armv7m_nvic_is_interrupt_enabled : asserts if the provided interrupt channel is enabled;
 */

static inline bool armv7m_nvic_is_interrupt_enabled(uint16_t interrupt_channel) {

	//Cache the address of the related ICPR register by dividing the interrupt channel by 32;
	volatile uint32_t *IABR_register = ARMV7_NVIC_IABR + (interrupt_channel >> 5);

	//Determine the bit mask using the remain of the interrupt channel by 32, assert if bit is set;
	return (bool) (*IABR_register & ((uint32_t) 1 << (interrupt_channel & (uint16_t) 0x1F)));

}


//----------------- IPR - RW : Interrupt Priority Registers. 16 4-byte long registers -----------------

#define ARMV7_NVIC_IPR ((volatile uint32_t *) 0xE000E400)

/**
 * armv7m_set_priority : updates the required interrupt channel priority;
 *
 * @param interrupt_channel : the channel to update the priority of;
 * @param priority : the new priority;
 */

static inline void armv7m_nvic_set_priority(uint16_t interrupt_channel, uint8_t priority) {

	//Determine the address of the byte to update, and save the priority in it;
	*((uint8_t *)ARMV7_NVIC_IPR + interrupt_channel) = priority;

}


/**
 * armv7m_get_priority : determines the priority of  the required interrupt channel;
 *
 * @param interrupt_channel : the channel to get the priority of;
 */

static inline uint8_t armv7m_nvic_get_priority(uint16_t interrupt_channel) {

	//Determine the address of the byte to update, and save the priority in it;
	return *((uint8_t *)ARMV7_NVIC_IPR + interrupt_channel);

}



#endif //TRACER_ARM_V7_H
