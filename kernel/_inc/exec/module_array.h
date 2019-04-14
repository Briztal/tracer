

#ifndef TRACER_CH_NAME_H
#define TRACER_CH_NAME_H

#include <macro/incr_call.h>

/*
 * This file must be included in the source file of a channel module, whose purpose is to be compiled once
 * 	for each channel;
 *
 * 	It checks that standard channel parameters (MODULE_NAME and CHANNEL_ID) are defined, and errors if not;
 *
 * 	It then defines macro utilities that create global symbols for channel data;
 */

/*If MODULE_NAME and CHANNEL_ID are not present :*/
#if !defined(MODULE_NAME) || !defined(NB_CHANNELS)

#error "MODULE_NAME or NB_CHANNELS not provided; Check your makefile"

/*Define them, so that non makefile IDE allow debug;*/
#define MODULE_NAME NAME

#define NB_CHANNELS 3

#endif


/*---------------------------------------------------- String macro ----------------------------------------------------*/

#define _STR(x) #x

#define STR(x) _STR(x)


/*--------------------------------------------------- Symbol creation --------------------------------------------------*/

/*Concatenate the module name, "_ch_" and the channel id. No argument expansion is made;*/
#define _REF_SYMB(module_name, channel_id) module_name##_##channel_id

/*Expand arguments before concatenating the result;*/
#define REFERENCE_SYMBOL(module_name, channel_id) _REF_SYMB(module_name, channel_id)


/*------------------------------------------------ Specs array creation ------------------------------------------------*/

/*Declare the i-th channel data. Must match a compiled channel;*/
#define _DECLARE_CHANNEL(i) extern const struct channel_specs REFERENCE_SYMBOL(MODULE_NAME, i);

/*Provide the reference of the i-th channel;*/
#define _REFERENCE_CHANNEL(i) &REFERENCE_SYMBOL(MODULE_NAME, i),

/*Create an array of channel_specs pointers, initialised with all channels references;*/
#define MODULE_CREATE_SPECS_ARRAY(name)\
INCR_CALL(NB_CHANNELS, _DECLARE_CHANNEL)\
static const struct channel_specs *name[NB_CHANNELS] = {INCR_CALL(NB_CHANNELS, _REFERENCE_CHANNEL)};


/*------------------------------------------------ Function referencing ------------------------------------------------*/


/*Create a global symbol aliasing the provided function. Type must "channel_specs";*/
#define MODULE_REFERENCE_FUNCTION(function, ret, args)\
	ret REFERENCE_SYMBOL(MODULE_NAME, function) args __attribute__((alias(#function)));

/*void f () __attribute__ ((weak, alias ("__f")));*/



#endif /*TRACER_CH_NAME_H*/
