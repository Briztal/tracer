#define architecture teensy35

//String conversion call
#define STR_MACRO1(x) STR_MACRO(x)

//String conversion macro
#define STR_MACRO(x) #x

//Identity macro;
#define EXPAND(x) x

//arch PATH GENERATOR.
#define ARCH_PATH(type) EXPAND(STR_MACRO1(EXPAND(Kernel/arch/boards/)EXPAND(architecture)EXPAND(_)EXPAND(type)EXPAND(.h)))
