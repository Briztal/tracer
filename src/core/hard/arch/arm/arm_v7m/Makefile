
#The armv7m uses the general ARM acrhitecture build;
include src/core/hard/arch/arm/Makefile

#add the armv7 directory to the include path;
CORE_INC += -Isrc/core/hard/arch/arm/arm_v7m

ARCH_DIR :=  core/hard/arch/arm/arm_v7m

#Add armv7 and nvic files to core source files;
CORE_SRCS +=  $(ARCH_DIR)/arm_v7m.c
CORE_SRCS +=  $(ARCH_DIR)/arm_v7m_startup.c
CORE_SRCS +=  $(ARCH_DIR)/arm_v7m_systick.c
CORE_SRCS +=  $(ARCH_DIR)/nvic.c




