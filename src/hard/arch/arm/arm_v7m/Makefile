
#The armv7m uses the general ARM acrhitecture build;
include src/hard/arch/arm/Makefile

#add the armv7 directory to the include path;
CORE_INC += -Isrc/hard/arch/arm/arm_v7m

ARCH_DIR :=  hard/arch/arm/arm_v7m

#Add armv7 and nvic files to core source files;
HARD_SRCS +=  $(ARCH_DIR)/arm_v7m.c
HARD_SRCS +=  $(ARCH_DIR)/arm_v7m_startup.c
HARD_SRCS +=  $(ARCH_DIR)/arm_v7m_systick.c
HARD_SRCS +=  $(ARCH_DIR)/nvic.c




