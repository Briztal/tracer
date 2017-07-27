
#ifndef TRACER_GCODEINTERFACECOMMANDS_H
#define TRACER_GCODEINTERFACECOMMANDS_H


class GCodeInterfaceCommands {

#ifdef ENABLE_GCODE_INTERFACE

public :

#define GO_UPPER()

#define GO_LOWER(i)

#define COMMAND(i, name)\
    static void name();

#define GO_LOWER_COMMAND(i, fname) COMMAND(i, fname)


#include "gcode_interface_config.h"

#undef GO_UPPER
#undef GO_LOWER
#undef GO_LOWER_COMMAND
#undef COMMAND

#endif
};


#endif //TRACER_GCODEINTERFACECOMMANDS_H
