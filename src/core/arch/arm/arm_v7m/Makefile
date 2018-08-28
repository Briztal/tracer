
#The armv7m uses the general ARM acrhitecture build;
include src/core/arch/arm/Makefile

ARCH_DIR :=  core/arch/arm/arm_v7m

#add the armv7 directory to the include path;
COMPIL_INC += -Isrc/core/arch/arm/arm_v7m

#Add armv7 and nvic files to core source files;
CORE_SRCS +=  $(ARCH_DIR)/arm_v7m.c
CORE_SRCS +=  $(ARCH_DIR)/arm_v7m_startup.c
CORE_SRCS +=  $(ARCH_DIR)/nvic.c




