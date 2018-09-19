
MODULES_SRCS += modules/test.c

#MODULES_SRCS += modules/pwm.c


servo_args := -DPERIOD=20000 -DTOLERANCE=10 -DTIMER_NAME=pit_0 -DMODULE_NAME=servo -DCHANNELS_FILE="servo.h"

servo:

#Create a directory for the PORT library;
	@mkdir -p build/mod/

#Log the compilation of the servo module;
	@echo "[CC]\tsrc/modules/servo.c"

#Compile the pit manager;
	@$(CC) $(CFLAGS) $(INC) $(servo_args) -o build/mod/servo.o -c src/modules/servo.c


#Add the servo rule to the module dependencies list;
MODULES += servo

#Add the servo object to the modules list;
MODULES_OBJS += build/mod/servo.o


