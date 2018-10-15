#The cortex M4 is based on the armv7 architecture;
include src/hard/arch/arm/arm_v7m/Makefile


#Proc compiler options;
CFLAGS += -mcpu=cortex-m4
#-mfloat-abi=hard -mfpu=fpv4-sp-d16

#Proc linker options
LDFLAGS += --specs=nano.specs
#-mfpu=fpv4-sp-d16 -mfloat-abi=hard --specs=nano.specs


cortex_m4f : arm_v7m
	@echo "compiling CORTEX_M4F module\n"
#@$(CC) $(CFLAGS) $(INC) -o build/hard/cortex_m4f.o -c src/hard/sched/cortex_m4f/cortex_m4f.c
