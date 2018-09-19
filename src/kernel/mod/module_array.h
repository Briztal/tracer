//
// Created by root on 9/19/18.
//

#ifndef TRACER_CH_NAME_H
#define TRACER_CH_NAME_H

#include <util/macro/incr_call.h>

/*
 * This file must be included in the source file of a channel module, whose purpose is to be compiled once
 * 	for each channel;
 *
 * 	It checks that standard channel parameters (MODULE_NAME and CHANNEL_ID) are defined, and errors if not;
 *
 * 	It then defines macro utilities that create global symbols for channel data;
 */

//If MODULE_NAME and CHANNEL_ID are not present :
#if !defined(MODULE_NAME) || !defined(NB_CHANNELS)

#error "MODULE_NAME or NB_CHANNELS not provided; Check your makefile"

//Define them, so that non makefile IDE allow debug;
#define MODULE_NAME NAME

#define NB_CHANNELS 3

#endif



//Concatenate the module name, "_ch_" and the channel id. No argument expansion is made;
#define _CH_SYMB(module_name, channel_id) module_name##_ch_##channel_id

//Expand arguments before concatenating the result;
#define CHANNEL_SYMBOL(module_name, channel_id) _CH_SYMB(module_name, channel_id)


//Declare the i-th channel data. Must match a compiled channel;
#define _DECLARE_CHANNEL(i) extern const struct channel_specs CHANNEL_SYMBOL(MODULE_NAME, i);

//Declare all channels. Uses NB_CHANNELS;
#define MODULE_DECLARE_CHANNELS() INCR_CALL(NB_CHANNELS, _DECLARE_CHANNEL)

//Provide the reference of the i-th channel;
#define _REFERENCE_CHANNEL(i) &CHANNEL_SYMBOL(MODULE_NAME, i),

//Create an array of channel_specs pointers, initialised with all channels references;
#define MODULE_CREATE_ARRAY(name) static const struct channel_specs *name[NB_CHANNELS] = {INCR_CALL(NB_CHANNELS, _REFERENCE_CHANNEL)};

#endif //TRACER_CH_NAME_H
