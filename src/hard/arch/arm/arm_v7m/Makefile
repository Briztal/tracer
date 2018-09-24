
#The armv7m uses the general ARM acrhitecture build;
include src/hard/arch/arm/Makefile


V7M_DIR :=  src/hard/arch/arm/arm_v7m

arm_v7m :
	@echo "compiling ARM_V7M module\n"
	@$(CC) $(CFLAGS) $(INC) -o build/hard/arm_v7m.o -c $(V7M_DIR)/arm_v7m.c
	@$(CC) $(CFLAGS) $(INC) -o build/hard/arm_v7m_startup.o -c $(V7M_DIR)/arm_v7m_startup.c
	@$(CC) $(CFLAGS) $(INC) -o build/hard/arm_v7m_systick.o -c $(V7M_DIR)/arm_v7m_systick.c
	@$(CC) $(CFLAGS) $(INC) -o build/hard/nvic.o -c $(V7M_DIR)/nvic.c




