#The cortex M4 is based on the armv7 architecture;
include src/hard/arch/arm/arm_v7m/Makefile

CPUARCH := cortex-m4

#Compiler options for C;
CFLAGS += -mthumb -MMD -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

# linker options
LDFLAGS +=  --specs=nano.specs -mcpu=cortex-m4 -mthumb
LDFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

HARD_SRCS += hard/proc/cortex_m4f/cortex_m4f.c