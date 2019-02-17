//
// Created by root on 10/17/18.
//


//-------------------------------------------------- Make Parameters ---------------------------------------------------

/*
 * cortex_m4f.mk must provide :
 * 	- AUTO_STACKING : if set, vfp lower registers will be stacked automatically;
 * 	- LAZY_STACKING : if set, vfp lower registers will be stacked only if FPU is used during an exception;
 */

#if !defined(FPU_COPROC_ID) || !defined(AUTO_STACKING) || !defined(LAZY_STACKING)

#error "One macro parameter was not provided. Check your makefile;"

#define FPU_COPROC_ID 0

#define AUTO_STACKING 1

#define LAZY_STACKING 1

#endif


//------------------------------------------------------ Includes ------------------------------------------------------

#include "../arm_v7m.h"

#include <kernel/res/coproc.h>


//--------------------------------------------------- Implementation ---------------------------------------------------

/**
 * coproc_x_init : initialises the fpu.
 * 	- enables access to coprocessor;
 * 	- Sets up stacking depending on make parameters;
 */

void coproc_init(FPU_COPROC_ID) () {
	
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


/**
 * coproc_x_saver : saves the context of the FPU, in the provided block;
 *
 * 	The provided pointer is the lowest address of the block, and the block is suitably aligned for any transfer size;
 *
 * 	This function is pure assembly, and follows ARM calling convention. Dst will be located in R0
 *
 * @param dst : the lowest address of the memory block to store the context in;
 */

__attribute__ ((naked)) void coproc_saver(FPU_COPROC_ID) (void *dst) {
	
	//If the auto stacking is enabled, we must simply save s16 - s31;
	#ifdef AUTO_STACKING
	
	__asm__ __volatile__ (""
		
		//Transfer the content of {s15 - s31} in the block referenced by r0;
		"vstmia r0, {s15 - s31} \n\t"
	
	);
	
	//If the auto stacking is disabled, we must save all registers, s0 - s31;
	#else
	
	__asm__ __volatile__ (""
		
		//Transfer the content of {s0 - s31} in the block referenced by r0;
		"vstmia r0, {s0 - s31} \n\t"
	
	);
	
	#endif
	
	__asm__ __volatile__ ("bx lr");
	
}


/**
 * coproc_x_loader : loads the context of the FPU, in the provided block;
 *
 * 	The provided pointer is the lowest address of the block, and the block is suitably aligned for any transfer size;
 *
 * 	This function is pure assembly, and follows ARM calling convention. src will be located in R0
 *
 * @param dst : the lowest address of the memory block to store the context in;
 */

__attribute__ ((naked)) void coproc_loader(FPU_COPROC_ID) (const void *src) {
	
	//If the auto stacking is enabled, we must load s16 - s31;
	#ifdef AUTO_STACKING
	
	__asm__ __volatile__ (""
		
		//Transfer the content of {s15 - s31} in the block referenced by r0;
		"vldmia r0, {s15 - s31} \n\t"
	
	);
	
	//If the auto stacking is disabled, we must load all registers, s0 - s31;
	#else
	
	__asm__ __volatile__ (""
		
		//Transfer the content of {s0 - s31} in the block referenced by r0;
		"vldmia r0, {s0 - s31} \n\t"
	
	);
	
	#endif
	
	__asm__ __volatile__ ("bx lr");
	
}
