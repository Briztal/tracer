/*
  arm_v7m_fault.c Part of TRACER

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


#include <stdint.h>

#include <stdbool.h>

#include <kernel/res/printk.h>

#include "arm_v7m.h"


/*---------------------------------------------------- fault env -----------------------------------------------------*/


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

/*--------------------------------------------------- fault repair ---------------------------------------------------*/

/**
 * __flt_repair : attempts to repair a fault, described by the up-to-date fault environment;
 * @return true if the repairs are successful, false if the fault is not recoverable;
 */

bool __flt_repair() {

    //TODO IMPLEMENT;
    return false;

}


/*---------------------------------------------------- Fault Log -----------------------------------------------------*/

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