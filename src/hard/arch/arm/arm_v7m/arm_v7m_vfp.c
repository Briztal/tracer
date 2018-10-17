//
// Created by root on 10/17/18.
//



/*
 * TODO :
 *
 * 	FPU power on (coprocessor access and priority);
 *
 * 	FPU context saver registration;
 *
 * 	FPU stack_data provider registration;
 */



/*
 * Makefile must provide :
 * 	- AUTO_STACKING : if set, vfp lower registers will be stacked automatically;
 * 	- LAZY_STACKING : if set, vfp lower registers will be stacked only if FPU is used during an exception;
 */


#include "arm_v7m.h"

#if !defined(AUTO_STACKING) || !defined(LAZY_STACKING)

#error "One macro parameter was not provided. Check your makefile;"

#define AUTO_STACKING 1

#define LAZY_STACKING 1

#endif


static void fpu_init() {
	
	//First, provide full access to both coprocessors 10 and 11;
	arm_v7m_set_coprocessor_access_priv(10, ARMV7_CPACR_FULL_ACCESS);
	arm_v7m_set_coprocessor_access_priv(11, ARMV7_CPACR_FULL_ACCESS);
	
	//FPCCR must be updated. Cache the current value;
	uint32_t FPCCR = *ARMV7_FPCCR;
	
	//If the automatic stacking is enabled :
	#if (AUTO_STACKING > 0)
	
	//Set the ASPEN bit in FPCCR;
	FPCCR |= FPCCR_ASPEN;
	
	{
		//If the lazy stacking is enabled :
		#if (LAZY_STACKING > 0)
		
		//Set the LSPEN bit in FPCCR;
		FPCCR |= FPCCR_LSPEN;
		
		#else //(LAZY_STACKING > 0)
		
		//Clear the LSPEN in FPCCR;
		FPCCR &= ~FPCCR_LSPEN;
		
		#endif //(LAZY_STACKING > 0)
		
	}
	
	#else //(AUTO_STACKING > 0)
	
	//Clear both ASPEN and LSPEN bits in FPCCR;
	FPCCR &= ~(FPCCR_ASPEN | FPCCR_LSPEN);
	
	#endif //(AUTO_STACKING > 0)
	
	//Write back;
	*ARMV7_FPCCR = FPCCR;
	
	
}

