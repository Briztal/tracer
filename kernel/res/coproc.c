//
// Created by root on 10/17/18.
//

#include "kernel/exec/pmem.h"

//------------------------------------------------ Coprocs declarations ------------------------------------------------

//DECLARE_COPROC : declares all 3 symbols of a coprocessor;

#define DECLARE_COPROC(i) \
	extern void coproc_init(i) ();\
	extern void coproc_saver(i) (void *dst);\
	extern void coproc_loader(i) (const void *src);\

//Declare all symbols of all coprocessors;
INCR_CALL(NB_COPROCESSORS, DECLARE_COPROC)

//Clean
#undef DECLARE_COPROC


//------------------------------------------------- Coprocs operations -------------------------------------------------

/**
 * coprocs_init : calls each coprocessor initializer;
 */

void coprocs_init() {
	
	//Save context : calls the i-th coprocessor's initializer;
	#define INIT(i)\
		 coproc_init(i)();
	
	//Initialize all coprocessors;
	INCR_CALL(NB_COPROCESSORS, INIT)
	
	//Clean
	#undef INIT
}


/**
 * coprocs_save_contexts : calls each coprocessor's context saver;
 *
 * @param mem : the process memory where to store contexts;
 */

void coprocs_save_contexts(struct coprocs_contexts *ctxts) {
	
	//Save context : calls the i-th coprocessor's context saver;
	#define SAVE_CONTEXT(i)\
		 coproc_saver(i) (&(ctxts->coproc_##i##_context));
	
	//Save all coprocessors context;
	INCR_CALL(NB_COPROCESSORS, SAVE_CONTEXT)
	
	//Clean
	#undef SAVE_CONTEXT
}


/**
 * coprocs_save_contexts : calls each coprocessor's context saver;
 *
 * @param mem : the process memory where to store contexts;
 */

void coprocs_load_contexts(struct coprocs_contexts *ctxts) {
	
	//Save context : calls the i-th coprocessor's context saver;
	#define LOAD_CONTEXT(i)\
		 coproc_loader(i) (&(ctxts->coproc_##i##_context));
	
	//Save all coprocessors context;
	INCR_CALL(NB_COPROCESSORS, LOAD_CONTEXT)
	
	//Clean
	#undef LOAD_CONTEXT
	
}