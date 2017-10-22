#include "config.h"
#ifdef ENABLE_GCODE_INTERFACE

#include "GCodeInterfaceCommands.h"
#include "GCodeInterface.h"

void GCodeInterfaceCommands::g0() {
    GI::echo("SUUSg0");
}

void GCodeInterfaceCommands::g1() {
    GI::echo("SUUSg1");
}

void GCodeInterfaceCommands::g10() {
    GI::echo("SUUSg10");
}

void GCodeInterfaceCommands::g5d0() {
    GI::echo("SUUSg5d0");
}

void GCodeInterfaceCommands::m1() {
    GI::echo("SUUSm1");
}

void GCodeInterfaceCommands::m2() {
    GI::echo("SUUSm2");
}

void GCodeInterfaceCommands::m225() {
    GI::echo("SUUSm225");
}

#endif