#include "Chips/teensy35/teensy35_arch.h"


/*
 * A Macro combination to ease the architecture peripheral classes locationA
 */

//String conversion call
#define STR_MACRO1(x) STR_MACRO(x)

//Stringisation macro
#define STR_MACRO(x) #x

//Identity macro;
#define EXPAND(x) x

//ARCH PATH GENERATOR.
#define ARCH_PATH(peripheral) EXPAND(STR_MACRO1(EXPAND(Kernel/Arch/Chips/)EXPAND(arch)EXPAND(/)EXPAND(peripheral)EXPAND(.h)))
