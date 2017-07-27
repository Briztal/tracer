
#ifndef TRACER_TREEINTERFACECOMMANDS_H
#define TRACER_TREEINTERFACECOMMANDS_H


#include <stdint.h>

class TreeInterfaceCommands {

#define TI_STD_ARGS char *dptr, uint8_t size

#ifdef ENABLE_TREE_INTERFACE

    public:

    static void system_canal_function(TI_STD_ARGS);

#define GO_UPPER

#define GO_LOWER(i, name)

#define CREATE_LEAF(i, name)\
    static void name(char * dptr, uint8_t size);

#define CREATE_CALLABLE_LEAF(i, name)\
    CREATE_LEAF(i, name)

#include "tree_interface_config.h"

#undef GO_UPPER
#undef GO_LOWER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF


    static void parameters_system_canal(TI_STD_ARGS);

    static void pid_system_canal(TI_STD_ARGS);

    static void loop_system_canal(TI_STD_ARGS);

    static void actions_system_canal(TI_STD_ARGS);

    static void steppers_system_canal(TI_STD_ARGS);

    static void EEPROM_system_canal(TI_STD_ARGS);

#endif

};


#endif //TRACER_TREEINTERFACECOMMANDS_H
