
#Add the core folder to the core include dir list;
CORE_INC += -Isrc
CORE_INC += -Isrc/core

#Add general core sources;
CORE_SRCS += core/core.c
CORE_SRCS += core/debug.c
CORE_SRCS += core/ram.c
CORE_SRCS += core/startup.c

#Add type sources;
CORE_SRCS += core/type/list.c
CORE_SRCS += core/type/heap.c

#Add the communication protocol;//TODO CONFIG
CORE_SRCS += core/log/probe.c



#----------------------------------------------------- Core config -----------------------------------------------------

#include the teensy35 makefile that will initialise the toolchain environment and define basic rules;
include src/core/board/teensy35/Makefile