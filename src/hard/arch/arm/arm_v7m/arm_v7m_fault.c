//
// Created by root on 10/5/18.
//

#include <stdint.h>
#include <stdbool.h>

#include <kernel/async/fault.h>

#include <kernel/debug/log.h>
#include <kernel/mod/auto_mod.h>

#include "arm_v7m.h"


#define DETECT_FAULT(var, mask, msg, f_type) {\
	if ((var) & (mask)) {\
    	kernel_log_(msg);\
		FAULT_ESCALATE(fault, f_type);\
	}\
}


#define DETECT_REC_FAULT(var, mask, msg) DETECT_FAULT(var, mask, msg, RECOVERABLE_FAULT)



/**
 * check_MMFSR : reads, analyses and clears the mem_manage fsr;
 * @return false if a non recoverable error is found;
 */

static enum fault_type check_HFSR() {
	
	//Cache the MemManage Fault Status Register;
	uint32_t HFSR = *ARMV7_HFSR;
	
	//Write one to clear
	*ARMV7_HFSR = (uint32_t) -1;
	
	//Cache the found fault type;
	enum fault_type fault = NO_FAULT;
	
	//vector table read error;
	DETECT_REC_FAULT(HFSR, HFSR_VECTTBL, "error reading the vector table");
	
	//vector table read error;
	DETECT_REC_FAULT(HFSR, HFSR_FORCED, "Configurable fault disabled, escalated in hardfault");
	
	//vector table read error;
	DETECT_REC_FAULT(HFSR, HFSR_DEBUGEVT, "Debug event");
	
	//Return the highest fault level found;
	return fault;
	
}


/**
 * check_MMFSR : reads, analyses and clears the mem_manage fsr;
 * @return false if a non recoverable error is found;
 */

static enum fault_type check_MMFSR() {
	
	//Cache the MemManage Fault Status Register;
	uint8_t MMFSR = *ARMV7_MMFSR;
	
	//Write one to clear
	*ARMV7_MMFSR = (uint8_t) -1;
	
	//Cache the found fault type;
	enum fault_type fault = NO_FAULT;
	
	//instruction access violation;
	DETECT_REC_FAULT(MMFSR, MMFSR_IACCVIOL, "Instruction access violation");
	
	//Data access violation;
	if (MMFSR & MMFSR_DACCVIOL) {
	
		//the MMFAR has valid content :
		if (MMFSR & MMFSR_MMARKVALID) {
			
			//Cache the MM address;
			uint32_t MMFAR = *ARMV7_MMFAR;
			
			//Log;
			kernel_log("Data access violation : %h", MMFAR);
			
		} else {
			
			//MMFAR has invalid content, log;
			kernel_log_("Data access violation, invalid memory indicator");
			
		}
		
	}
	
	//mem manage error on exception return;
	DETECT_REC_FAULT(MMFSR, MMFSR_MUNSTKERR, "Derived MemManage fault on exception return");
	
	//mem manage error on exception entry;
	DETECT_REC_FAULT(MMFSR, MMFSR_MSTKERR, "Derived MemManage fault on exception entry");
	
	
	//Return the highest fault level found;
	return fault;
	
}


/**
 * check_BFSR : reads, analyses and clears the mem_manage fsr;
 * @return false if a non recoverable error is found;
 */

static enum fault_type check_BFSR() {
	
	//Cache the MemManage Fault Status Register;
	uint8_t BFSR = *ARMV7_BFSR;
	
	//Write one to clear
	*ARMV7_BFSR = (uint8_t) -1;
	
	//Cache the found fault type;
	enum fault_type fault = NO_FAULT;
	
	
	//bus fault on instruction prefetch
	DETECT_REC_FAULT(BFSR, BFSR_IBUSERR,"Bus fault on instruction prefetch");
	
	//precise data access error :
	if (BFSR & BFSR_PRECISERR) {
		
		//the BFAR has valid content :
		if (BFSR & MMFSR_MMARKVALID) {
			
			//Cache the BFAR content;
			uint32_t BFAR = *ARMV7_BFAR;
			
			//Log;
			kernel_log("Bus precise data access error : %h", BFAR);
			
			
		} else {
			
			//MMFAR has invalid content, log;
			kernel_log_("Bus precise data error, invalid memory indicator");
			
		}
		
		//Escalate;
		FAULT_ESCALATE(fault, RECOVERABLE_FAULT);
		
	}
	
	//imprecise data access error :
	DETECT_REC_FAULT(BFSR, BFSR_IMPRECISERR,"Bus imprecise data access error");
	
	//derived bus fault on exception return :
	DETECT_REC_FAULT(BFSR, BFSR_UNSTKERR, "Derived bus fault on exception return");
	
	//derived bus fault on exception entry:
	DETECT_REC_FAULT(BFSR, BFSR_STKERR, "Derived bus fault on exception entry");
	
	
	//Return the highest fault level found;
	return fault;
	
}



/**
 * check_BFSR : reads, analyses and clears the mem_manage fsr;
 * @return false if a non recoverable error is found;
 */

static enum fault_type check_UFSR() {
	
	//Cache the Usage Fault Status Register;
	uint16_t UFSR = *ARMV7_UFSR;
	
	//Write one to clear
	*ARMV7_UFSR = (uint16_t) -1;
	
	//Cache the found fault type;
	enum fault_type fault = NO_FAULT;
	
	
	//an undefined instruction is found :
	DETECT_REC_FAULT(UFSR, UFSR_UNDEFINSTR, "Undefined instruction");
	
	//invalid EPSR.T bit or illegal EPSR.IT bits for executing instruction :
	DETECT_REC_FAULT(UFSR, UFSR_INVSTATE, "invalid EPSR.T bit or illegal EPSR.IT bits");
	
	//integrity check error on EXEC_RETURN :
	DETECT_REC_FAULT(UFSR, UFSR_INVPC, "integrity check error on EXEC_RETURN");
	
	//coprocessor access error :
	DETECT_REC_FAULT(UFSR, UFSR_NOCP, "coprocessor access error");
	
	//unaligned access error :
	DETECT_REC_FAULT(UFSR, UFSR_UNALIGNED, "unaligned access error");
	
	//Division by zero :
	DETECT_REC_FAULT(UFSR, UFSR_DIVBYZERO, "Division by zero");
	
	
	//Return the highest fault level found;
	return fault;
	
}



static enum fault_type fault_handler(uint32_t type) {
	
	enum fault_type fault = NO_FAULT;
	
	enum fault_type tmp_fault;
	
	tmp_fault = check_HFSR();
	
	FAULT_ESCALATE(fault, tmp_fault);
	
	tmp_fault = check_MMFSR();
	
	FAULT_ESCALATE(fault, tmp_fault);
	
	tmp_fault = check_BFSR();
	
	FAULT_ESCALATE(fault, tmp_fault);
	
	tmp_fault = check_UFSR();
	
	FAULT_ESCALATE(fault, tmp_fault);
	
	return fault;
	
}


static bool armv7_fault_init() {
	
	kernel_init_fault_analyser(&fault_handler);
	
	return true;
	
}

KERNEL_EMBED_MODULE(PROC_MODULE, armv7_fault, &armv7_fault_init)