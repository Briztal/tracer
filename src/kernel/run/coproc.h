//
// Created by root on 10/17/18.
//

#ifndef TRACER_COPROC_H
#define TRACER_COPROC_H


//--------------------------------------------------- Make Parameters --------------------------------------------------

//The coproc library requires the number of coprocessors to be provided by the makefile;
#if !defined(NB_COPROCESSORS)

//Compilation fail;
#error "Error, one make parameter not provided, check your makefile"

#define NB_COPROCESSORS 2

#define COPROC_0_SIZE 2

#define COPROC_1_SIZE 3

#endif

//------------------------------------------------------ Includes ------------------------------------------------------

#include <stdint.h>
#include <macro/incr_call.h>


//------------------------------------------------ Coprocs contex struct -----------------------------------------------

#define ALIGN(size) (((size) + (sizeof(void *) - 1)) &~((sizeof(void *) - 1)))

#define REGISTER_COPROC_SPACE(i)\
	uint8_t coproc_##i##_context[ALIGN(COPROC_##i##_SIZE)];


//Coproc context saving structure;
struct coprocs_contexts {
	
	//Reserve a space for all coprocessors;
	INCR_CALL(NB_COPROCESSORS, REGISTER_COPROC_SPACE)
	
};


//-------------------------------------------------- Generic headers ---------------------------------------------------
/*
 * A coprocessor defines three symbols :
 * 	- coproc_i_size : an integer, representing the size of its context to save;
 * 	- coproc_i_saver : a function in charge of saving the coprocessor context to the provided location;
 * 	- coproc_i_loader : a function in charge of loading the coprocessor context from the provided location;
 */

#define coproc_name(i, name) coproc_##i##_##name

#define coproc_init(i) coproc_name(i, init)
#define coproc_saver(i) coproc_name(i, saver)
#define coproc_loader(i) coproc_name(i, loader)


//--------------------------------------------------- Context switch ---------------------------------------------------

//Init coprocessors;
void coprocs_init();

//Save contexts;
void coprocs_save_contexts(struct coprocs_contexts *ctxts);

//Load contexts;
void coprocs_load_contexts(struct coprocs_contexts *ctxts);




#endif //TRACER_COPROC_H
