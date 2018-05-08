

#include "Kernel/arch/boards/teensy35.h"

/*
 * A Macro combination to ease the architecture peripheral classes locations
 */

//String conversion call
#define STR_MACRO1(x) STR_MACRO(x)

//String conversion macro
#define STR_MACRO(x) #x

//Identity macro;
#define EXPAND(x) x

//arch PATH GENERATOR.
#define ARCH_PATH(peripheral) EXPAND(STR_MACRO1(EXPAND(Kernel/Arch/Chips/)EXPAND(arch)EXPAND(/)EXPAND(peripheral)EXPAND(.h)))
